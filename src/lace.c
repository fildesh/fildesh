/** \file lace.c
 *
 * This code is written by Alex Klinkhamer.
 * It uses the ISC license (see the LICENSE file in the top-level directory).
 **/

#include "lace.h"
#include "lace_builtin.h"
#include "lace_compat_errno.h"
#include "lace_compat_fd.h"
#include "lace_posix_thread.h"
#include "utilace.h"
#include "cx/associa.h"
#include "cx/fileb.h"

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


enum SymValKind
{
  IDescVal, ODescVal, IODescVal,
  IDescArgVal,
  IDescFileVal, ODescFileVal,
  IFutureDescVal, OFutureDescVal,
  IFutureDescFileVal, OFutureDescFileVal,
  HereDocVal, IHereDocFileVal,
  DefVal,
  NSymValKinds
};
enum CommandKind {
  RunCommand, HereDocCommand,
  StdinCommand, StdoutCommand,
  DefCommand,
  NCommandKinds
};

typedef enum SymValKind SymValKind;
typedef enum CommandKind CommandKind;

typedef struct SymVal SymVal;
typedef struct Command Command;
typedef struct BuiltinCommandThreadArg BuiltinCommandThreadArg;

DeclTableT( Command, Command );
DeclTableT( SymVal, SymVal );
DeclTableT( iargs, struct { int fd; bool scrap_newline; } );

struct Command
{
  char* line;
  uint line_num;
  CommandKind kind;
  TableT(cstr) args;
  TableT(cstr) extra_args;
  TableT(cstr) tmp_files;
  pthread_t thread;
  pid_t pid;
  int status;
  int stdis; /**< Standard input stream.**/
  TableT( int ) is; /**< Input streams.**/
  int stdos; /**< Standard output stream.**/
  TableT( int ) os; /** Output streams.**/
  /** If >= 0, this is a file descriptor that will
   * close when the program command is safe to run.
   **/
  int exec_fd;
  /** If != 0, this is the contents of a file to execute.**/
  const char* exec_doc;

  /** Use these input streams to fill corresponding (null) arguments.**/
  TableT( iargs ) iargs;

  /** Use this if it's a HERE document.**/
  char* doc;
};

struct SymVal
{
  AlphaTab name;
  SymValKind kind;
  uint arg_idx;  /**< If a file.**/
  uint ios_idx;
  uint cmd_idx;
  union SymVal_union
  {
    int file_desc;
    char* here_doc;
  } as;
};

struct BuiltinCommandThreadArg {
  Command* command;  /* Cleanup but don't free.*/
  char** argv;  /* Free nested.*/
};

  static void
init_SymVal (SymVal* v)
{
  v->name = dflt_AlphaTab ();
  v->kind = NSymValKinds;
}

  static void
lose_SymVal (SymVal* v)
{
  v->name = dflt_AlphaTab ();
  v->kind = NSymValKinds;
}

  static void
init_Command (Command* cmd)
{
  cmd->kind = NCommandKinds;
  cmd->line_num = 0;
  InitTable( cmd->args );
  InitTable( cmd->extra_args );
  InitTable( cmd->tmp_files );
  cmd->pid = -1;
  cmd->exec_fd = -1;
  cmd->exec_doc = 0;
  cmd->stdis = -1;
  InitTable( cmd->is );
  cmd->stdos = -1;
  InitTable( cmd->os );
  InitTable( cmd->iargs );
}

  static void
close_Command (Command* cmd)
{
  unsigned i;
  if (cmd->stdis >= 0) {
    lace_compat_fd_close(cmd->stdis);
    cmd->stdis = -1;
  }
  if (cmd->stdos >= 0) {
    lace_compat_fd_close(cmd->stdos);
    cmd->stdos = -1;
  }
  for (i = 0; i < cmd->is.sz; ++i) {
    lace_compat_fd_close(cmd->is.s[i]);
  }
  LoseTable( cmd->is );
  InitTable( cmd->is );

  for (i = 0; i < cmd->os.sz; ++i) {
    lace_compat_fd_close(cmd->os.s[i]);
  }
  LoseTable( cmd->os );
  InitTable( cmd->os );

  LoseTable( cmd->iargs );
  InitTable( cmd->iargs );

  cmd->exec_fd = -1;
  cmd->exec_doc = 0;
}

  static void
inherit_fds_Command(Command* cmd)
{
  size_t i;
  if (cmd->stdis >= 0) {
    lace_compat_fd_inherit(cmd->stdis);
  }
  if (cmd->stdos >= 0) {
    lace_compat_fd_inherit(cmd->stdos);
  }
  for (i = 0; i < cmd->is.sz; ++i) {
    lace_compat_fd_inherit(cmd->is.s[i]);
  }
  for (i = 0; i < cmd->os.sz; ++i) {
    lace_compat_fd_inherit(cmd->os.s[i]);
  }
  if (cmd->exec_fd >= 0) {
    lace_compat_fd_inherit(cmd->exec_fd);
  }
}

  static void
lose_Command (Command* cmd)
{
  uint i;
  close_Command (cmd);
  free (cmd->line);
  switch (cmd->kind) {
    case DefCommand:
    case RunCommand:
    case StdinCommand:
    case StdoutCommand:
      LoseTable( cmd->args );
      break;
    case HereDocCommand:
      free (cmd->doc);
      break;
    default:
      break;
  }

  UFor( i, cmd->extra_args.sz )
    free (cmd->extra_args.s[i]);
  LoseTable( cmd->extra_args );

  UFor( i, cmd->tmp_files.sz ) {
    remove (cmd->tmp_files.s[i]);
    free (cmd->tmp_files.s[i]);
  }
  LoseTable( cmd->tmp_files );
  cmd->kind = NCommandKinds;
}

  static void
lose_Commands (TableT(Command)* cmds)
{
  unsigned i;
  for (i = 0; i < cmds->sz; ++i) {
    if (cmds->s[i].kind == RunCommand && cmds->s[i].pid > 0)
      kill_please_sysCx(cmds->s[i].pid);
    if (cmds->s[i].kind != NCommandKinds)
      lose_Command (&cmds->s[i]);
  }
  LoseTable (*cmds);
  free(cmds);
}


  static SymVal*
