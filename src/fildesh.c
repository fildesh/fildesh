/** \file fildesh.c
 *
 * This code is written by Alex Klinkhamer.
 * It uses the ISC license (see the LICENSE file in the top-level directory).
 **/

#include "fildesh.h"
#include "fildesh_builtin.h"
#include "fildesh_compat_errno.h"
#include "fildesh_compat_fd.h"
#include "fildesh_compat_file.h"
#include "fildesh_compat_sh.h"
#include "fildesh_compat_string.h"
#include "fildesh_posix_thread.h"

#include "parse_fildesh.h"

#include "cx/syscx.h"
#include "cx/alphatab.h"
#include "cx/associa.h"
#include "cx/table.h"

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


enum SymValKind
{
  IDescVal, ODescVal, IODescVal,
  ODescStatusVal,
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
  BarrierCommand,
  NCommandKinds
};


typedef enum SymValKind SymValKind;
typedef enum CommandKind CommandKind;
typedef struct SymVal SymVal;
typedef struct Command Command;
typedef struct CommandHookup CommandHookup;
typedef struct BuiltinCommandThreadArg BuiltinCommandThreadArg;

DeclTableT( Command, Command );
DeclTableT( SymVal, SymVal );
DeclTableT( iargs, struct { int fd; bool scrap_newline; } );


static SymValKind parse_sym(char* s, bool firstarg);

struct Command
{
  char* line;
  unsigned line_num;
  CommandKind kind;
  TableT(cstr) args;
  TableT(cstr) tmp_files;
  pthread_t thread;
  fildesh_compat_pid_t pid;
  int status;
  fildesh_fd_t stdis; /**< Standard input stream.**/
  TableT( int ) is; /**< Input streams.**/
  fildesh_fd_t stdos; /**< Standard output stream.**/
  TableT( int ) os; /** Output streams.**/
  /* Exit status stream.**/
  fildesh_fd_t status_fd;
  /** File descriptor to close upon exit.**/
  TableT( int ) exit_fds;
  /** If >= 0, this is a file descriptor that will
   * close when the program command is safe to run.
   **/
  fildesh_fd_t exec_fd;
  /** Whether exec_fd actually has bytes, rather than just used for signaling.*/
  bool exec_fd_has_bytes;
  /** If != NULL, this is the contents of a file to execute.**/
  const char* exec_doc;

  /** Use these input streams to fill corresponding (null) arguments.**/
  TableT( iargs ) iargs;

  /** Use this if it's a HERE document.**/
  char* doc;

  /** Use this to allocate stuff.
   * Currently just extra args and elements of tmp_files.
   **/
  FildeshAlloc* alloc;
};

/* See the setup_commands() phase.*/
struct CommandHookup {
  char* temporary_directory;
  unsigned tmpfile_count;
  Associa map;
  Associa add_map; /* Temporarily hold new symbols for the current line.*/
  fildesh_fd_t stdin_fd;
  fildesh_fd_t stdout_fd;
  /* Stderr stays at fd 2 but should be closed explicitly if we dup2 over it.*/
  bool stderr_fd_opened;
};

struct SymVal
{
  AlphaTab name;
  SymValKind kind;
  unsigned arg_idx;  /**< If a file.**/
  unsigned ios_idx;
  unsigned cmd_idx;
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
init_Command(Command* cmd, FildeshAlloc* alloc)
{
  cmd->kind = NCommandKinds;
  cmd->line_num = 0;
  InitTable( cmd->args );
  InitTable( cmd->tmp_files );
  cmd->pid = -1;
  cmd->stdis = -1;
  InitTable( cmd->is );
  cmd->stdos = -1;
  InitTable( cmd->os );
  cmd->status_fd = -1;
  InitTable( cmd->exit_fds );
  cmd->exec_fd = -1;
  cmd->exec_fd_has_bytes = false;
  cmd->exec_doc = NULL;
  InitTable( cmd->iargs );
  cmd->alloc = alloc;
}

  static void
close_Command (Command* cmd)
{
  unsigned i;
  if (cmd->stdis >= 0) {
    fildesh_compat_fd_close(cmd->stdis);
    cmd->stdis = -1;
  }
  if (cmd->stdos >= 0) {
    fildesh_compat_fd_close(cmd->stdos);
    cmd->stdos = -1;
  }
  for (i = 0; i < cmd->is.sz; ++i) {
    fildesh_compat_fd_close(cmd->is.s[i]);
  }
  LoseTable( cmd->is );
  InitTable( cmd->is );

  for (i = 0; i < cmd->os.sz; ++i) {
    fildesh_compat_fd_close(cmd->os.s[i]);
  }
  LoseTable( cmd->os );
  InitTable( cmd->os );

  if (cmd->status_fd >= 0) {
    fildesh_compat_fd_close(cmd->status_fd);
    cmd->status_fd = -1;
  }
  LoseTable( cmd->exit_fds );
  InitTable( cmd->exit_fds );

  cmd->exec_fd = -1;
  cmd->exec_fd_has_bytes = false;
  cmd->exec_doc = NULL;

  LoseTable( cmd->iargs );
  InitTable( cmd->iargs );
}

static
  fildesh_compat_fd_t*
build_fds_to_inherit_Command(Command* cmd)
{
  size_t i, off;
  fildesh_compat_fd_t* fds = (fildesh_compat_fd_t*)
    malloc(sizeof(fildesh_compat_fd_t) *
           (cmd->is.sz + cmd->os.sz + 1));

  off = 0;
  for (i = 0; i < cmd->is.sz; ++i) {
    fds[off++] = cmd->is.s[i];
  }
  LoseTable( cmd->is );
  InitTable( cmd->is );
  for (i = 0; i < cmd->os.sz; ++i) {
    fds[off++] = cmd->os.s[i];
  }
  LoseTable( cmd->os );
  InitTable( cmd->os );
  fds[off] = -1;
  return fds;
}

  static void
lose_Command (Command* cmd)
{
  unsigned i;
  close_Command (cmd);
  switch (cmd->kind) {
    case DefCommand:
    case RunCommand:
    case StdinCommand:
    case StdoutCommand:
      LoseTable( cmd->args );
      break;
    case HereDocCommand:
      break;
    default:
      break;
  }

  UFor( i, cmd->tmp_files.sz ) {
    fildesh_compat_file_rm(cmd->tmp_files.s[i]);
  }
  LoseTable( cmd->tmp_files );
  cmd->kind = NCommandKinds;
}

  static void
lose_Commands (void* arg)
{
  TableT(Command)* cmds = (TableT(Command)*) arg;
  unsigned i;
  for (i = 0; i < cmds->sz; ++i) {
    if (cmds->s[i].kind == RunCommand && cmds->s[i].pid > 0)
      fildesh_compat_sh_kill(cmds->s[i].pid);
    if (cmds->s[i].kind != NCommandKinds)
      lose_Command (&cmds->s[i]);
  }
  LoseTable (*cmds);
  free(cmds);
}

  static CommandHookup*
new_CommandHookup(FildeshAlloc* alloc)
{
  CommandHookup* cmd_hookup = fildesh_allocate(CommandHookup, 1, alloc);
  InitAssocia( AlphaTab, SymVal, cmd_hookup->map, cmp_AlphaTab );
  InitAssocia( AlphaTab, SymVal, cmd_hookup->add_map, cmp_AlphaTab );
  cmd_hookup->temporary_directory = NULL;
  cmd_hookup->tmpfile_count = 0;
  cmd_hookup->stdin_fd = 0;
  cmd_hookup->stdout_fd = 1;
  cmd_hookup->stderr_fd_opened = false;
  return cmd_hookup;
}

static void free_CommandHookup(CommandHookup* cmd_hookup, int* istat) {
  Assoc* assoc;
  Associa* map = &cmd_hookup->map;
  for (assoc = beg_Associa(map);
      assoc;
      assoc = next_Assoc(assoc))
  {
    SymVal* x = (SymVal*) val_of_Assoc(map, assoc);
    if (x->kind == ODescVal && *istat == 0) {
      fildesh_log_errorf("Dangling output stream! Symbol: %s", x->name.s);
      *istat = -1;
    }
    lose_SymVal (x);
  }
  lose_Associa(&cmd_hookup->map);
  lose_Associa(&cmd_hookup->add_map);
  /* Close stdio if they are still valid and have been changed.*/
  if (cmd_hookup->stdin_fd >= 0 && cmd_hookup->stdin_fd != 0) {
    fildesh_compat_fd_close(cmd_hookup->stdin_fd);
  }
  if (cmd_hookup->stdout_fd >= 0 && cmd_hookup->stdout_fd != 1) {
    fildesh_compat_fd_close(cmd_hookup->stdout_fd);
  }
  if (cmd_hookup->stderr_fd_opened) {
    fildesh_compat_fd_close(2);
  }
}

  static void
close_FildeshAlloc_generic(void* arg)
{ close_FildeshAlloc((FildeshAlloc*) arg); }

static char* lace_strdup(const char* s) {
  return fildesh_compat_string_duplicate(s);
}

static char* strdup_fd_Command(Command* cmd, fildesh_fd_t fd)
{
  char buf[FILDESH_INT_BASE10_SIZE_MAX];
  fildesh_encode_int_base10(buf, fd);
  return strdup_FildeshAlloc(cmd->alloc, buf);
}

static char* lace_fd_strdup(fildesh_fd_t fd) {
  char buf[FILDESH_INT_BASE10_SIZE_MAX];
  fildesh_encode_int_base10(buf, fd);
  return lace_strdup(buf);
}

static char* lace_fd_path_strdup(fildesh_fd_t fd) {
  char buf[FILDESH_FD_PATH_SIZE_MAX];
  fildesh_encode_fd_path(buf, fd);
  return lace_strdup(buf);
}


static sign_t pstrcmp_callback(const void* plhs, const void* prhs) {
  const char* lhs = *(const char* const*)plhs;
  const char* rhs = *(const char* const*)prhs;
  return strcmp(lhs, rhs);
}

static void init_strmap(Associa* map) {
  InitAssocia( char*, char*, *map, pstrcmp_callback );
}
static void lose_strmap(Associa* map) {
  lose_Associa(map);
}

static void ensure_strmap(Associa* map, char* k, char* v) {
  Assoc* assoc = ensure_Associa(map, &k);
  *(char**)val_of_Assoc(map, assoc) = v;
}

static char* lookup_strmap(Associa* map, const char* k) {
  Assoc* assoc = lookup_Associa(map, &k);
  if (!assoc) {return NULL;}
  return *(char**)val_of_Assoc(map, assoc);
}