getf_SymVal (Associa* map, const char* s)
{
  size_t sz = map->nodes.sz;
  AlphaTab ts = dflt1_AlphaTab (s);
  Assoc* assoc = ensure_Associa (map, &ts);
  SymVal* x = (SymVal*) val_of_Assoc (map, assoc);

  if (map->nodes.sz > sz) {
    init_SymVal (x);
    x->name = ts;
  }
  return x;
}

  static uint
count_ws (const char* s)
{
  return strspn (s, WhiteSpaceChars);
}
  static uint
count_non_ws (const char* s)
{
  return strcspn (s, WhiteSpaceChars);
}
  static uint
trim_trailing_ws (char* s)
{
  uint n = strlen (s);
  while (0 < n && strchr (WhiteSpaceChars, s[n-1]))  --n;
  s[n] = '\0';
  return n;
}

  static void
failout_Command (const Command* cmd, const char* msg, const char* msg2)
{
  if (msg && msg2) {
    lace_log_errorf("Problem on line %u. %s: %s", cmd->line_num, msg, msg2);
  } else if (msg) {
    lace_log_errorf("Problem on line %u. %s.", cmd->line_num, msg);
  } else {
    lace_log_errorf("Problem on line %u.", cmd->line_num);
  }
  lose_sysCx ();
  exit (1);
}

/** HERE document is created by
 * $(H var_name) Optional identifying stuff.
 * Line 1 in here.
 * Line 2 in here.
 * ...
 * Line n in here.
 * $(H var_name) Optional identifying stuff.
 *
 * OR it could look like:
 * $(H: var_name) value
 **/
  static char*
parse_here_doc (LaceX* in, const char* term, size_t* text_nlines)
{
  char* s;
  const size_t term_length = strlen(term);
  LaceX slice;
  LaceX* content;

  /* Check for the single-line case.*/
  if (term[3] == ':')
  {
    term = strchr (term, ')');
    if (!term)  return dup_cstr ("");
    term = &term[1];
    term = &term[count_ws (term)];
    s = dup_cstr (term);
    trim_trailing_ws (s);
    return s;
  }

  content = open_LaceXA();
  for (slice = sliceline_LaceX(in);
       slice.at;
       slice = sliceline_LaceX(in))
  {
    *text_nlines += 1;
    if (slice.size >= term_length) {
      if (0 == memcmp(term, slice.at, term_length)) {
        break;
      }
    }
    memcpy(grow_LaceX(content, slice.size), slice.at, slice.size);
    *grow_LaceX(content, 1) = '\n';
  }
  if (content->size > 0) {content->size -= 1;}
  *grow_LaceX(content, 1) = '\0';

  s = content->at;
  content->at = NULL;
  close_LaceX(content);
  return s;
}

  static char*
parse_line (LaceX* xf, size_t* text_nlines)
{
  AlphaTab line = DEFAULT_AlphaTab;
  char* s;

  while ((s = getline_LaceX(xf)))
  {
    uint n;
    bool multiline = false;
    *text_nlines += 1;

    s = &s[count_ws (s)];
    if (s[0] == '#' || s[0] == '\0')  continue;

    n = trim_trailing_ws (s);

    multiline = s[n-1] == '\\';
    if (multiline)
      --n;

    cat1_cstr_AlphaTab (&line, s, n);

    if (!multiline)  break;
  }
  return forget_AlphaTab (&line);
}

  static void
sep_line (TableT(cstr)* args, char* s)
{
  while (1)
  {
    s = &s[count_ws (s)];
    if (s[0] == '\0')  break;

    if (s[0] == '\'') {
      uint i;
      s = &s[1];
      PushTable( *args, s );
      i = strcspn (s, "'");
      if (s[i] == '\0') {
        lace_log_warning("Unterminated single quote.");
        break;
      }
      s = &s[i];
    }
    else if (s[0] == '"') {
      uint i = 0;
      uint j = 0;
      s = &s[1];
      PushTable( *args, s );

      while (s[j] != '\0' && s[j] != '"') {
        if (s[j] == '\\') {
          j += 1;
#define C( c, d )  case c: s[j] = d; break;
          switch (s[j])
          {
            C( 'n', '\n' );
            C( 't', '\t' );
            default: break;
          }
#undef C
        }
        s[i] = s[j];
        i += 1;
        j += 1;
      }

      if (s[j] == '\0') {
        lace_log_warning("Unterminated double quote.");
        break;
      }
      j += 1;
      s[i] = '\0';
      s = &s[j];
    }
    else if (s[0] == '$' && s[1] == '(') {
      uint i;
      PushTable( *args, s );
      s = &s[2];
      i = strcspn (s, ")");
      if (s[i] == '\0') {
        lace_log_warning("Unterminated variable.");
        break;
      }
      s = &s[i+1];
    }
    else {
      PushTable( *args, s );
      s = &s[count_non_ws (s)];
    }
    if (s[0] == '\0')  break;
    s[0] = '\0';
    s = &s[1];
  }
  PackTable( *args );
}

  static void
parse_file(TableT(Command)* cmds, LaceX* in, const char* this_filename)
{
  size_t text_nlines = 0;
  while (true)
  {
    char* line;
    Command* cmd;
    line = parse_line(in, &text_nlines);
    if (!line) {
      break;
    }
    else if (line[0] == '\0') {
      free (line);
      break;
    }
    cmd = Grow1Table( *cmds );
    init_Command (cmd);
    cmd->line = line;
    cmd->line_num = text_nlines;

    if (line[0] == '$' && line[1] == '(' &&
        line[2] == 'H' && line[3] != 'F')
    {
      cmd->kind = HereDocCommand;
      cmd->doc = parse_here_doc(in, line, &text_nlines);
    }
    else if (pfxeq_cstr ("$(<<", line))
    {
      char* filename = &line[4];
      LaceX* src = NULL;

      filename = &filename[count_ws (filename)];
      filename[strcspn (filename, ")")] = '\0';

      src = open_sibling_LaceXF(this_filename, filename);
      if (!src) {
        failout_Command(cmd, "Failed to include file", filename);
      }

      lose_Command (TopTable( *cmds ));
      MPopTable( *cmds, 1 );

      parse_file(cmds, src, filename_LaceXF(src));
      close_LaceX(src);
    }
    else if (pfxeq_cstr ("$(>", line) ||
        pfxeq_cstr ("$(set", line))
    {
      char* begline;
      char* sym = line;

      cmd->kind = RunCommand;

      sym = &sym[count_non_ws (sym)];
      sym = &sym[count_ws(sym)];
      begline = strchr (sym, ')');
      if (!begline) {
        failout_Command (cmd, "Unclosed paren in variable def.", 0);
      }

      begline[0] = '\0';
      begline = &begline[1];

      PushTable( cmd->args, (char*) "zec" );
      PushTable( cmd->args, (char*) "/" );
      sep_line (&cmd->args, begline);
      PushTable( cmd->args, (char*) "/" );

      {
        AlphaTab oname = DEFAULT_AlphaTab;
        cat_cstr_AlphaTab (&oname, "$(O ");
        cat_cstr_AlphaTab (&oname, sym);
        cat_cstr_AlphaTab (&oname, ")");
        PushTable( cmd->extra_args, forget_AlphaTab (&oname));
      }
      PushTable( cmd->args, *TopTable( cmd->extra_args ) );

      cmd = Grow1Table( *cmds );
      init_Command (cmd);
      cmd->kind = DefCommand;
      cmd->line_num = text_nlines;
      PushTable( cmd->args, (char*) "elastic" );
      {
        AlphaTab xname = DEFAULT_AlphaTab;
        cat_cstr_AlphaTab (&xname, "$(X ");
        cat_cstr_AlphaTab (&xname, sym);
        cat_cstr_AlphaTab (&xname, ")");
        PushTable( cmd->extra_args, forget_AlphaTab (&xname));
      }
      PushTable( cmd->args, *TopTable( cmd->extra_args ) );
      cmd->line = dup_cstr (sym);
    }
    else
    {
      cmd->kind = RunCommand;
      sep_line (&cmd->args, cmd->line);
    }
  }
}

  static SymValKind
parse_sym (char* s, bool firstarg)
{
  uint i, o;
  SymValKind kind = NSymValKinds;

  if (firstarg && s[0] == '|') {
    if (s[1] == '-')
      kind = IODescVal;
    else if (s[1] == '<')
      kind = ODescVal;
    else if (s[1] == '>')
      kind = IDescVal;

    if (kind != NSymValKinds) {
      s[0] = '-';
      s[1] = '\0';
    }
    return kind;
  }

  if (!(s[0] == '$' && s[1] == '('))  return NSymValKinds;

  i = count_non_ws (s);
  if (s[i] == '\0') {
    uint n = i-1;

    if (s[n] != ')')
      return NSymValKinds;

    i = 2;
    n -= 2;
    memmove (s, &s[i], n);
    s[n] = '\0';
    return HereDocVal;
  }

  /* Offset into string.*/
  o = 2;

  if (s[o] == 'X')
  {
    if (s[o+1] == 'O')
    {
      kind = IODescVal;
    }
    else if (s[o+1] == 'A')
    {
      kind = IDescArgVal;
    }
    else if (s[o+1] == 'F')
    {
      if (s[o+2] == 'v')  kind = IFutureDescFileVal;
      else                kind = IDescFileVal;
    }
    else
    {
      if (s[o+1] == 'v')  kind = IFutureDescVal;
      else                kind = IDescVal;
    }
  }
  else if (s[o] == 'O')
  {
    if (s[o+1] == 'F')
    {
      if (s[o+2] == '^')  kind = OFutureDescFileVal;
      else                kind = ODescFileVal;
    }
    else
    {
      if (s[o+1] == '^')  kind = OFutureDescVal;
      else                kind = ODescVal;
    }
  }
  else if (s[o] == 'H')
  {
    if (s[o+1] == 'F')  kind = IHereDocFileVal;
    else                kind = HereDocVal;
  }

  if (kind != NSymValKinds)
  {
    uint n;
    i += count_ws (&s[i]);
    n = strcspn (&s[i], ")");
    if (s[i+n] == ')')
    {
      memmove (s, &s[i], n * sizeof (char));
      s[n] = '\0';
    }
    else
    {
      kind = NSymValKinds;
    }
  }
  return kind;
}

  static uint
add_ios_Command (Command* cmd, int in, int out)
{
  uint idx = UINT_MAX;
  if (in >= 0) {
    idx = cmd->is.sz;
    PushTable( cmd->is, in );
  }

  if (out >= 0) {
    idx = cmd->os.sz;
    PushTable( cmd->os, out );
  }
  return idx;
}

  static void
add_iarg_Command (Command* cmd, int in, bool scrap_newline)
{
  add_ios_Command (cmd, in, -1);
  GrowTable( cmd->iargs, 1 );
  TopTable( cmd->iargs )->fd = in;
  TopTable( cmd->iargs )->scrap_newline = scrap_newline;
}

  static char*
add_extra_arg_Command (Command* cmd, const char* s)
{
  PushTable( cmd->extra_args, dup_cstr (s) );
  return *TopTable( cmd->extra_args );
}

  static char*
add_fd_arg_Command (Command* cmd, int fd)
{
  char buf[1024];
  sprintf (buf, "/dev/fd/%d", fd);
  return add_extra_arg_Command (cmd, buf);
}

  static char*
add_tmp_file_Command (Command* cmd, uint x, const char* tmpdir)
{
  char buf[1024];
  sprintf (buf, "%s/%u", tmpdir, x);
  PushTable( cmd->tmp_files, dup_cstr (buf) );
  return cmd->tmp_files.s[cmd->tmp_files.sz - 1];
}

/** Write a file /name/ given contents /doc/.**/
  static void
write_here_doc_file (const char* name, const char* doc)
{
  uint n;
  FILE* out;

  out = fopen (name, "wb");
  if (!out) {
    lace_log_errorf("Cannot open file for writing!: %s", name);
    return;
  }
  n = strlen (doc);
  fwrite (doc, sizeof (char), n, out);
  fclose (out);
}

  static void