  static SymVal*
lookup_SymVal (Associa* map, const char* s)
{
  AlphaTab ts = dflt1_AlphaTab(s);
  Assoc* assoc = lookup_Associa(map, &ts);
  if (assoc) {
    return (SymVal*) val_of_Assoc(map, assoc);
  }
  return NULL;
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

  static unsigned
count_ws (const char* s)
{
  return strspn (s, fildesh_compat_string_blank_bytes);
}
  static unsigned
count_non_ws (const char* s)
{
  return strcspn (s, fildesh_compat_string_blank_bytes);
}

  static void
perror_Command(const Command* cmd, const char* msg, const char* msg2)
{
  if (msg && msg2) {
    fildesh_log_errorf("Problem on line %u. %s: %s", cmd->line_num, msg, msg2);
  } else if (msg) {
    fildesh_log_errorf("Problem on line %u. %s.", cmd->line_num, msg);
  } else {
    fildesh_log_errorf("Problem on line %u.", cmd->line_num);
  }
}

static
  char*
parse_double_quoted_string(FildeshO* out, char* s, Associa* map)
{
  truncate_FildeshO(out);
  while (s[0] != '"') {
    if (s[0] == '\0') {
      fildesh_log_error("Unterminated double quote.");
      return NULL;
    }
    else if (s[0] == '\\') {
      switch(s[1]) {
        case 'n':
          putc_FildeshO(out, '\n');
          s = &s[2];
          break;
        case 't':
          putc_FildeshO(out, '\t');
          s = &s[2];
          break;
        case '\\':
        case '$':
          putc_FildeshO(out, s[1]);
          s = &s[2];
          break;
        case '\0':
          fildesh_log_error("Unterminated double quote.");
          return NULL;
        default:
          fildesh_log_error("Unrecognized escape charactor.");
          return NULL;
      }
    }
    else if (s[0] == '$') {
      char* name = &s[2];
      SymVal* sym;
      if (s[1] != '{') {
        fildesh_log_error("Please wrap varible name in curly braces when it is part of a string.");
        return NULL;
      }
      s = strchr(name, '}');
      if (!s) {
        fildesh_log_error("Missing closing curly brace.");
        return NULL;
      }
      s[0] = '\0';
      s = &s[1];
      sym = lookup_SymVal(map, name);
      if (!sym || sym->kind != HereDocVal) {
        fildesh_log_errorf("${%s} variable not known at parse time.", name);
        return NULL;
      }
      puts_FildeshO(out, sym->as.here_doc);
    }
    else {
      putc_FildeshO(out, s[0]);
      s = &s[1];
    }
  }
  return &s[1];
}

static
  int
sep_line(TableT(cstr)* args, char* s, Associa* map, FildeshAlloc* alloc, FildeshO* tmp_out)
{
  while (1) {
    s = &s[count_ws (s)];
    if (s[0] == '\0')  break;

    if (s[0] == '\'') {
      unsigned i;
      s = &s[1];
      PushTable( *args, s );
      i = strcspn (s, "'");
      if (s[i] == '\0') {
        fildesh_log_warning("Unterminated single quote.");
        s = NULL;
        break;
      }
      s = &s[i];
    }
    else if (s[0] == '"') {
      s = parse_double_quoted_string(tmp_out, &s[1], map);
      if (!s)  break;
      PushTable( *args, strdup_FildeshO(tmp_out, alloc) );
    }
    else if (pfxeq_cstr("$(getenv ", s)) {
      FildeshX in[1] = {DEFAULT_FildeshX};
      FildeshX slice;
      in->at = s;
      in->size = strlen(s);
      in->off = strlen("$(getenv ");
      while_chars_FildeshX(in, " ");
      slice = until_char_FildeshX(in, ')');
      if (slice.at) {
        const char* v;
        in->at[in->off++] = '\0';
        s = &in->at[in->off];
        v = getenv(slice.at);
        if (!v) {v = "";}
        PushTable( *args, strdup_FildeshAlloc(alloc, v) );
      }
      else {
        s = NULL;
        fildesh_log_error("Unterminated environment variable.");
        break;
      }
    }
    else if (s[0] == '$' && s[1] == '(') {
      unsigned i;
      PushTable( *args, s );
      s = &s[2];
      i = strcspn (s, ")");
      if (s[i] == '\0') {
        fildesh_log_warning("Unterminated variable.");
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
  return s ? 0 : -1;
}

static
  int
parse_file(
    TableT(Command)* cmds,
    FildeshX* in,
    const char* this_filename,
    Associa* map,
    FildeshAlloc* scope_alloc,
    FildeshAlloc* global_alloc,
    FildeshO* tmp_out)
{
  int istat = 0;
  size_t text_nlines = 0;
  while (istat == 0) {
    char* line;
    Command* cmd;
    line = fildesh_syntax_parse_line(in, &text_nlines, global_alloc, tmp_out);
    if (!line) {
      break;
    }
    cmd = Grow1Table( *cmds );
    init_Command(cmd, scope_alloc);
    cmd->line = line;
    cmd->line_num = text_nlines;

    if (line[0] == '$' && line[1] == '(' &&
        line[2] == 'H' && line[3] != 'F')
    {
      SymVal* sym;
      SymValKind sym_kind;

      cmd->kind = HereDocCommand;
      cmd->doc = fildesh_syntax_parse_here_doc(
          in, line, &text_nlines, global_alloc, tmp_out);

      sym_kind = parse_sym(cmd->line, false);
      assert(sym_kind == HereDocVal);
      sym = getf_SymVal(map, cmd->line);
      sym->kind = HereDocVal;
      sym->as.here_doc = cmd->doc;
    }
    else if (pfxeq_cstr ("$(<<", line))
    {
      char* filename = &line[4];
      FildeshX* src = NULL;

      filename = &filename[count_ws (filename)];
      filename[strcspn (filename, ")")] = '\0';

      src = open_sibling_FildeshXF(this_filename, filename);
      if (!src) {
        perror_Command(cmd, "Failed to include file", filename);
        istat = -1;
        break;
      }
      lose_Command (TopTable( *cmds ));
      MPopTable( *cmds, 1 );

      istat = parse_file(cmds, src, filename_FildeshXF(src),
                         map, scope_alloc, global_alloc, tmp_out);
      if (istat == 0 && cmds->sz > 0 &&
          cmds->s[cmds->sz-1].kind == BarrierCommand)
      {
        perror_Command(
            &cmds->s[cmds->sz-1],
            "No barrier allowed in included file", filename);
        istat = -1;
      }
      close_FildeshX(src);
    }
    else if (pfxeq_cstr ("$(>", line) ||
        pfxeq_cstr ("$(set", line))
    {
      char* begline;
      char* sym_name = line;
      char* concatenated_args = NULL;

      cmd->kind = RunCommand;

      sym_name = &sym_name[count_non_ws(sym_name)];
      sym_name = &sym_name[count_ws(sym_name)];
      begline = strchr(sym_name, ')');
      if (!begline) {
        perror_Command(cmd, "Unclosed paren in variable def.", 0);
        istat = -1;
        break;
      }

      begline[0] = '\0';
      begline = &begline[1];

      PushTable( cmd->args, (char*) "zec" );
      PushTable( cmd->args, (char*) "/" );
      istat = sep_line(&cmd->args, begline, map, scope_alloc, tmp_out);
      if (istat != 0) {
        break;
      }

      concatenated_args = fildesh_syntax_maybe_concatenate_args(
          (unsigned) cmd->args.sz-2,
          (const char* const*)(void*)&cmd->args.s[2],
          global_alloc);
      if (concatenated_args) {
        SymVal* sym = getf_SymVal(map, sym_name);
        sym->kind = HereDocVal;
        sym->as.here_doc = concatenated_args;

        cmd->kind = HereDocCommand;
        cmd->doc = concatenated_args;
        cmd->args.sz = 0;
        PackTable(cmd->args);
        continue;
      }
      PushTable( cmd->args, (char*) "/" );

      {
        char* buf = fildesh_allocate(char, 6+strlen(sym_name), scope_alloc);
        sprintf(buf, "$(O %s)", sym_name);
        PushTable( cmd->args, buf );
      }

      cmd = Grow1Table( *cmds );
      init_Command(cmd, scope_alloc);
      cmd->kind = DefCommand;
      cmd->line_num = text_nlines;
      PushTable( cmd->args, (char*) "elastic" );
      {
        char* buf = fildesh_allocate(char, 6+strlen(sym_name), scope_alloc);
        sprintf(buf, "$(X %s)", sym_name);
        PushTable( cmd->args, buf );
      }
      cmd->line = sym_name;
    }
    else
    {
      cmd->kind = RunCommand;
      istat = sep_line(&cmd->args, cmd->line, map, scope_alloc, tmp_out);
      if (istat == 0 && 0 == strcmp(cmd->args.s[0], "$(barrier)")) {
        if (cmd->args.sz != 1) {
          perror_Command(cmd, "Barrier does not accept args.", 0);
          istat = -1;
        }
        cmd->kind = BarrierCommand;
        cmd->args.sz = 0;
        PackTable( cmd->args );
        break;
      }
    }
  }
  return istat;
}

  SymValKind
parse_sym (char* s, bool firstarg)
{
  unsigned i, o;
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
    unsigned n = i-1;
    fildesh_log_warningf("For forward compatibility, please change %s to use the $(XA ...) syntax.", s);

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
    if (s[o+1] == '?') {
      kind = ODescStatusVal;
    }
    else if (s[o+1] == 'F') {
      if (s[o+2] == '^')  kind = OFutureDescFileVal;
      else                kind = ODescFileVal;
    }
    else {
      if (s[o+1] == '^')  kind = OFutureDescVal;
      else                kind = ODescVal;
    }
  }
  else if (s[o] == 'H')
  {
    if (s[o+1] == 'F') {
      fildesh_log_warning("For forward compatibility, please change $(HF ...) to the $(XF ...) syntax.");
      kind = IDescFileVal;
    }
    else {
      kind = HereDocVal;
    }
  }

  if (kind != NSymValKinds)
  {
    unsigned n;
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

  static unsigned
add_ios_Command (Command* cmd, int in, int out)
{
  unsigned idx = UINT_MAX;
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
  GrowTable( cmd->iargs, 1 );
  TopTable( cmd->iargs )->fd = in;
  TopTable( cmd->iargs )->scrap_newline = scrap_newline;
}

  static char*
add_fd_arg_Command (Command* cmd, int fd)
{
  char buf[FILDESH_FD_PATH_SIZE_MAX];
  fildesh_encode_fd_path(buf, fd);
  return strdup_FildeshAlloc(cmd->alloc, buf);
}

  static void
remove_tmppath(void* temporary_directory)
{
  if (0 != fildesh_compat_file_rmdir((char*)temporary_directory)) {
    fildesh_compat_errno_trace();
    fildesh_log_warningf("Temp directory not removed: %s",
                      (char*)temporary_directory);
  }
  free(temporary_directory);
  fildesh_log_trace("freed temporary_directory");
}

  static char*
add_tmp_file_Command(Command* cmd,
                     CommandHookup* cmd_hookup,
                     const char* extension)
{
  char buf[2048];
  assert(extension);
  if (!cmd_hookup->temporary_directory) {
    cmd_hookup->temporary_directory = mktmppath_sysCx("fildesh");
    fildesh_compat_errno_clear();
    if (!cmd_hookup->temporary_directory) {
      fildesh_log_error("Unable to create temp directory.");
      return NULL;
    }
    push_losefn_sysCx(remove_tmppath, cmd_hookup->temporary_directory);
  }

  sprintf(buf, "%s/%u%s", cmd_hookup->temporary_directory,
          cmd_hookup->tmpfile_count, extension);
  cmd_hookup->tmpfile_count += 1;
  PushTable( cmd->tmp_files, strdup_FildeshAlloc(cmd->alloc, buf) );
  return cmd->tmp_files.s[cmd->tmp_files.sz - 1];
}

static
  char*
write_heredoc_tmpfile(Command* cmd, CommandHookup* cmd_hookup, const char* doc)
{
  FildeshO* out;
  char* filename = add_tmp_file_Command(cmd, cmd_hookup, ".txt");
  if (!filename) {return NULL;}
  /* Write the temp file now.*/
  out = open_FildeshOF(filename);
  if (!out) {return NULL;}
  put_bytestring_FildeshO(out, (const unsigned char*)doc, strlen(doc));
  close_FildeshO(out);
  return filename;
}

static
  fildesh_fd_t
pipe_from_elastic(Command* elastic_cmd)
{
  fildesh_fd_t fd[2];
  if (0 != fildesh_compat_fd_pipe(&fd[1], &fd[0])) {
    return -1;
  }
  add_ios_Command(elastic_cmd, -1, fd[1]);
  PushTable( elastic_cmd->args, add_fd_arg_Command(elastic_cmd, fd[1]) );
  return fd[0];
}

static
  int
setup_commands(TableT(Command)* cmds, CommandHookup* cmd_hookup)
{
  Associa* map = &cmd_hookup->map;
  /* Temporarily hold new symbols for the current line.*/
  Associa* add_map = &cmd_hookup->add_map;
  Assoc* assoc;
  int istat = 0;
  unsigned i;

#define FailBreak(cmd, msg, arg) { \
  perror_Command(cmd, msg, arg); \
  istat = -1; \
  break; \
}

  for (i = 0; i < cmds->sz && istat == 0; ++i) {
    unsigned arg_q = 0;
    unsigned arg_r = 0;
    Command* cmd = &cmds->s[i];

    /* The command defines a HERE document.*/
    if (cmd->kind == HereDocCommand)
    {
      /* Command symbol was parsed during parsing
       * but we need to overwrite the symbol
       * just in case there are multiple occurrences.
       */
      SymVal* sym = getf_SymVal(map, cmd->line);
      sym->kind = HereDocVal;
      sym->as.here_doc = cmd->doc;

      /* The loops should not run.*/
      assert( cmd->args.sz == 0 ); /* Invariant.*/
    }

    for (arg_r = 0; arg_r < cmd->args.sz && istat == 0; ++ arg_r) {
      char* arg = cmd->args.s[arg_r];
      if (arg_q == 0 && eq_cstr("stdin", arg)) {
        cmd->kind = StdinCommand;
        cmd->stdis = cmd_hookup->stdin_fd;
        cmd_hookup->stdin_fd = -1;
        if (cmd->stdis < 0) {
          FailBreak(cmd, "Cannot have multiple stdin commands", arg);
        }
        break;
      }
      else if (arg_q == 0 && eq_cstr("stdout", arg)) {
        cmd->kind = StdoutCommand;
        cmd->stdos = cmd_hookup->stdout_fd;
        cmd_hookup->stdout_fd = -1;
        if (cmd->stdos < 0) {
          FailBreak(cmd, "Cannot have multiple stdout commands", arg);
        }
        break;
      }
    }

    for (arg_r = 0; arg_r < cmd->args.sz && istat == 0; ++ arg_r)
    {
      char* arg = cmd->args.s[arg_r];
      const SymValKind kind = parse_sym (arg, (arg_r == 0));


      if (arg_q == 0 && (kind == ODescFileVal || kind == OFutureDescFileVal)) {
        FailBreak(cmd, "Cannot execute file that this command intends to write",
                  arg);
      }
      else if (arg_q == 0 && kind == IFutureDescFileVal) {
        FailBreak(cmd, "Executable bytes cannot come from below", arg);
      }
      else if (kind == HereDocVal || kind == IDescArgVal)
      {
        SymVal* sym = getf_SymVal (map, arg);
        if (sym->kind == HereDocVal) {
          cmd->args.s[arg_q] = sym->as.here_doc;
        }
        else if (sym->kind == ODescVal) {
          fildesh_fd_t fd = sym->as.file_desc;
          sym->kind = NSymValKinds;
          add_iarg_Command (cmd, fd, true);
          cmd->args.s[arg_q] = NULL;
        }
        else if (sym->kind == DefVal) {
          fildesh_fd_t fd = pipe_from_elastic(&cmds->s[sym->cmd_idx]);
          if (fd < 0) {
            FailBreak(cmd, "Failed to create pipe for variable", arg);
          }
          add_iarg_Command (cmd, fd, true);
          cmd->args.s[arg_q] = NULL;
        }
        else {
          FailBreak(cmd, "Unknown source for argument", arg);
        }
        ++ arg_q;
      }
      else if (cmd->kind == StdoutCommand && kind == IDescVal)
      {
        SymVal* sym = getf_SymVal (map, arg);
        Command* last = &cmds->s[sym->cmd_idx];

        if (last->kind != RunCommand) {
          FailBreak(cmd, "Stdout stream not coming from a command?", arg);
        }

        fildesh_compat_fd_close(sym->as.file_desc);

        if (sym->ios_idx < last->os.sz) {
          fildesh_compat_fd_move_to(last->os.s[sym->ios_idx], cmd->stdos);
        }
        else {
          fildesh_compat_fd_move_to(last->stdos, cmd->stdos);
        }
        sym->kind = NSymValKinds;
        cmd->stdos = -1;
      }
      else if (kind == IDescVal ||
               kind == IDescFileVal ||
               kind == IODescVal)
      {
        SymVal* sym = getf_SymVal (map, arg);
        int fd = sym->as.file_desc;
        if (sym->kind == HereDocVal) {
          /* Do nothing.*/
        }
        else if (sym->kind == ODescVal) {
          sym->kind = NSymValKinds;
        }
        else if (sym->kind == DefVal) {
          fd = pipe_from_elastic(&cmds->s[sym->cmd_idx]);
        }
        else {
          FailBreak(cmd, "Unknown source for", arg);
        }

        if (sym->kind == HereDocVal) {
          char* filename = write_heredoc_tmpfile(
              cmd, cmd_hookup, sym->as.here_doc);
          if (!filename) {
            FailBreak(cmd, "Cannot create tmpfile for heredoc.", NULL);
          }

          assert(kind != IODescVal);
          if (kind == IDescVal) {
            fd = fildesh_arg_open_readonly(filename);
            cmd->stdis = fd;
          }
          else {
            cmd->args.s[arg_q] = filename;
            if (arg_q == 0)
              cmd->exec_doc = sym->as.here_doc;
            ++ arg_q;
          }
        }
        else if (kind == IDescVal || kind == IODescVal) {
          cmd->stdis = fd;
        }
        else if (kind == IDescFileVal) {
          add_ios_Command(cmd, fd, -1);
          if (arg_q > 0) {
            cmd->args.s[arg_q] = add_fd_arg_Command (cmd, fd);
          } else {
            cmd->args.s[0] = add_tmp_file_Command(cmd, cmd_hookup, ".exe");
            if (!cmd->args.s[0]) {
              FailBreak(cmd, "Cannot create tmpfile for executable.", NULL);
            }
            if (sym->arg_idx < UINT_MAX) {
              static const char dev_fd_prefix[] = "/dev/fd/";
              static const unsigned dev_fd_prefix_length =
                sizeof(dev_fd_prefix)-1;
              Command* src_cmd = &cmds->s[sym->cmd_idx];
              char* src_fd_filename = src_cmd->args.s[sym->arg_idx];
              int src_fd = -1;
              assert(0 == memcmp(src_fd_filename, dev_fd_prefix,
                                 dev_fd_prefix_length));
              fildesh_parse_int(&src_fd,
                                &src_fd_filename[dev_fd_prefix_length]);
              assert(src_fd >= 0);

              PushTable(src_cmd->exit_fds, src_fd);
              /* Replace fd path with tmpfile name.*/
              src_cmd->args.s[sym->arg_idx] = cmd->args.s[0];
            } else {
              /* Source command isn't directly creating file.*/
              cmd->exec_fd_has_bytes = true;
            }
            cmd->exec_fd = fd;
          }
          ++ arg_q;
        }
        else {
          FailBreak(cmd, "Unexpected kind.", NULL);
        }
      }
      else if (kind == OFutureDescVal || kind == OFutureDescFileVal)
      {
        SymVal* sym = getf_SymVal (map, arg);
        int fd;

        if (sym->kind != IFutureDescVal) {
          FailBreak(cmd, "Argument should be a stream to the past", arg);
        }

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
            s = add_tmp_file_Command(&cmds->s[sym->cmd_idx],
                cmd_hookup, ".exe");
            if (!s) {
              FailBreak(cmd, "Cannot create tmpfile for executable.", NULL);
            }
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
        sym->as.file_desc = cmd->stdis;
        cmd->stdis = -1;
        InitDomMax( sym->arg_idx );
        InitDomMax( sym->ios_idx );
      }
      else if (kind == ODescFileVal && eq_cstr (arg, "VOID")) {
        cmd->args.s[arg_q] = strdup_FildeshAlloc(cmd->alloc, "/dev/null");
        arg_q += 1;
      } else if (kind == ODescVal || kind == IODescVal ||
                 kind == ODescStatusVal ||
                 kind == ODescFileVal)
      {
        fildesh_fd_t fd[2];
        SymVal* sym = getf_SymVal (add_map, arg);
        sym->kind = ODescVal;
        sym->cmd_idx = i;
        InitDomMax( sym->arg_idx );
        InitDomMax( sym->ios_idx );

        if (0 != fildesh_compat_fd_pipe(&fd[1], &fd[0])) {
          FailBreak(cmd, "Failed to create pipe for variable", arg);
        }
        assert(fd[0] >= 0);
        assert(fd[1] >= 0);

        sym->as.file_desc = fd[0];

        if (kind == ODescVal || kind == IODescVal) {
          cmd->stdos = fd[1];
        } else if (kind == ODescStatusVal) {
          cmd->status_fd = fd[1];
        } else {
          sym->ios_idx = add_ios_Command (cmd, -1, fd[1]);
          cmd->args.s[arg_q] = add_fd_arg_Command (cmd, fd[1]);
          sym->arg_idx = arg_q;
          ++ arg_q;
        }
      }
      else if (kind == IFutureDescVal || kind == IFutureDescFileVal)
      {
        fildesh_fd_t fd[2];
        SymVal* sym = getf_SymVal (add_map, arg);
        sym->kind = IFutureDescVal;
        sym->cmd_idx = i;
        InitDomMax( sym->arg_idx );
        InitDomMax( sym->ios_idx );

        if (0 != fildesh_compat_fd_pipe(&fd[1], &fd[0])) {
          FailBreak(cmd, "Failed to create pipe for variable", arg);
        }

        sym->as.file_desc = fd[1];
        if (kind == IFutureDescVal)
        {
          cmd->stdis = fd[0];
        }
        else
        {
          assert(arg_q > 0 && "executable bytes cannot come from below");
          sym->ios_idx = add_ios_Command (cmd, -1, fd[0]);
          cmd->args.s[arg_q] = add_fd_arg_Command (cmd, fd[0]);
          ++ arg_q;
        }
      }

      if (kind == NSymValKinds) {
        cmd->args.s[arg_q] = arg;
        ++ arg_q;
      }
    }

    /* This guard might not be necessary.*/
    if (istat != 0) {break;}

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
        FailBreak(cmd, "Trying to overwrite an existing stream variable",
            ccstr_of_AlphaTab (add_key));
      }

      *sym = *add_sym;
      assoc = next_Assoc (assoc);
      give_Associa (add_map, tmp_assoc);
    } while (assoc);
  }
#undef FailBreak
  return istat;
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
show_usage()
{
  const char fildesh_exe[] = "fildesh";
  fprintf(stderr, "Usage: %s [[-f] SCRIPTFILE | -- SCRIPT]\n",
          fildesh_exe);
}


#ifdef FILDESH_BUILTIN_PERMIT_ELASTIC_AIO
int main_elastic_aio(unsigned argc, char** argv);
#endif
#ifdef FILDESH_BUILTIN_PERMIT_ELASTIC_POLL
int main_elastic_poll(unsigned argc, char** argv);
#endif
int main_godo(unsigned argc, char** argv);
int main_ssh_all(unsigned argc, char** argv);
int main_waitdo(unsigned argc, char** argv);
int main_xpipe(unsigned argc, char** argv);

static int fildesh_main_add(unsigned argc, char** argv) {
  return fildesh_builtin_add_main(argc, argv, NULL, NULL);
}
static int fildesh_main_bestmatch(unsigned argc, char** argv) {
  return fildesh_builtin_bestmatch_main(argc, argv, NULL, NULL);
}
static int fildesh_main_cmp(unsigned argc, char** argv) {
  return fildesh_builtin_cmp_main(argc, argv, NULL, NULL);
}
static int fildesh_main_elastic_pthread(unsigned argc, char** argv) {
  return fildesh_builtin_elastic_pthread_main(argc, argv, NULL, NULL);
}
static int main_execfd(unsigned argc, char** argv) {
  return fildesh_builtin_execfd_main(argc, argv, NULL, NULL);
}
static int main_expect_failure(unsigned argc, char** argv) {
  return fildesh_builtin_expect_failure_main(argc, argv, NULL, NULL);
}
static int main_fildesh(unsigned argc, char** argv) {
  return fildesh_builtin_fildesh_main(argc, argv, NULL, NULL);
}
static int fildesh_main_replace_string(unsigned argc, char** argv) {
  return fildesh_builtin_replace_string_main(argc, argv, NULL, NULL);
}
static int fildesh_main_seq(unsigned argc, char** argv) {
  return fildesh_builtin_seq_main(argc, argv, NULL, NULL);
}
static int fildesh_main_sponge(unsigned argc, char** argv) {
  return fildesh_builtin_sponge_main(argc, argv, NULL, NULL);
}
static int fildesh_main_time2sec(unsigned argc, char** argv) {
  return fildesh_builtin_time2sec_main(argc, argv, NULL, NULL);
}
static int fildesh_main_transpose(unsigned argc, char** argv) {
  return fildesh_builtin_transpose_main(argc, argv, NULL, NULL);
}
static int fildesh_main_ujoin(unsigned argc, char** argv) {
  return fildesh_builtin_ujoin_main(argc, argv, NULL, NULL);
}
static int fildesh_main_void(unsigned argc, char** argv) {
  return fildesh_builtin_void_main(argc, argv, NULL, NULL);
}
static int fildesh_main_zec(unsigned argc, char** argv) {
  return fildesh_builtin_zec_main(argc, argv, NULL, NULL);
}

static int fildesh_main_elastic(unsigned argc, char** argv) {
#if defined(FILDESH_BUILTIN_PERMIT_ELASTIC_AIO)
  return main_elastic_aio(argc, argv);
#elif defined(FILDESH_BUILTIN_PERMIT_ELASTIC_POLL)
  return main_elastic_poll(argc, argv);
#else
  return fildesh_main_elastic_pthread(argc, argv);
#endif
}

bool fildesh_builtin_is_threadsafe(const char* name)
{
  static const char* const all[] = {
    "add",
    "bestmatch",
    "cmp",
#if !defined(FILDESH_BUILTIN_PERMIT_ELASTIC_AIO) && !defined(FILDESH_BUILTIN_PERMIT_ELASTIC_POLL)
    "elastic",
#endif
    "elastic_pthread",
    "execfd",
    "expect_failure",
    "replace_string",
    "seq",
    "sponge",
    "time2sec",
    "transpose",
    "ujoin",
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

int (*fildesh_specific_util (const char* arg)) (unsigned, char**)
{
  typedef struct FildeshBuiltinMap FildeshBuiltinMap;
  struct FildeshBuiltinMap {
    const char* name;
    int (*main_fn)(unsigned,char**);
  };
  static const FildeshBuiltinMap builtins[] = {
    {"add", fildesh_main_add},
    {"best-match", fildesh_main_bestmatch},
    {"bestmatch", fildesh_main_bestmatch},
    {"cmp", fildesh_main_cmp},
    {"elastic", fildesh_main_elastic},
    {"elastic_pthread", fildesh_main_elastic_pthread},
#ifdef FILDESH_BUILTIN_PERMIT_ELASTIC_AIO
    {"elastic_aio", main_elastic_aio},
#endif
#ifdef FILDESH_BUILTIN_PERMIT_ELASTIC_POLL
    {"elastic_poll", main_elastic_poll},
#endif
    {"execfd", main_execfd},
    {"expect_failure", main_expect_failure},
    {"fildesh", main_fildesh},
    {"godo", main_godo},
    {"replace_string", fildesh_main_replace_string},
    {"seq", fildesh_main_seq},
    {"sponge", fildesh_main_sponge},
    {"ssh-all", main_ssh_all},
    {"time2sec", fildesh_main_time2sec},
    {"transpose", fildesh_main_transpose},
    {"ujoin", fildesh_main_ujoin},
    {"void", fildesh_main_void},
    {"waitdo", main_waitdo},
    {"xpipe", main_xpipe},
    {"zec", fildesh_main_zec},
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

int fildesh_builtin_main(const char* name, unsigned argc, char** argv)
{
  int (*f) (unsigned, char**);
  f = fildesh_specific_util(name);
  if (!f) {
    fildesh_log_errorf("Unknown builtin: %s", name);
    return -1;
  }
  return f(argc, argv);
}

FILDESH_POSIX_THREAD_CALLBACK(builtin_command_thread_fn, BuiltinCommandThreadArg*, st)
{
  typedef struct FildeshBuiltinMainMap FildeshBuiltinMainMap;
  struct FildeshBuiltinMainMap {
    const char* name;
    int (*main_fn)(unsigned, char**, FildeshX**, FildeshO**);
  };
  static const FildeshBuiltinMainMap builtins[] = {
    {"add", fildesh_builtin_add_main},
    {"bestmatch", fildesh_builtin_bestmatch_main},
    {"cmp", fildesh_builtin_cmp_main},
#if !defined(FILDESH_BUILTIN_PERMIT_ELASTIC_AIO) && !defined(FILDESH_BUILTIN_PERMIT_ELASTIC_POLL)
    {"elastic", fildesh_builtin_elastic_pthread_main},
#endif
    {"elastic_pthread", fildesh_builtin_elastic_pthread_main},
    {"execfd", fildesh_builtin_execfd_main},
    {"expect_failure", fildesh_builtin_expect_failure_main},
    {"replace_string", fildesh_builtin_replace_string_main},
    {"seq", fildesh_builtin_seq_main},
    {"sponge", fildesh_builtin_sponge_main},
    {"time2sec", fildesh_builtin_time2sec_main},
    {"transpose", fildesh_builtin_transpose_main},
    {"ujoin", fildesh_builtin_ujoin_main},
    {"void", fildesh_builtin_void_main},
    {"zec", fildesh_builtin_zec_main},
    {NULL, NULL},
  };
  Command* cmd = st->command;
  unsigned offset = 0;
  unsigned argc;
  char** argv;
  FildeshX** inputs = NULL;
  FildeshO** outputs = NULL;
  int (*main_fn)(unsigned, char**, FildeshX**, FildeshO**) = NULL;
  unsigned i;
  char* name = NULL;
  bool only_argv = false;

  assert(cmd->iargs.sz == 0);
  assert(cmd->exec_fd < 0);

  for (argc = 0; st->argv[argc]; ++argc) {
    if (offset == 0 && 0 == strcmp("-as", st->argv[argc])) {
      offset = argc + 1;
    }
  }

  /* We should have found something. Retain argv[0].*/
  assert(offset >= 2);
  name = st->argv[offset];
  st->argv[offset] = st->argv[offset-2];
  st->argv[offset-2] = name;

  assert(fildesh_builtin_is_threadsafe(name));
  for (i = 0; builtins[i].name; ++i) {
    if (0 == strcmp(name, builtins[i].name)) {
      main_fn = builtins[i].main_fn;
      break;
    }
  }
  assert(main_fn);

  argc -= offset;
  argv = &st->argv[offset];
  only_argv = (0 == strcmp("execfd", name));
  inputs = (FildeshX**) malloc(sizeof(FildeshX*) * (argc+1));
  outputs = (FildeshO**) malloc(sizeof(FildeshO*) * (argc+1));
  for (i = 0; i <= argc; ++i) {
    inputs[i] = NULL;
    outputs[i] = NULL;
  }
  if (cmd->stdis >= 0) {
    inputs[0] = open_fd_FildeshX(cmd->stdis);
    cmd->stdis = -1;
  }
  if (cmd->stdos >= 0) {
    outputs[0] = open_fd_FildeshO(cmd->stdos);
    cmd->stdos = -1;
  }

  if (false) {
    for (i = 0; i < argc; ++i) {
      fildesh_log_tracef("%u argv[%u]: %s", cmd->line_num, i, argv[i]);
    }
  }

  if (main_fn && !only_argv) {
    cmd->status = main_fn(argc, argv, inputs, outputs);
  } else if (main_fn && only_argv) {
    cmd->status = main_fn(argc, argv, NULL, NULL);
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
}

static
  void
fix_known_flags_Command(Command* cmd, Associa* alias_map) {
  char* replacement = lookup_strmap(alias_map, cmd->args.s[0]);
  if (replacement) {
    cmd->args.s[0] = replacement;
  }

  if (eq_cstr("sed", cmd->args.s[0])) {
    unsigned i;
    for (i = 1; i < cmd->args.sz; ++i) {
      const char* arg = cmd->args.s[i];
      if (eq_cstr("--line-buffered", arg)) {
#ifdef __APPLE__
        const char line_buffering_flag[] = "-l";
#else
        const char line_buffering_flag[] = "-u";
#endif
        cmd->args.s[i] = strdup_FildeshAlloc(cmd->alloc, line_buffering_flag);
      }
    }
  }
  else if (eq_cstr("tr", cmd->args.s[0])) {
    if (cmd->args.sz == 3 &&
        1 == strlen(cmd->args.s[1]) &&
        1 == strlen(cmd->args.s[2])) {
      cmd->args.s[0] = strdup_FildeshAlloc(cmd->alloc, "replace_string");
    }
  }
}

static
  void
add_inheritfd_flags_Command(TableT(cstr)* argv, Command* cmd, bool inprocess) {
  unsigned i;

  if (inprocess) {
    /* Let command inherit all input file descriptors except:
     * - The fd providing executable bytes (or signaling that they are ready).
     * - The fds providing input arguments. See inprocess else clause.
     */
    for (i = 0; i < cmd->is.sz; ++i) {
      const fildesh_fd_t fd = cmd->is.s[i];
      if (fd != cmd->exec_fd) {
        PushTable( *argv, lace_strdup("-inheritfd") );
        PushTable( *argv, lace_fd_strdup(fd) );
      }
    }
    /* Let command inherit all output file descriptors except:
     * - The fds that must be closed on exit.
     */
    for (i = 0; i < cmd->os.sz; ++i) {
      const fildesh_fd_t fd = cmd->os.s[i];
      bool inherit = true;
      unsigned j;
      for (j = 0; j < cmd->exit_fds.sz && inherit; ++j) {
        inherit = (fd != cmd->exit_fds.s[j]);
      }
      if (inherit) {
        PushTable( *argv, lace_strdup("-inheritfd") );
        PushTable( *argv, lace_fd_strdup(fd) );
      }
    }

    if (cmd->stdis >= 0) {
      PushTable( *argv, lace_strdup("-stdin") );
      PushTable( *argv, lace_fd_path_strdup(cmd->stdis) );
      PushTable( cmd->is, cmd->stdis );
    }
    cmd->stdis = -1;
    if (cmd->stdos >= 0) {
      PushTable( *argv, lace_strdup("-stdout") );
      PushTable( *argv, lace_fd_path_strdup(cmd->stdos) );
      PushTable( cmd->os, cmd->stdos );
    }
    cmd->stdos = -1;
  } else {
    for (i = 0; i < cmd->iargs.sz; ++i) {
      add_ios_Command(cmd, cmd->iargs.s[i].fd, -1);
    }
  }

  LoseTable(cmd->iargs);
  InitTable(cmd->iargs);

  if (cmd->exec_fd >= 0) {
    if (!cmd->exec_fd_has_bytes) {
      PushTable( *argv, lace_strdup("-waitfd") );
      PushTable( *argv, lace_fd_strdup(cmd->exec_fd) );
    }
    add_ios_Command(cmd, cmd->exec_fd, -1);
  }
  cmd->exec_fd = -1;

  for (i = 0; i < cmd->exit_fds.sz; ++i) {
    PushTable( *argv, lace_strdup("-exitfd") );
    PushTable( *argv, lace_fd_strdup(cmd->exit_fds.s[i]) );
  }
  LoseTable(cmd->exit_fds);
  InitTable(cmd->exit_fds);

  if (cmd->status_fd >= 0) {
    PushTable( *argv, lace_strdup("-o?") );
    PushTable( *argv, lace_fd_path_strdup(cmd->status_fd) );
    PushTable( cmd->os, cmd->status_fd );
  }
  cmd->status_fd = -1;
}

  static int
spawn_commands(const char* fildesh_exe, TableT(Command) cmds,
               Associa* alias_map, bool forkonly)
{
  DeclTable( cstr, argv );
  DeclTable( uint2, fdargs );
  unsigned i;
  int istat = 0;

  for (i = 0; i < cmds.sz && istat == 0; ++i)
  {
    Command* cmd = &cmds.s[i];
    bool use_thread = false;
    unsigned argi, j;

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

    fix_known_flags_Command(cmd, alias_map);

    if (cmd->exec_fd >= 0 || fdargs.sz > 0 ||
        cmd->status_fd >=0 || cmd->exit_fds.sz > 0)
    {
      const char* execfd_exe = lookup_strmap(alias_map, "execfd");
      char* execfd_fmt = NULL;
      if (execfd_exe) {
        PushTable( argv, lace_strdup(execfd_exe) );
      } else {
        if (!forkonly) {
          use_thread = true;
        }
        PushTable( argv, lace_strdup(fildesh_exe) );
        PushTable( argv, lace_strdup("-as") );
        PushTable( argv, lace_strdup("execfd") );
      }
      if (cmd->exec_fd >= 0) {
        PushTable( argv, lace_strdup("-exe") );
        PushTable( argv, lace_strdup(cmd->args.s[0]) );
        if (cmd->exec_fd_has_bytes) {
          uint2 p;
          p.s[0] = 0;
          p.s[1] = cmd->exec_fd;
          PushTable( fdargs, p );
        }
      }

      add_inheritfd_flags_Command(&argv, cmd, use_thread);

      execfd_fmt = (char*)malloc(2*cmd->args.sz);
      for (j = 0; j < cmd->args.sz; ++j) {
        execfd_fmt[2*j] = 'a';
        execfd_fmt[2*j+1] = '_';
      }
      execfd_fmt[2*cmd->args.sz-1] = '\0';

      for (j = 0; j < fdargs.sz; ++j) {
        uint2 p = fdargs.s[j];
        cmd->args.s[p.s[0]] = strdup_fd_Command(cmd, p.s[1]);
        execfd_fmt[2*p.s[0]] = 'x';
      }

      PushTable( argv, execfd_fmt );
      PushTable( argv, lace_strdup("--") );
      PushTable( argv, lace_strdup(cmd->args.s[0]) );
    }
    else if (fildesh_specific_util (cmd->args.s[0])) {
      if (!forkonly) {
        use_thread = fildesh_builtin_is_threadsafe(cmd->args.s[0]);
      }
      PushTable( argv, lace_strdup(fildesh_exe) );
      PushTable( argv, lace_strdup("-as") );
      PushTable( argv, lace_strdup(cmd->args.s[0]));
    }
    else {
      PushTable( argv, lace_strdup(cmd->args.s[0]));
    }

    for (j = 1; j < cmd->args.sz; ++j)
      PushTable( argv, lace_strdup(cmd->args.s[j]) );

    PushTable( argv, NULL );

    if (cmd->exec_doc)
    {
      cmd->exec_doc = 0;
      fildesh_compat_file_chmod_u_rwx(cmd->args.s[0], 1, 1, 1);
    }

    if (use_thread) {
      BuiltinCommandThreadArg* arg = AllocT(BuiltinCommandThreadArg, 1);
      arg->command = cmd;
      arg->argv = DupliT(char*, argv.s, argv.sz);
      cmd->pid = 0;
      istat = pthread_create(
          &cmd->thread, NULL, builtin_command_thread_fn, arg);
      if (istat < 0) {
        fildesh_log_errorf("Could not pthread_create(). File: %s", argv.s[0]);
      }
    } else {
      fildesh_compat_fd_t* fds_to_inherit =
        build_fds_to_inherit_Command(cmd);
      cmd->pid = fildesh_compat_fd_spawnvp(
          cmd->stdis, cmd->stdos, 2, fds_to_inherit, (const char**)argv.s);
      free(fds_to_inherit);
      cmd->stdis = -1;
      cmd->stdos = -1;
      close_Command(cmd);
      if (cmd->pid < 0) {
        fildesh_log_errorf("Could not spawnvp(). File: %s", argv.s[0]);
        istat = -1;
      }
      for (argi = 0; argi < argv.sz; ++argi)
        free (argv.s[argi]);
    }
    fdargs.sz = 0;
    argv.sz = 0;
  }
  LoseTable( argv );
  LoseTable( fdargs );
  return istat;
}


  int
fildesh_builtin_fildesh_main(unsigned argc, char** argv,
                             FildeshX** inputv, FildeshO** outputv)
{
  char* fildesh_exe = argv[0];
  DeclTable( AlphaTab, script_args );
  TableT(Command)* cmds = NULL;
  bool use_stdin = true;
  FildeshX* script_in = NULL;
  FildeshO tmp_out[1] = {DEFAULT_FildeshO};
  FildeshAlloc* global_alloc;
  CommandHookup* cmd_hookup;
  unsigned argi = 1;
  unsigned i;
  Associa alias_map[1];
  bool forkonly = false;
  bool exiting = false;
  int exstatus = 0;
  int istat;

  /* We shouldn't have an error yet.*/
  fildesh_compat_errno_clear();

  /* With all the signal handling below,
   * primarily to clean up the temp directory,
   * it's not clear how to act as a proper builtin.
   */
  assert(!inputv);
  assert(!outputv);

  init_strmap(alias_map);
  global_alloc = open_FildeshAlloc();
  cmd_hookup = new_CommandHookup(global_alloc);

  /* add_util_path_env (); */
  (void) add_util_path_env;

  signal(SIGINT, lose_sysCx);
  signal(SIGSEGV, lose_sysCx);
#ifndef _MSC_VER
  signal(SIGQUIT, lose_sysCx);
  /* We already detect closed pipes when write() returns <= 0.*/
  signal(SIGPIPE, SIG_IGN);
#endif

  while (argi < argc && exstatus == 0 && !exiting) {
    const char* arg;
    arg = argv[argi++];
    if (eq_cstr (arg, "--")) {
      use_stdin = false;
      if (argi >= argc) {
        show_usage();
        exstatus = 64;
        break;
      }
      if (!script_in) {
        script_in = open_FildeshXA();
      }
      while (argi < argc) {
        size_t sz;
        arg = argv[argi++];
        sz = strlen(arg);
        memcpy(grow_FildeshX(script_in, sz), arg, sz);
        *grow_FildeshX(script_in, 1) = '\n';
      }
    }
    else if (eq_cstr (arg, "-as")) {
      const char* builtin_name = argv[argi];
      argv[argi] = fildesh_exe;
      exstatus = fildesh_builtin_main(builtin_name, argc-argi, &argv[argi]);
      exiting = true;
    }
    else if (eq_cstr(arg, "-alias")) {
      char* k = argv[argi++];
      char* v = NULL;
     if (k) {
       v = strchr(k, '=');
     }
     if (k && v) {
       v[0] = '\0';
       v = &v[1];
       ensure_strmap(alias_map, k, v);
       if (eq_cstr(k, "fildesh")) {
         fildesh_exe = v;
       }
     } else {
        fildesh_log_errorf("Failed alias: %s", k);
        exstatus = 64;
      }
    }
    else if (eq_cstr(arg, "-a")) {
      char* k = argv[argi++];
      char* v = NULL;
     if (k) {
       v = strchr(k, '=');
     }
     if (k && v) {
       SymVal* sym;
       v[0] = '\0';
       v = &v[1];
       sym = getf_SymVal(&cmd_hookup->map, k);
       sym->kind = HereDocVal;
       sym->as.here_doc = v;
     } else {
        fildesh_log_errorf("Bad -a arg: %s", k);
        exstatus = 64;
      }
    }
    else if (eq_cstr(arg, "-setenv")) {
      char* k = argv[argi++];
      char* v = NULL;
     if (k) {
       v = strchr(k, '=');
     }
     if (k && v) {
       v[0] = '\0';
       v = &v[1];
       istat = fildesh_compat_sh_setenv(k, v);
       if (istat != 0) {
         fildesh_log_error("Can't -setenv!");
         exstatus = 71;
       }
     } else {
        fildesh_log_errorf("Bad -setenv arg: %s", k);
        exstatus = 64;
      }
    }
    else if (eq_cstr(arg, "-forkonly")) {
      forkonly = true;
    }
    else if (eq_cstr (arg, "-stdin")) {
      const char* stdin_filepath = argv[argi++];
      fildesh_fd_t fd = fildesh_arg_open_readonly(stdin_filepath);
      if (fd >= 0) {
        istat = fildesh_compat_fd_move_to(cmd_hookup->stdin_fd, fd);
        if (istat != 0) {
          fildesh_log_error("Failed to dup2 -stdin.");
          exstatus = 72;
        }
      } else {
        fildesh_log_errorf("Failed to open stdin: %s", stdin_filepath);
        exstatus = 66;
      }
    }
    else if (eq_cstr (arg, "-stdout")) {
      const char* stdout_filepath = argv[argi++];
      fildesh_fd_t fd = fildesh_arg_open_writeonly(stdout_filepath);
      if (fd >= 0) {
        istat = fildesh_compat_fd_move_to(cmd_hookup->stdout_fd, fd);
        if (istat != 0) {
          fildesh_log_error("Failed to dup2 -stdout.");
          exstatus = 72;
        }
      } else {
        fildesh_log_errorf("Failed to open stdout: %s", stdout_filepath);
        exstatus = 73;
      }
    }
    else if (eq_cstr (arg, "-stderr")) {
      const char* stderr_filepath = argv[argi++];
      fildesh_fd_t fd = fildesh_arg_open_writeonly(stderr_filepath);
      if (fd >= 0) {
        istat = fildesh_compat_fd_move_to(2, fd);
        if (istat == 0) {
          cmd_hookup->stderr_fd_opened = true;
        }
        else {
          fildesh_log_error("Failed to dup2 -stderr.");
          exstatus = 72;
        }
      } else {
        fildesh_log_errorf("Failed to open stderr: %s", stderr_filepath);
        exstatus = 73;
      }
    }
    else {
      /* Optional -f flag.*/
      if (eq_cstr (arg, "-x") || eq_cstr (arg, "-f")) {
        if (argi >= argc) {
          show_usage();
          exstatus = 64;
          break;
        }
        arg = argv[argi++];
      }
      use_stdin = false;
      if (!arg) {
        PushTable( script_args, cons1_AlphaTab("/dev/fd/something") );
        script_in = open_arg_FildeshXF(argi-1, argv, inputv);
        if (!script_in) {
          fildesh_log_errorf("Cannot read script from builtin.");
          exstatus = 66;
        }
        break;
      }
      PushTable( script_args, cons1_AlphaTab(arg) );
      script_in = open_arg_FildeshXF(argi-1, argv, inputv);
      if (!script_in) {
        fildesh_compat_errno_trace();
        fildesh_log_errorf("Cannot read script. File: %s", arg);
        exstatus = 66;
      }
      break;
    }
  }

  if (exiting || exstatus != 0) {
    LoseTable( script_args );
    lose_strmap(alias_map);
    free_CommandHookup(cmd_hookup, &istat);
    close_FildeshAlloc(global_alloc);
    return exstatus;
  }
  push_losefn_sysCx(close_FildeshAlloc_generic, global_alloc);

  cmds = AllocT(TableT(Command), 1);
  InitTable(*cmds);
  push_losefn_sysCx(lose_Commands, cmds);

  if (use_stdin) {
    script_in = open_arg_FildeshXF(0, argv, inputv);
    cmd_hookup->stdin_fd = -1;
    PushTable( script_args, cons1_AlphaTab("/dev/stdin") );
  }

  while (argi < argc) {
    const char* arg = argv[argi++];
    PushTable( script_args, cons1_AlphaTab (arg) );
  }

  if (script_args.sz > 0)
  {
    SymVal* sym;
    SymValKind sym_kind;
    Command* cmd = Grow1Table( *cmds );
    truncate_FildeshO(tmp_out);
    print_int_FildeshO(tmp_out, (int)(script_args.sz-1));

    init_Command(cmd, global_alloc);
    cmd->kind = HereDocCommand;
    cmd->line_num = 0;
    cmd->line = strdup_FildeshAlloc(global_alloc, "$(H: #)");
    cmd->doc = strdup_FildeshO(tmp_out, global_alloc);

    sym_kind = parse_sym(cmd->line, false);
    assert(sym_kind == HereDocVal);
    sym = getf_SymVal(&cmd_hookup->map, cmd->line);
    sym->kind = HereDocVal;
    sym->as.here_doc = cmd->doc;

    while (script_args.sz < 10) {
      PushTable( script_args, cons1_AlphaTab ("") );
    }
  }

  for (i = 0; i < script_args.sz; ++i) {
    SymVal* sym;
    SymValKind sym_kind;
    Command* cmd = Grow1Table( *cmds );

    truncate_FildeshO(tmp_out);
    puts_FildeshO(tmp_out, "$(H: ");
    print_int_FildeshO(tmp_out, (int)i);
    putc_FildeshO(tmp_out, ')');

    init_Command(cmd, global_alloc);
    cmd->kind = HereDocCommand;
    cmd->line_num = 0;
    cmd->line = strdup_FildeshO(tmp_out, global_alloc);
    cmd->doc = strdup_FildeshAlloc(
        global_alloc, ccstr_of_AlphaTab(&script_args.s[i]));
    lose_AlphaTab(&script_args.s[i]);

    sym_kind = parse_sym(cmd->line, false);
    assert(sym_kind == HereDocVal);
    sym = getf_SymVal(&cmd_hookup->map, cmd->line);
    sym->kind = HereDocVal;
    sym->as.here_doc = cmd->doc;
  }
  LoseTable( script_args );

  if (cmd_hookup->stdin_fd == 0) {
    cmd_hookup->stdin_fd = fildesh_compat_fd_claim(cmd_hookup->stdin_fd);
  }
  if (cmd_hookup->stdout_fd == 1) {
    cmd_hookup->stdout_fd = fildesh_compat_fd_claim(cmd_hookup->stdout_fd);
  }
  /* Stderr stays at fd 2. It is closed on or immediately prior to exit.*/

  fildesh_compat_errno_trace();
  while (exstatus == 0 && script_in) {
    FildeshAlloc* scope_alloc = open_FildeshAlloc();
    istat = parse_file(
        cmds, script_in, filename_FildeshXF(script_in),
        &cmd_hookup->map, scope_alloc, global_alloc, tmp_out);
    if (istat != 0 ||
        (cmds->sz == 0 || cmds->s[cmds->sz-1].kind != BarrierCommand))
    {
      close_FildeshX(script_in);
      fildesh_compat_errno_trace();
      script_in = NULL;
    }
    if (istat == 0) {
      fildesh_compat_errno_trace();
      istat = setup_commands(cmds, cmd_hookup);
      fildesh_compat_errno_trace();
    }
    if (exstatus == 0 && istat != 0) {
      exstatus = 65;
    }
    if (false && exstatus == 0) {
      for (i = 0; i < cmds->sz; ++i) {
        output_Command (stderr, &cmds->s[i]);
      }
    }

    if (exstatus == 0) {
      fildesh_compat_errno_trace();
      istat = spawn_commands(fildesh_exe, *cmds, alias_map, forkonly);
      fildesh_compat_errno_trace();
    }
    if (exstatus == 0 && istat != 0) {
      exstatus = 126;
    }

    istat = 0;
    for (i = 0; i < cmds->sz; ++i) {
      Command* cmd = &cmds->s[i];
      if (false && exstatus == 0) {
        output_Command(stderr, cmd);
      }
      if ((cmd->kind == RunCommand || cmd->kind == DefCommand) && cmd->pid >= 0) {
        if (cmd->pid == 0) {
          pthread_join(cmd->thread, NULL);
        } else {
          cmd->status = fildesh_compat_sh_wait(cmd->pid);
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
    cmds->sz = 0;
    fildesh_compat_errno_trace();
    close_FildeshAlloc(scope_alloc);

    if (exstatus == 0) {
      exstatus = istat;
    }
  }
  close_FildeshX(script_in);  /* Just in case we missed it.*/
  free_CommandHookup(cmd_hookup, &istat);
  lose_strmap(alias_map);
  close_FildeshO(tmp_out);

  return exstatus;
}