setup_commands (TableT(Command)* cmds,
    const char* tmpdir)
{
  uint ntmp_files = 0;
  Associa map[1];
  Associa add_map[1]; /* Temporarily hold new symbols for the current line.*/
  Assoc* assoc;
  uint i;

  InitAssocia( AlphaTab, SymVal, *map, cmp_AlphaTab );
  InitAssocia( AlphaTab, SymVal, *add_map, cmp_AlphaTab );

  UFor( i, cmds->sz ) {
    uint arg_q = 0;
    uint arg_r = 0;
    Command* cmd = &cmds->s[i];

    /* The command defines a HERE document.*/
    if (cmd->kind == HereDocCommand)
    {
      SymVal* sym;
      SymValKind kind;

      /* The loops should not run.*/
      Claim2( cmd->args.sz ,==, 0 ); /* Invariant.*/

      kind = parse_sym (cmd->line, false);
      Claim2( kind ,==, HereDocVal);
      sym = getf_SymVal (map, cmd->line);
      sym->kind = kind;
      sym->as.here_doc = cmd->doc;
    }

    for (arg_r = 0; arg_r < cmd->args.sz; ++ arg_r) {
      char* arg = cmd->args.s[arg_r];
      if (arg_q == 0 && eq_cstr("stdin", arg)) {
        cmd->kind = StdinCommand;
        break;
      }
      else if (arg_q == 0 && eq_cstr("stdout", arg)) {
        cmd->kind = StdoutCommand;
        break;
      }
    }

    for (arg_r = 0; arg_r < cmd->args.sz; ++ arg_r)
    {
      char* arg = cmd->args.s[arg_r];
      const SymValKind kind = parse_sym (arg, (arg_r == 0));


      if (kind == HereDocVal || kind == IDescArgVal)
      {
        SymVal* sym = getf_SymVal (map, arg);
        if (sym->kind == HereDocVal) {
          cmd->args.s[arg_q] = sym->as.here_doc;
        }
        else if (sym->kind == ODescVal) {
          fd_t fd = sym->as.file_desc;
          sym->kind = NSymValKinds;
          add_iarg_Command (cmd, fd, true);
          cmd->args.s[arg_q] = 0;
        }
        else if (sym->kind == DefVal) {
          fd_t fd[2];
          Command* xcmd = &cmds->s[sym->cmd_idx];

          if (0 != lace_compat_fd_pipe(&fd[1], &fd[0]))
            failout_Command (cmd, "Failed to create pipe for variable", arg);

          add_ios_Command (xcmd, -1, fd[1]);
          PushTable( xcmd->args, add_fd_arg_Command (xcmd, fd[1]) );

          add_iarg_Command (cmd, fd[0], true);
          cmd->args.s[arg_q] = 0;
        }
        else {
          failout_Command (cmd, "Unknown source for argument", arg);
        }
        ++ arg_q;
      }
      else if (cmd->kind == StdoutCommand && kind == IDescVal)
      {
        SymVal* sym = getf_SymVal (map, arg);
        Command* last = &cmds->s[sym->cmd_idx];

        if (last->kind != RunCommand)
          failout_Command (cmd, "Stdout stream not coming from a command?", arg);

        lace_compat_fd_close(sym->as.file_desc);

        if (sym->ios_idx < last->os.sz) {
          lace_compat_fd_move_to(last->os.s[sym->ios_idx], 1);
        }
        else {
          lace_compat_fd_close(last->stdos);
          last->stdos = 1;
        }
        sym->kind = NSymValKinds;
      }
      else if (kind == IDescVal || kind == IDescFileVal ||
          kind == IODescVal || kind == IHereDocFileVal)
      {
        SymVal* sym = getf_SymVal (map, arg);
        int fd;
        if (kind == IHereDocFileVal)
        {
          if (sym->kind != HereDocVal) {
            failout_Command (cmd, "Unknown HERE doc", arg);
          }
        }
        else
        {
          if (sym->kind != ODescVal) {
            failout_Command (cmd, "Unknown source for", arg);
          }
          sym->kind = NSymValKinds;
        }
        fd = sym->as.file_desc;

        if (kind == IDescVal || kind == IODescVal)
        {
          cmd->stdis = fd;
        }
        else if (kind == IDescFileVal)
        {
          add_ios_Command (cmd, fd, -1);
          if (arg_q > 0)
          {
            cmd->args.s[arg_q] = add_fd_arg_Command (cmd, fd);
          }
          else
          {
            cmd->args.s[0] = add_tmp_file_Command (cmd, ntmp_files, tmpdir);
            ++ ntmp_files;
            if (sym->arg_idx < UINT_MAX)
              cmds->s[sym->cmd_idx].args.s[sym->arg_idx] = cmd->args.s[0];
            cmd->exec_fd = fd;
          }
          ++ arg_q;
        }
        else
        {
          if (kind != IHereDocFileVal) {
            failout_Command (cmd, "Not a HERE doc file?", arg);
          }
          cmd->args.s[arg_q] =
            add_tmp_file_Command (cmd, ntmp_files, tmpdir);
          ++ ntmp_files;
          /* Write the temp file now.*/
          write_here_doc_file (cmd->args.s[arg_q],
              sym->as.here_doc);
          if (arg_q == 0)
            cmd->exec_doc = sym->as.here_doc;
          ++ arg_q;
        }
      }
      else if (kind == OFutureDescVal || kind == OFutureDescFileVal)
      {
        SymVal* sym = getf_SymVal (map, arg);
        int fd;

        if (sym->kind != IFutureDescVal)
          failout_Command (cmd, "Argument should be a stream to the past", arg);

        sym->kind = NSymValKinds;
        fd = sym->as.file_desc;

        if (kind == OFutureDescVal)
        {
          cmd->stdos = fd;
        }
        else
        {
          add_ios_Command (cmd, fd, -1);
          if (sym->arg_idx > 0)
          {
            cmd->args.s[arg_q] = add_fd_arg_Command (cmd, fd);
          }
          else
          {
            char* s;
            s = add_tmp_file_Command (&cmds->s[sym->cmd_idx],
                ntmp_files, tmpdir);
            ++ ntmp_files;
            cmds->s[sym->cmd_idx].args.s[0] = s;
            cmd->args.s[arg_q] = s;
          }
          ++ arg_q;
        }
      }

      if (cmd->kind == StdinCommand && kind == ODescVal)
      {
        SymVal* sym = getf_SymVal (add_map, arg);
        sym->kind = ODescVal;
        sym->cmd_idx = i;
        sym->as.file_desc = lace_compat_fd_duplicate(0);
        InitDomMax( sym->arg_idx );
        InitDomMax( sym->ios_idx );
      }
      else if (kind == ODescFileVal && eq_cstr (arg, "VOID")) {
        cmd->args.s[arg_q] = add_extra_arg_Command (cmd, "/dev/null");
        arg_q += 1;
      }
      else if (kind == ODescVal || kind == ODescFileVal ||
          kind == IODescVal)
      {
        fd_t fd[2];
        SymVal* sym = getf_SymVal (add_map, arg);
        sym->kind = ODescVal;
        sym->cmd_idx = i;
        InitDomMax( sym->arg_idx );
        InitDomMax( sym->ios_idx );

        if (0 != lace_compat_fd_pipe(&fd[1], &fd[0]))
          failout_Command (cmd, "Failed to create pipe for variable", arg);
        assert(fd[0] >= 0);
        assert(fd[1] >= 0);

        sym->as.file_desc = fd[0];

        if (kind == ODescVal || kind == IODescVal) {
          cmd->stdos = fd[1];
        } else {
          sym->ios_idx = add_ios_Command (cmd, -1, fd[1]);
          cmd->args.s[arg_q] = add_fd_arg_Command (cmd, fd[1]);
          sym->arg_idx = arg_q;
          ++ arg_q;
        }
      }
      else if (kind == IFutureDescVal || kind == IFutureDescFileVal)
      {
        fd_t fd[2];
        SymVal* sym = getf_SymVal (add_map, arg);
        sym->kind = IFutureDescVal;
        sym->cmd_idx = i;
        InitDomMax( sym->arg_idx );
        InitDomMax( sym->ios_idx );

        if (0 != lace_compat_fd_pipe(&fd[1], &fd[0]))
          failout_Command (cmd, "Failed to create pipe for variable", arg);

        sym->as.file_desc = fd[1];
        if (kind == IFutureDescVal)
        {
          cmd->stdis = fd[0];
        }
        else
        {
          sym->ios_idx = add_ios_Command (cmd, -1, fd[0]);
          cmd->args.s[arg_q] = add_fd_arg_Command (cmd, fd[0]);
          if (arg_q == 0)
          {
            sym->arg_idx = 0;
            cmd->exec_fd = fd[0];
          }
          ++ arg_q;
        }
      }

      if (kind == NSymValKinds) {
        cmd->args.s[arg_q] = arg;
        ++ arg_q;
      }
    }

    if (cmd->args.sz > 0)
      cmd->args.sz = arg_q;

    if (cmd->kind == DefCommand) {
      SymVal* sym = getf_SymVal (map, cmd->line);
      sym->kind = DefVal;
      sym->cmd_idx = i;
    }

    assoc = beg_Associa (add_map);
    if (assoc) do {
      AlphaTab* add_key = (AlphaTab*) key_of_Assoc (map, assoc);
      SymVal* add_sym = (SymVal*) val_of_Assoc (map, assoc);
      Assoc* tmp_assoc = assoc;
      SymVal* sym = getf_SymVal (map, ccstr_of_AlphaTab (add_key));

      if (!(sym->kind==NSymValKinds || sym->kind==HereDocVal || sym->kind==DefVal)) {
        failout_Command (cmd, "Trying to overwrite an existing stream variable",
            ccstr_of_AlphaTab (add_key));
      }

      *sym = *add_sym;
      assoc = next_Assoc (assoc);
      give_Associa (add_map, tmp_assoc);
    } while (assoc);
  }

  for (assoc = beg_Associa (map);
      assoc;
      assoc = next_Assoc (assoc))
  {
    SymVal* x = (SymVal*) val_of_Assoc (map, assoc);
    if (x->kind == ODescVal)
    {
      lace_log_errorf("Dangling output stream! Symbol: %s", x->name.s);
      failout_sysCx ("");
    }
    lose_SymVal (x);
  }

  lose_Associa (map);
  lose_Associa (add_map);
}

  static void
output_Command(FILE* out, const Command* cmd)
{
  unsigned i;
  if (cmd->kind != RunCommand)  return;

  fputs ("COMMAND: ", out);
  for (i = 0; i < (unsigned)cmd->args.sz; ++i) {
    if (i > 0)  fputc(' ', out);
    if (cmd->args.s[i]) {
      fputs(cmd->args.s[i], out);
    } else {
      fputs("NULL", out);
    }
  }
  fputc('\n', out);
}

/** Add the utility bin directory to the PATH environment variable.**/
  static void
add_util_path_env ()
{
  static const char k[] = "PATH";
#ifndef UtilBin
#define UtilBin "bin"
#endif
  static const char path[] = UtilBin;
#undef UtilBin
  tacenv_sysCx (k, path);
}

  static void
show_usage_and_exit ()
{
  fprintf(stderr, "Usage: %s [[-f] SCRIPTFILE | -- SCRIPT]\n",
          exename_of_sysCx ());
  failout_sysCx(0);
}

  static void
remove_tmppath (AlphaTab* tmppath)
{
  if (!rmdir_sysCx (cstr_AlphaTab (tmppath)))
    lace_log_warningf("Temp directory not removed: %s", cstr_AlphaTab(tmppath));
  lose_AlphaTab (tmppath);
  free(tmppath);
}


int main_best_match(unsigned argc, char** argv);
#ifdef LACE_BUILTIN_PERMIT_ELASTIC_AIO
int main_elastic_aio(unsigned argc, char** argv);
#endif
#ifdef LACE_BUILTIN_PERMIT_ELASTIC_POLL
int main_elastic_poll(unsigned argc, char** argv);
#endif
int main_execfd(unsigned argc, char** argv);
int main_godo(unsigned argc, char** argv);
int main_lace(unsigned argc, char** argv);
int main_ssh_all(unsigned argc, char** argv);
int main_transpose(unsigned argc, char** argv);
int main_ujoin(unsigned argc, char** argv);
int main_waitdo(unsigned argc, char** argv);
int main_xpipe(unsigned argc, char** argv);

static int lace_main_add(unsigned argc, char** argv) {
  return lace_builtin_add_main(argc, argv, NULL, NULL);
}
static int lace_main_cmp(unsigned argc, char** argv) {
  return lace_builtin_cmp_main(argc, argv, NULL, NULL);
}
static int lace_main_elastic_pthread(unsigned argc, char** argv) {
  return lace_builtin_elastic_pthread_main(argc, argv, NULL, NULL);
}
static int lace_main_seq(unsigned argc, char** argv) {
  return lace_builtin_seq_main(argc, argv, NULL, NULL);
}
static int lace_main_time2sec(unsigned argc, char** argv) {
  return lace_builtin_time2sec_main(argc, argv, NULL, NULL);
}
static int lace_main_void(unsigned argc, char** argv) {
  return lace_builtin_void_main(argc, argv, NULL, NULL);
}
static int lace_main_zec(unsigned argc, char** argv) {
  return lace_builtin_zec_main(argc, argv, NULL, NULL);
}

static int lace_main_elastic(unsigned argc, char** argv) {
#if defined(LACE_BUILTIN_PERMIT_ELASTIC_AIO)
  return main_elastic_aio(argc, argv);
#elif defined(LACE_BUILTIN_PERMIT_ELASTIC_POLL)
  return main_elastic_poll(argc, argv);
#else
  return lace_main_elastic_pthread(argc, argv);
#endif
}

bool lace_builtin_is_threadsafe(const char* name)
{
  static const char* const all[] = {
    "add",
    "cmp",
#if !defined(LACE_BUILTIN_PERMIT_ELASTIC_AIO) && !defined(LACE_BUILTIN_PERMIT_ELASTIC_POLL)
    "elastic",
#endif
    "elastic_pthread",
    "seq",
    "time2sec",
    "void",
    "zec",
    NULL,
  };
  unsigned i;
  for (i = 0; all[i]; ++i) {
    if (0 == strcmp(name, all[i])) {
      return true;
    }
  }
  return false;
}

int (*lace_specific_util (const char* arg)) (unsigned, char**)
{
  typedef struct LaceBuiltinMap LaceBuiltinMap;
  struct LaceBuiltinMap {
    const char* name;
    int (*main_fn)(unsigned,char**);
  };
  static const LaceBuiltinMap builtins[] = {
    {"add", lace_main_add},
    {"best-match", main_best_match},
    {"cmp", lace_main_cmp},
    {"elastic", lace_main_elastic},
    {"elastic_pthread", lace_main_elastic_pthread},
#ifdef LACE_BUILTIN_PERMIT_ELASTIC_AIO
    {"elastic_aio", main_elastic_aio},
#endif
#ifdef LACE_BUILTIN_PERMIT_ELASTIC_POLL
    {"elastic_poll", main_elastic_poll},
#endif
    {"execfd", main_execfd},
    {"godo", main_godo},
    {"lace", main_lace},
    {"seq", lace_main_seq},
    {"ssh-all", main_ssh_all},
    {"time2sec", lace_main_time2sec},
    {"transpose", main_transpose},
    {"ujoin", main_ujoin},
    {"void", lace_main_void},
    {"waitdo", main_waitdo},
    {"xpipe", main_xpipe},
    {"zec", lace_main_zec},
    {NULL, NULL},
  };
  unsigned i;

  for (i = 0; builtins[i].name; ++i) {
    if (0 == strcmp(builtins[i].name, arg)) {
      return builtins[i].main_fn;
    }
  }
  return NULL;
}

int lace_builtin_main(unsigned argc, char** argv)
{
  int (*f) (unsigned, char**);
  f = lace_specific_util(argv[0]);
  if (!f) {
    lace_log_errorf("Unknown tool name: %s", argv[0]);
    return -1;
  }
  return f(argc, argv);
}

static
  void*
builtin_command_thread_fn(BuiltinCommandThreadArg* st)
{
  typedef struct LaceBuiltinMainMap LaceBuiltinMainMap;
  struct LaceBuiltinMainMap {
    const char* name;
    int (*main_fn)(unsigned, char**, LaceX**, LaceO**);
  };
  static const LaceBuiltinMainMap builtins[] = {
    {"add", lace_builtin_add_main},
    {"cmp", lace_builtin_cmp_main},
#if !defined(LACE_BUILTIN_PERMIT_ELASTIC_AIO) && !defined(LACE_BUILTIN_PERMIT_ELASTIC_POLL)
    {"elastic", lace_builtin_elastic_pthread_main},
#endif
    {"elastic_pthread", lace_builtin_elastic_pthread_main},
    {"seq", lace_builtin_seq_main},
    {"time2sec", lace_builtin_time2sec_main},
    {"void", lace_builtin_void_main},
    {"zec", lace_builtin_zec_main},
    {NULL, NULL},
  };
  Command* cmd = st->command;
  unsigned offset = 0;
  unsigned argc;
  char** argv;
  LaceX** inputs = NULL;
  LaceO** outputs = NULL;
  int (*main_fn)(unsigned, char**, LaceX**, LaceO**) = NULL;
  unsigned i;

  assert(cmd->iargs.sz == 0);
  assert(cmd->exec_fd < 0);

  for (argc = 0; st->argv[argc]; ++argc) {
    if (offset == 0 && 0 == strcmp("-as", st->argv[argc])) {
      offset = argc + 1;
    }
  }

  argc -= offset;
  argv = &st->argv[offset];
  inputs = (LaceX**) malloc(sizeof(LaceX*) * (argc+1));
  outputs = (LaceO**) malloc(sizeof(LaceO*) * (argc+1));
  for (i = 0; i <= argc; ++i) {
    inputs[i] = NULL;
    outputs[i] = NULL;
  }
  if (cmd->stdis >= 0) {
    inputs[0] = open_fd_LaceXF(cmd->stdis);
    cmd->stdis = -1;
  }
  if (cmd->stdos >= 0) {
    outputs[0] = open_fd_LaceOF(cmd->stdos);
    cmd->stdos = -1;
  }

  for (i = 0; builtins[i].name; ++i) {
    if (0 == strcmp(argv[0], builtins[i].name)) {
      main_fn = builtins[i].main_fn;
      break;
    }
  }
  if (main_fn) {
    cmd->status = main_fn(argc, argv, inputs, outputs);
  } else {
    cmd->status = -1;
  }

  /* `main_fn()` should have closed all files passed to it.*/
  cmd->is.sz = 0;
  cmd->os.sz = 0;
  for (i = 0; i < argc; ++i) {
    assert(!inputs[i]);
    assert(!outputs[i]);
  }

  /* Free memory.*/
  free(inputs);
  free(outputs);
  close_Command(cmd);
  for (i = 0; st->argv[i]; ++i) {
    free(st->argv[i]);
  }
  free(st->argv);
  free(st);
  return NULL;
}

static
  void
fix_known_flags_Command(Command* cmd) {
  if (eq_cstr("sed", cmd->args.s[0])) {
    uint i;
    for (i = 1; i < cmd->args.sz; ++i) {
      const char* arg = cmd->args.s[i];
      if (eq_cstr("--line-buffered", arg)) {
#ifdef __APPLE__
        const char line_buffering_flag[] = "-l";
#else
        const char line_buffering_flag[] = "-u";
#endif
        cmd->args.s[i] = add_extra_arg_Command(cmd, line_buffering_flag);
      }
    }
  }
}


  static void
spawn_commands (TableT(Command) cmds)
{
  DeclTable( cstr, argv );
  DeclTable( uint2, fdargs );
  uint i;

  lace_compat_fd_cloexec(0);
  lace_compat_fd_cloexec(1);

  for (i = 0; i < cmds.sz; ++i)
  {
    Command* cmd = &cmds.s[i];
    bool use_thread = false;
    uint argi, j;

    if (cmd->kind != RunCommand && cmd->kind != DefCommand)  continue;

    for (argi = 0; argi < cmd->args.sz; ++argi)
    {
      if (!cmd->args.s[argi])
      {
        uint2 p;
        Claim2( fdargs.sz ,<, cmd->iargs.sz );
        p.s[0] = argi;
        p.s[1] = cmd->iargs.s[fdargs.sz].fd;
        PushTable( fdargs, p );
      }
    }
    Claim2( fdargs.sz ,==, cmd->iargs.sz );
    if (cmd->exec_fd >= 0)
    {
      uint2 p;
      p.s[0] = 0;
      p.s[1] = cmd->exec_fd;
      PushTable( fdargs, p );
    }

    PushTable( argv, dup_cstr (exename_of_sysCx ()) );
    PushTable( argv, dup_cstr (MagicArgv1_sysCx) );

    if (cmd->stdis >= 0)
    {
      PushTable( argv, dup_cstr ("-stdxfd") );
      PushTable( argv, itoa_dup_cstr (cmd->stdis) );
    }
    if (cmd->stdos >= 0)
    {
      PushTable( argv, dup_cstr ("-stdofd") );
      PushTable( argv, itoa_dup_cstr (cmd->stdos) );
    }

    if (fdargs.sz > 0) {
      PushTable( argv, dup_cstr ("--") );
      PushTable( argv, dup_cstr ("-as") );
      PushTable( argv, dup_cstr ("execfd") );
      PushTable( argv, dup_cstr ("-exe") );
      PushTable( argv, dup_cstr (cmd->args.s[0]) );

      for (j = 0; j < fdargs.sz; ++j)
      {
        uint2 p = fdargs.s[j];
        PushTable( cmd->extra_args, itoa_dup_cstr (p.s[1]) );
        cmd->args.s[p.s[0]] = *TopTable( cmd->extra_args );
        PushTable( argv, itoa_dup_cstr (p.s[0]) );
      }

      PushTable( argv, dup_cstr ("--") );
      PushTable( argv, dup_cstr (cmd->args.s[0]) );
    }
    else if (lace_specific_util (cmd->args.s[0])) {
      use_thread = lace_builtin_is_threadsafe(cmd->args.s[0]);
      PushTable( argv, dup_cstr ("--") );
      PushTable( argv, dup_cstr ("-as") );
      PushTable( argv, dup_cstr (cmd->args.s[0]));
    }
    else {
      fix_known_flags_Command(cmd);
      PushTable( argv, dup_cstr ("-exec") );
      PushTable( argv, dup_cstr (cmd->args.s[0]));
      PushTable( argv, dup_cstr ("--") );
    }

    for (j = 1; j < cmd->args.sz; ++j)
      PushTable( argv, dup_cstr (cmd->args.s[j]) );

    PushTable( argv, 0 );

    if (cmd->exec_doc)
    {
      cmd->exec_doc = 0;
      chmodu_sysCx (cmd->args.s[0], true, true, true);
    }

    if (use_thread) {
      int istat;
      BuiltinCommandThreadArg* arg = AllocT(BuiltinCommandThreadArg, 1);
      arg->command = cmd;
      arg->argv = DupliT(char*, argv.s, argv.sz);
      cmd->pid = 0;
      istat = pthread_create(&cmd->thread, NULL,
                             (void* (*) (void*)) builtin_command_thread_fn,
                             arg);
      if (istat < 0) {
        lace_log_errorf("Could not pthread_create(). File: %s", argv.s[0]);
        failout_sysCx(0);
      }
    } else {
      inherit_fds_Command(cmd);
      cmd->pid = spawnvp_sysCx (argv.s);
      close_Command(cmd);
      if (cmd->pid < 0) {
        lace_log_errorf("Could not spawnvp(). File: %s", argv.s[0]);
        failout_sysCx(0);
      }
      for (argi = 0; argi < argv.sz; ++argi)
        free (argv.s[argi]);
    }
    fdargs.sz = 0;
    argv.sz = 0;
  }
  LoseTable( argv );
  LoseTable( fdargs );
}


int main(int argc, char** argv)
{
  int argi = init_sysCx(&argc, &argv);
  int ret = main_lace(argc-(argi-1), &argv[argi-1]);
  lose_sysCx();
  return ret;
}


int main_lace(unsigned argc, char** argv)
{
  DeclTable( AlphaTab, script_args );
  TableT(Command)* cmds = NULL;
  const char* stdin_sym = 0;
  const char* stdout_sym = 0;
  bool use_stdin = true;
  AlphaTab* tmppath = NULL;
  LaceX* in = NULL;
  unsigned argi = 1;
  unsigned i;
  int istat;

  /* add_util_path_env (); */
  (void) add_util_path_env;

  signal(SIGINT, lose_sysCx);
  signal(SIGSEGV, lose_sysCx);
#ifdef LACE_POSIX_SOURCE
  signal(SIGQUIT, lose_sysCx);
  /* We already detect closed pipes when write() returns <= 0.*/
  signal(SIGPIPE, SIG_IGN);
#endif

  while (argi < argc) {
    const char* arg;
    arg = argv[argi++];
    if (eq_cstr (arg, "--")) {
      use_stdin = false;
      if (argi >= argc)  show_usage_and_exit ();
      if (!in) {
        in = open_LaceXA();
      }
      while (argi < argc) {
        size_t sz;
        arg = argv[argi++];
        sz = strlen(arg);
        memcpy(grow_LaceX(in, sz), arg, sz);
        *grow_LaceX(in, 1) = '\n';
      }
    }
    else if (eq_cstr (arg, "-as")) {
      return lace_builtin_main(argc-argi, &argv[argi]);
    }
    else if (eq_cstr (arg, "-stdio")) {
      arg = argv[argi++];
      stdin_sym = arg;
      stdout_sym = arg;
    }
    else if (eq_cstr (arg, "-stdin")) {
      stdin_sym = argv[argi++];
    }
    else if (eq_cstr (arg, "-stdout")) {
      stdout_sym = argv[argi++];
    }
    else if (eq_cstr (arg, "-stdinfile")) {
      const char* stdin_filepath = argv[argi++];
      fd_t fd = open_lace_xfd(stdin_filepath);
      if (fd >= 0) {
        istat = lace_compat_fd_move_to(0, fd);
        lace_compat_fd_inherit(0);
        if (istat != 0) {
          failout_sysCx("Failed to dup2 -stdinfile.");
        }
      } else {
        failout_sysCx("Failed to open -stdinfile.");
      }
    }
    else if (eq_cstr (arg, "-stdoutfile")) {
      const char* stdout_filepath = argv[argi++];
      fd_t fd = open_lace_ofd(stdout_filepath);
      if (fd >= 0) {
        istat = lace_compat_fd_move_to(1, fd);
        lace_compat_fd_inherit(1);
        if (istat != 0) {
          failout_sysCx("Failed to dup2 -stdoutfile.");
        }
      } else {
        failout_sysCx("Failed to open -stdoutfile.");
      }
    }
    else if (eq_cstr (arg, "-stderrfile")) {
      const char* stdout_filepath = argv[argi++];
      fd_t fd = open_lace_ofd(stdout_filepath);
      if (fd >= 0) {
        istat = lace_compat_fd_move_to(2, fd);
        lace_compat_fd_inherit(2);
        if (istat != 0) {
          failout_sysCx("Failed to dup2 -stderrfile.");
        }
      } else {
        failout_sysCx("Failed to open -stderrfile.");
      }
    }
    else {
      /* Optional -f flag.*/
      if (eq_cstr (arg, "-x") || eq_cstr (arg, "-f")) {
        if (argi >= argc)  show_usage_and_exit ();
        arg = argv[argi++];
      }
      if (eq_cstr (arg, "-"))
        break;
      use_stdin = false;
      PushTable( script_args, cons1_AlphaTab (arg) );
      in = open_LaceXF(arg);
      if (!in) {
        lace_log_errorf("Cannot read script. File: %s", arg);
        failout_sysCx(0);
      }
      break;
    }
  }

  tmppath = AllocT(AlphaTab, 1);
  *tmppath = cons1_AlphaTab ("lace");
  mktmppath_sysCx (tmppath);
  lace_compat_errno_clear();
  if (tmppath->sz == 0)
    failout_sysCx ("Unable to create temp directory...");
  push_losefn1_sysCx ((void (*) (void*)) remove_tmppath, tmppath);
  cmds = AllocT(TableT(Command), 1);
  InitTable(*cmds);
  push_losefn1_sysCx ((void (*) (void*)) lose_Commands, cmds);

  if (use_stdin) {
    in = open_LaceXF("-");
    PushTable( script_args, cons1_AlphaTab ("-") );
  }

  while (argi < argc) {
    const char* arg = argv[argi++];
    PushTable( script_args, cons1_AlphaTab (arg) );
  }

  if (script_args.sz > 0)
  {
    Command* cmd = Grow1Table( *cmds );
    AlphaTab line = DEFAULT_AlphaTab;
    AlphaTab doc = DEFAULT_AlphaTab;
    cat_cstr_AlphaTab (&line, "$(H: #)");
    cat_uint_AlphaTab (&doc, script_args.sz-1);

    init_Command (cmd);
    cmd->kind = HereDocCommand;
    cmd->line_num = 0;
    cmd->line = forget_AlphaTab (&line);
    cmd->doc = forget_AlphaTab (&doc);

    while (script_args.sz < 10) {
      PushTable( script_args, cons1_AlphaTab ("") );
    }
  }

  UFor( i, script_args.sz ) {
    Command* cmd = Grow1Table( *cmds );
    AlphaTab line = DEFAULT_AlphaTab;
    cat_cstr_AlphaTab (&line, "$(H: ");
    cat_uint_AlphaTab (&line, i);
    cat_cstr_AlphaTab (&line, ")");

    init_Command (cmd);
    cmd->kind = HereDocCommand;
    cmd->line_num = 0;
    cmd->line = forget_AlphaTab (&line);
    cmd->doc = forget_AlphaTab (&script_args.s[i]);
  }
  LoseTable( script_args );

  if (stdin_sym) {
    Command* cmd = Grow1Table( *cmds );
    AlphaTab line = DEFAULT_AlphaTab;
    init_Command (cmd);
    cat_cstr_AlphaTab (&line, "$(O ");
    cat_cstr_AlphaTab (&line, stdin_sym);
    cat_cstr_AlphaTab (&line, ") stdin");
    cmd->line = forget_AlphaTab (&line);
    cmd->kind = RunCommand;
    sep_line (&cmd->args, cmd->line);
  }

  lace_compat_errno_trace();
  parse_file(cmds, in, filename_LaceXF(in));
  close_LaceX(in);
  lace_compat_errno_trace();

  if (stdout_sym) {
    Command* cmd = Grow1Table( *cmds );
    AlphaTab line = DEFAULT_AlphaTab;
    init_Command (cmd);
    cat_cstr_AlphaTab (&line, "$(X ");
    cat_cstr_AlphaTab (&line, stdout_sym);
    cat_cstr_AlphaTab (&line, ") stdout");
    cmd->line = forget_AlphaTab (&line);
    cmd->kind = RunCommand;
    sep_line (&cmd->args, cmd->line);
  }
  PackTable( *cmds );

  lace_compat_errno_trace();
  setup_commands (cmds, ccstr_of_AlphaTab (tmppath));
  lace_compat_errno_trace();


  if (false)
    for (i = 0; i < cmds->sz; ++i)
      output_Command (stderr, &cmds->s[i]);

  lace_compat_errno_trace();
  spawn_commands (*cmds);
  lace_compat_errno_trace();

  istat = 0;
  for (i = 0; i < cmds->sz; ++i) {
    Command* cmd = &cmds->s[i];
    if (cmd->kind == RunCommand || cmd->kind == DefCommand) {
      if (cmd->pid == 0) {
        pthread_join(cmd->thread, NULL);
      } else {
        waitpid_sysCx(cmd->pid, &cmd->status);
      }
      cmd->pid = -1;
      if (cmd->status != 0) {
        fputs("FAILED ", stderr);
        output_Command(stderr, cmd);
        if (istat < 127) {
          /* Not sure what to do here. Just accumulate.*/
          istat += 1;
        }
      }
    }

    lose_Command(cmd);
  }
  lace_compat_errno_trace();
  return istat;
}

