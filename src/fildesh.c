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

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Defined in main_fildesh.c.*/
void push_fildesh_exit_callback(void (*f) (void*), void* x);


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
  IOFileVal,
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


static SymValKind parse_sym(char* s, bool firstarg);

struct Command
{
  char* line;
  unsigned line_num;
  CommandKind kind;
  DECLARE_FildeshAT(char*, args);
  DECLARE_FildeshAT(char*, tmp_files);
  pthread_t thread;
  fildesh_compat_pid_t pid;
  int status;
  fildesh_fd_t stdis; /**< Standard input stream.**/
  DECLARE_FildeshAT(int, is); /**< Input streams.**/
  fildesh_fd_t stdos; /**< Standard output stream.**/
  DECLARE_FildeshAT(int, os); /**< Output streams.**/
  /* Exit status stream.**/
  fildesh_fd_t status_fd;
  /** File descriptor to close upon exit.**/
  DECLARE_FildeshAT(int, exit_fds);
  /** If >= 0, this is a file descriptor that will
   * close when the program command is safe to run.
   **/
  fildesh_fd_t exec_fd;
  /** Whether exec_fd actually has bytes, rather than just used for signaling.*/
  bool exec_fd_has_bytes;
  /** If != NULL, this is the contents of a file to execute.**/
  const char* exec_doc;

  /** Use these input streams to fill corresponding (null) arguments.**/
  DECLARE_FildeshAT(struct { int fd; bool scrap_newline; }, iargs);

  /** Use this if it's a HERE document.**/
  char* doc;

  /** Use this to allocate stuff.
   **/
  FildeshAlloc* alloc;
};

/* See the setup_commands() phase.*/
struct CommandHookup {
  char* temporary_directory;
  unsigned tmpfile_count;
  FildeshKV map;
  FildeshKV add_map; /* Temporarily hold new symbols for the current line.*/
  fildesh_fd_t stdin_fd;
  fildesh_fd_t stdout_fd;
  /* Stderr stays at fd 2 but should be closed explicitly if we dup2 over it.*/
  bool stderr_fd_opened;
};

struct SymVal
{
  SymValKind kind;
  unsigned arg_idx;  /**< If a file.**/
  unsigned ios_idx;
  unsigned cmd_idx;
  union SymVal_union
  {
    int file_desc;
    char* here_doc;
    const char* iofilename;  /* IOFileVal only.*/
  } as;
};

struct BuiltinCommandThreadArg {
  Command* command;  /* Cleanup but don't free.*/
  char** argv;  /* Free nested.*/
};

static char* add_tmp_file(CommandHookup*, const char*, FildeshAlloc*);

  static void
init_SymVal (SymVal* v)
{
  v->kind = NSymValKinds;
}

  static void
lose_SymVal (SymVal* v)
{
  v->kind = NSymValKinds;
}

  static void
init_Command(Command* cmd, FildeshAlloc* alloc)
{
  cmd->kind = NCommandKinds;
  cmd->line_num = 0;
  init_FildeshAT(cmd->args);
  init_FildeshAT(cmd->tmp_files);
  cmd->pid = -1;
  cmd->stdis = -1;
  init_FildeshAT(cmd->is);
  cmd->stdos = -1;
  init_FildeshAT(cmd->os);
  cmd->status_fd = -1;
  init_FildeshAT(cmd->exit_fds);
  cmd->exec_fd = -1;
  cmd->exec_fd_has_bytes = false;
  cmd->exec_doc = NULL;
  init_FildeshAT(cmd->iargs);
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
  for (i = 0; i < count_of_FildeshAT(cmd->is); ++i) {
    fildesh_compat_fd_close((*cmd->is)[i]);
  }
  close_FildeshAT(cmd->is);

  for (i = 0; i < count_of_FildeshAT(cmd->os); ++i) {
    fildesh_compat_fd_close((*cmd->os)[i]);
  }
  close_FildeshAT(cmd->os);

  if (cmd->status_fd >= 0) {
    fildesh_compat_fd_close(cmd->status_fd);
    cmd->status_fd = -1;
  }
  close_FildeshAT(cmd->exit_fds);

  cmd->exec_fd = -1;
  cmd->exec_fd_has_bytes = false;
  cmd->exec_doc = NULL;

  close_FildeshAT(cmd->iargs);
}

static
  fildesh_compat_fd_t*
build_fds_to_inherit_Command(Command* cmd)
{
  size_t i, off;
  fildesh_compat_fd_t* fds = (fildesh_compat_fd_t*)
    malloc(sizeof(fildesh_compat_fd_t) *
           (count_of_FildeshAT(cmd->is) +
            count_of_FildeshAT(cmd->os) +
            1));

  off = 0;
  for (i = 0; i < count_of_FildeshAT(cmd->is); ++i) {
    fds[off++] = (*cmd->is)[i];
  }
  close_FildeshAT(cmd->is);
  for (i = 0; i < count_of_FildeshAT(cmd->os); ++i) {
    fds[off++] = (*cmd->os)[i];
  }
  close_FildeshAT(cmd->os);
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
      close_FildeshAT(cmd->args);
      break;
    case HereDocCommand:
      break;
    default:
      break;
  }

  for (i = 0; i < count_of_FildeshAT(cmd->tmp_files); ++i) {
    fildesh_compat_file_rm((*cmd->tmp_files)[i]);
  }
  close_FildeshAT(cmd->tmp_files);
  cmd->kind = NCommandKinds;
}

  static void
lose_Commands (void* arg)
{
  Command** cmds = (Command**) arg;
  unsigned i;
  for (i = 0; i < count_of_FildeshAT(cmds); ++i) {
    if ((*cmds)[i].kind == RunCommand && (*cmds)[i].pid > 0)
      fildesh_compat_sh_kill((*cmds)[i].pid);
    if ((*cmds)[i].kind != NCommandKinds)
      lose_Command (&(*cmds)[i]);
  }
  close_FildeshAT(cmds);
  free(cmds);
}

  static CommandHookup*
new_CommandHookup(FildeshAlloc* alloc)
{
  const FildeshKV empty_map = DEFAULT_FildeshKV_SINGLE_LIST;
  CommandHookup* cmd_hookup = fildesh_allocate(CommandHookup, 1, alloc);
  cmd_hookup->map = empty_map;
  cmd_hookup->add_map = empty_map;
  cmd_hookup->temporary_directory = NULL;
  cmd_hookup->tmpfile_count = 0;
  cmd_hookup->stdin_fd = 0;
  cmd_hookup->stdout_fd = 1;
  cmd_hookup->stderr_fd_opened = false;
  return cmd_hookup;
}

static void free_CommandHookup(CommandHookup* cmd_hookup, int* istat) {
  FildeshKV_id_t id;
  FildeshKV* map = &cmd_hookup->map;
  for (id = any_id_FildeshKV(map);
       !fildesh_nullid(id);
       id = any_id_FildeshKV(map))
  {
    SymVal* x = (SymVal*) value_at_FildeshKV(map, id);
    if (x->kind == ODescVal && *istat == 0) {
      fildesh_log_errorf("Dangling output stream! Symbol: %s",
                         (char*) key_at_FildeshKV(map, id));
      *istat = -1;
    }
    else if (x->kind == IOFileVal) {
      fildesh_compat_file_rm(x->as.iofilename);
    }
    lose_SymVal(x);
    remove_at_FildeshKV(map, id);
  }
  close_FildeshKV(&cmd_hookup->map);
  close_FildeshKV(&cmd_hookup->add_map);
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

static inline
  bool
eq_cstr(const char* a, const char* b)
{
  if (a == b)  return true;
  if (!a)  return false;
  if (!b)  return false;
  return (0 == strcmp (a, b));
}

static inline
  bool
pfxeq_cstr(const char* pfx, const char* s)
{
  if (!s)  return false;
  return (0 == strncmp(pfx, s, strlen (pfx)));
}

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


static void ensure_strmap(FildeshKV* map, char* k, char* v) {
  const FildeshKV_id_t id = ensure_FildeshKV(map, k, strlen(k)+1);
  assign_at_FildeshKV(map, id, v, strlen(v)+1);
}

static char* lookup_strmap(FildeshKV* map, const char* k) {
  return (char*) lookup_value_FildeshKV(map, k, strlen(k)+1);
}

static
  SymVal*
lookup_SymVal(FildeshKV* map, const char* s)
{
  return (SymVal*) lookup_value_FildeshKV(map, s, strlen(s)+1);
}

static
  SymVal*
getf_SymVal(FildeshKV* map, const char* s, FildeshAlloc* alloc)
{
  FildeshKV_id_t id = ensure_FildeshKV(map, s, strlen(s)+1);
  SymVal* x = (SymVal*) value_at_FildeshKV(map, id);

  if (!x) {
    x = fildesh_allocate(SymVal, 1, alloc);
    init_SymVal(x);
    assign_at_FildeshKV(map, id, x, sizeof(*x));
  }
  return x;
}

static
  SymVal*
declare_SymVal(FildeshKV* map, SymValKind kind,
               const char* name, FildeshAlloc* alloc)
{
  SymVal* x = getf_SymVal(map, name, alloc);
  if (x->kind == IOFileVal) {
    fildesh_log_errorf("Cannot redefine tmpfile symbol: %s", name);
    return NULL;
  }
  x->kind = kind;
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
parse_double_quoted_string(FildeshO* out, char* s, FildeshKV* map)
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
sep_line(char*** args, char* s, FildeshKV* map, FildeshAlloc* alloc, FildeshO* tmp_out)
{
  while (1) {
    s = &s[count_ws (s)];
    if (s[0] == '\0')  break;

    if (s[0] == '\'') {
      unsigned i;
      s = &s[1];
      push_FildeshAT(args, s);
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
      push_FildeshAT(args, strdup_FildeshO(tmp_out, alloc));
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
        push_FildeshAT(args, strdup_FildeshAlloc(alloc, v));
      }
      else {
        s = NULL;
        fildesh_log_error("Unterminated environment variable.");
        break;
      }
    }
    else if (s[0] == '$' && s[1] == '(') {
      unsigned i;
      push_FildeshAT(args, s);
      s = &s[2];
      i = strcspn (s, ")");
      if (s[i] == '\0') {
        fildesh_log_warning("Unterminated variable.");
        break;
      }
      s = &s[i+1];
    }
    else {
      push_FildeshAT(args, s);
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
    CommandHookup* cmd_hookup,
    Command** cmds,
    FildeshX* in,
    const char* this_filename,
    FildeshKV* map,
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
    cmd = grow1_FildeshAT(cmds);
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
      sym = declare_SymVal(map, HereDocVal, cmd->line, global_alloc);
      if (!sym) {istat = -1; break;}
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
      lose_Command(&last_FildeshAT(cmds));
      mpop_FildeshAT(cmds, 1);

      istat = parse_file(cmd_hookup, cmds, src, filename_FildeshXF(src),
                         map, scope_alloc, global_alloc, tmp_out);
      if (istat == 0 && count_of_FildeshAT(cmds) > 0 &&
          last_FildeshAT(cmds).kind == BarrierCommand)
      {
        perror_Command(
            &last_FildeshAT(cmds),
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

      push_FildeshAT(cmd->args, (char*) "zec");
      push_FildeshAT(cmd->args, (char*) "/");
      istat = sep_line(cmd->args, begline, map, scope_alloc, tmp_out);
      if (istat != 0) {
        break;
      }

      concatenated_args = fildesh_syntax_maybe_concatenate_args(
          (unsigned) count_of_FildeshAT(cmd->args)-2,
          (const char* const*)(void*)&(*cmd->args)[2],
          global_alloc);
      if (concatenated_args) {
        SymVal* sym = declare_SymVal(map, HereDocVal, sym_name, global_alloc);
        if (!sym) {istat = -1; break;}
        sym->as.here_doc = concatenated_args;

        cmd->kind = HereDocCommand;
        cmd->doc = concatenated_args;
        close_FildeshAT(cmd->args);
        continue;
      }
      push_FildeshAT(cmd->args, (char*)"/");

      {
        char* buf = fildesh_allocate(char, 6+strlen(sym_name), scope_alloc);
        sprintf(buf, "$(O %s)", sym_name);
        push_FildeshAT(cmd->args, buf);
      }

      cmd = grow1_FildeshAT(cmds);
      init_Command(cmd, scope_alloc);
      cmd->kind = DefCommand;
      cmd->line_num = text_nlines;
      push_FildeshAT(cmd->args, (char*)"elastic");
      {
        char* buf = fildesh_allocate(char, 6+strlen(sym_name), scope_alloc);
        sprintf(buf, "$(X %s)", sym_name);
        push_FildeshAT(cmd->args, buf);
      }
      cmd->line = sym_name;
    }
    else if (pfxeq_cstr("$(tmpfile ", line))
    {
      char* begline;
      char* sym_name = line;
      SymVal* sym;

      sym_name = &sym_name[count_non_ws(sym_name)];
      sym_name = &sym_name[count_ws(sym_name)];
      begline = strchr(sym_name, ')');
      if (!begline) {
        perror_Command(cmd, "Unclosed paren in tmpfile def.", 0);
        istat = -1;
        break;
      }
      begline[0] = '\0';
      sym = declare_SymVal(map, IOFileVal, sym_name, global_alloc);
      if (!sym) {istat = -1; break;}
      sym->as.iofilename = add_tmp_file(cmd_hookup, ".txt", global_alloc);

      lose_Command(cmd);
      mpop_FildeshAT(cmds, 1);
    }
    else
    {
      cmd->kind = RunCommand;
      istat = sep_line(cmd->args, cmd->line, map, scope_alloc, tmp_out);
      if (istat == 0 && 0 == strcmp((*cmd->args)[0], "$(barrier)")) {
        if (count_of_FildeshAT(cmd->args) != 1) {
          perror_Command(cmd, "Barrier does not accept args.", 0);
          istat = -1;
        }
        cmd->kind = BarrierCommand;
        close_FildeshAT(cmd->args);
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
      if (s[o+2] == 'F')  kind = IOFileVal;
      else                kind = IODescVal;
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
    idx = count_of_FildeshAT(cmd->is);
    push_FildeshAT(cmd->is, in);
  }

  if (out >= 0) {
    idx = count_of_FildeshAT(cmd->os);
    push_FildeshAT(cmd->os, out);
  }
  return idx;
}

  static void
add_iarg_Command (Command* cmd, int in, bool scrap_newline)
{
  grow_FildeshAT(cmd->iargs, 1);
  last_FildeshAT(cmd->iargs).fd = in;
  last_FildeshAT(cmd->iargs).scrap_newline = scrap_newline;
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

  char*
add_tmp_file(CommandHookup* cmd_hookup, const char* extension,
             FildeshAlloc* alloc)
{
  char buf[2048];
  assert(extension);
  if (!cmd_hookup->temporary_directory) {
    cmd_hookup->temporary_directory = fildesh_compat_file_mktmpdir("fildesh");
    fildesh_compat_errno_clear();
    if (!cmd_hookup->temporary_directory) {
      fildesh_log_error("Unable to create temp directory.");
      return NULL;
    }
    push_fildesh_exit_callback(remove_tmppath, cmd_hookup->temporary_directory);
  }

  sprintf(buf, "%s/%u%s", cmd_hookup->temporary_directory,
          cmd_hookup->tmpfile_count, extension);
  cmd_hookup->tmpfile_count += 1;
  return strdup_FildeshAlloc(alloc, buf);
}

  static char*
add_tmp_file_Command(Command* cmd,
                     CommandHookup* cmd_hookup,
                     const char* extension)
{
  push_FildeshAT(cmd->tmp_files, add_tmp_file(cmd_hookup, extension, cmd->alloc));
  return (*cmd->tmp_files)[count_of_FildeshAT(cmd->tmp_files) - 1];
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
  push_FildeshAT(elastic_cmd->args, add_fd_arg_Command(elastic_cmd, fd[1]));
  return fd[0];
}

#define FailBreak(cmd, msg, arg) { \
  perror_Command(cmd, msg, arg); \
  istat = -1; \
  break; \
}

static
  int
transfer_map_entries(FildeshKV* map, FildeshKV* add_map, const Command* cmd)
{
  int istat = 0;
  FildeshKV_id_t add_id;
  for (add_id = any_id_FildeshKV(add_map);
       !fildesh_nullid(add_id);
       add_id = any_id_FildeshKV(add_map))
  {
    const char* add_key = (const char*) key_at_FildeshKV(add_map, add_id);
    const SymVal* add_sym = (const SymVal*) value_at_FildeshKV(add_map, add_id);
    const FildeshKV_id_t id = ensure_FildeshKV( map, add_key, size_of_key_at_FildeshKV(add_map, add_id));
    const SymVal* sym = (const SymVal*) value_at_FildeshKV(map, id);
    if (sym && !(sym->kind==NSymValKinds || sym->kind==HereDocVal || sym->kind==DefVal)) {
      FailBreak(cmd, "Trying to overwrite an existing stream variable", add_key);
    }
    assign_at_FildeshKV(map, id, add_sym, sizeof(*add_sym));
    remove_at_FildeshKV(add_map, add_id);
  }
  return istat;
}

static
  int
setup_commands(Command** cmds, CommandHookup* cmd_hookup,
               FildeshAlloc* global_alloc)
{
  FildeshKV* map = &cmd_hookup->map;
  /* Temporarily hold new symbols for the current line.*/
  FildeshKV* add_map = &cmd_hookup->add_map;
  int istat = 0;
  unsigned i;

  for (i = 0; i < count_of_FildeshAT(cmds) && istat == 0; ++i) {
    unsigned arg_q = 0;
    unsigned arg_r = 0;
    Command* cmd = &(*cmds)[i];

    /* The command defines a HERE document.*/
    if (cmd->kind == HereDocCommand)
    {
      /* Command symbol was parsed during parsing
       * but we need to overwrite the symbol
       * just in case there are multiple occurrences.
       */
      SymVal* sym = declare_SymVal(map, HereDocVal, cmd->line, global_alloc);
      if (!sym) {istat = -1; break;}
      sym->as.here_doc = cmd->doc;

      /* The loops should not run.*/
      assert( count_of_FildeshAT(cmd->args) == 0 ); /* Invariant.*/
    }

    for (arg_r = 0; arg_r < count_of_FildeshAT(cmd->args) && istat == 0; ++ arg_r) {
      char* arg = (*cmd->args)[arg_r];
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

    for (arg_r = 0; arg_r < count_of_FildeshAT(cmd->args) && istat == 0; ++ arg_r)
    {
      char* arg = (*cmd->args)[arg_r];
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
        SymVal* sym = getf_SymVal(map, arg, global_alloc);
        if (sym->kind == HereDocVal) {
          (*cmd->args)[arg_q] = sym->as.here_doc;
        }
        else if (sym->kind == ODescVal) {
          fildesh_fd_t fd = sym->as.file_desc;
          sym->kind = NSymValKinds;
          add_iarg_Command (cmd, fd, true);
          (*cmd->args)[arg_q] = NULL;
        }
        else if (sym->kind == DefVal) {
          fildesh_fd_t fd = pipe_from_elastic(&(*cmds)[sym->cmd_idx]);
          if (fd < 0) {
            FailBreak(cmd, "Failed to create pipe for variable", arg);
          }
          add_iarg_Command (cmd, fd, true);
          (*cmd->args)[arg_q] = NULL;
        }
        else {
          FailBreak(cmd, "Unknown source for argument", arg);
        }
        ++ arg_q;
      }
      else if (cmd->kind == StdoutCommand && kind == IDescVal)
      {
        SymVal* sym = getf_SymVal(map, arg, global_alloc);
        Command* last = &(*cmds)[sym->cmd_idx];

        if (last->kind != RunCommand) {
          FailBreak(cmd, "Stdout stream not coming from a command?", arg);
        }

        fildesh_compat_fd_close(sym->as.file_desc);

        if (sym->ios_idx < count_of_FildeshAT(last->os)) {
          fildesh_compat_fd_move_to((*last->os)[sym->ios_idx], cmd->stdos);
        }
        else {
          fildesh_compat_fd_move_to(last->stdos, cmd->stdos);
        }
        sym->kind = NSymValKinds;
        cmd->stdos = -1;
      }
      else if (kind == IOFileVal) {
        SymVal* sym = getf_SymVal(map, arg, global_alloc);
        if (sym->kind != IOFileVal) {
          FailBreak(cmd, "Not declared as a file", arg);
        }
        (*cmd->args)[arg_q] = (char*)sym->as.iofilename;
        arg_q += 1;
      }
      else if (kind == IDescVal ||
               kind == IDescFileVal ||
               kind == IODescVal)
      {
        SymVal* sym = getf_SymVal(map, arg, global_alloc);
        int fd = sym->as.file_desc;
        if (sym->kind == HereDocVal) {
          /* Do nothing.*/
        }
        else if (sym->kind == ODescVal) {
          sym->kind = NSymValKinds;
        }
        else if (sym->kind == DefVal) {
          fd = pipe_from_elastic(&(*cmds)[sym->cmd_idx]);
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
            (*cmd->args)[arg_q] = filename;
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
            (*cmd->args)[arg_q] = add_fd_arg_Command (cmd, fd);
          } else {
            (*cmd->args)[0] = add_tmp_file_Command(cmd, cmd_hookup, ".exe");
            if (!(*cmd->args)[0]) {
              FailBreak(cmd, "Cannot create tmpfile for executable.", NULL);
            }
            if (sym->arg_idx < UINT_MAX) {
              static const char dev_fd_prefix[] = "/dev/fd/";
              static const unsigned dev_fd_prefix_length =
                sizeof(dev_fd_prefix)-1;
              Command* src_cmd = &(*cmds)[sym->cmd_idx];
              char* src_fd_filename = (*src_cmd->args)[sym->arg_idx];
              int src_fd = -1;
              assert(0 == memcmp(src_fd_filename, dev_fd_prefix,
                                 dev_fd_prefix_length));
              fildesh_parse_int(&src_fd,
                                &src_fd_filename[dev_fd_prefix_length]);
              assert(src_fd >= 0);

              push_FildeshAT(src_cmd->exit_fds, src_fd);
              /* Replace fd path with tmpfile name.*/
              (*src_cmd->args)[sym->arg_idx] = (*cmd->args)[0];
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
        SymVal* sym = getf_SymVal(map, arg, global_alloc);
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
            (*cmd->args)[arg_q] = add_fd_arg_Command(cmd, fd);
          }
          else
          {
            char* s;
            s = add_tmp_file_Command(&(*cmds)[sym->cmd_idx],
                cmd_hookup, ".exe");
            if (!s) {
              FailBreak(cmd, "Cannot create tmpfile for executable.", NULL);
            }
            (*(*cmds)[sym->cmd_idx].args)[0] = s;
            (*cmd->args)[arg_q] = s;
          }
          ++ arg_q;
        }
      }

      if (cmd->kind == StdinCommand && kind == ODescVal)
      {
        SymVal* sym = declare_SymVal(add_map, ODescVal, arg, global_alloc);
        if (!sym) {istat = -1; break;}
        sym->cmd_idx = i;
        sym->as.file_desc = cmd->stdis;
        cmd->stdis = -1;
        sym->arg_idx = UINT_MAX;
        sym->ios_idx = UINT_MAX;
      }
      else if (kind == ODescVal || kind == IODescVal ||
               kind == ODescStatusVal ||
               kind == ODescFileVal)
      {
        fildesh_fd_t fd[2];
        SymVal* sym = declare_SymVal(add_map, ODescVal, arg, global_alloc);
        if (!sym) {istat = -1; break;}
        sym->cmd_idx = i;
        sym->arg_idx = UINT_MAX;
        sym->ios_idx = UINT_MAX;

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
          (*cmd->args)[arg_q] = add_fd_arg_Command(cmd, fd[1]);
          sym->arg_idx = arg_q;
          ++ arg_q;
        }
      }
      else if (kind == IFutureDescVal || kind == IFutureDescFileVal)
      {
        fildesh_fd_t fd[2];
        SymVal* sym = declare_SymVal(add_map, IFutureDescVal, arg, global_alloc);
        if (!sym) {istat = -1; break;}
        sym->cmd_idx = i;
        sym->arg_idx = UINT_MAX;
        sym->ios_idx = UINT_MAX;

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
          (*cmd->args)[arg_q] = add_fd_arg_Command(cmd, fd[0]);
          ++ arg_q;
        }
      }

      if (kind == NSymValKinds) {
        (*cmd->args)[arg_q] = arg;
        ++ arg_q;
      }
    }

    /* This guard might not be necessary.*/
    if (istat != 0) {break;}

    if (count_of_FildeshAT(cmd->args) > 0)
      mpop_FildeshAT(cmd->args, count_of_FildeshAT(cmd->args) - arg_q);

    if (cmd->kind == DefCommand) {
      SymVal* sym = declare_SymVal(map, DefVal, cmd->line, global_alloc);
      if (!sym) {istat = -1; break;}
      sym->cmd_idx = i;
    }

    istat = transfer_map_entries(map, add_map, cmd);
  }
  return istat;
}

  static void
output_Command(FILE* out, const Command* cmd)
{
  unsigned i;
  if (cmd->kind != RunCommand)  return;

  fputs ("COMMAND: ", out);
  for (i = 0; i < (unsigned)count_of_FildeshAT(cmd->args); ++i) {
    if (i > 0)  fputc(' ', out);
    if ((*cmd->args)[i]) {
      fputs((*cmd->args)[i], out);
    } else {
      fputs("NULL", out);
    }
  }
  fputc('\n', out);
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

  assert(count_of_FildeshAT(cmd->iargs) == 0);
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
  mpop_FildeshAT(cmd->is, count_of_FildeshAT(cmd->is));
  mpop_FildeshAT(cmd->os, count_of_FildeshAT(cmd->os));
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
fix_known_flags_Command(Command* cmd, FildeshKV* alias_map) {
  char* replacement = lookup_strmap(alias_map, (*cmd->args)[0]);
  if (replacement) {
    (*cmd->args)[0] = replacement;
  }

  if (eq_cstr("sed", (*cmd->args)[0])) {
    unsigned i;
    for (i = 1; i < count_of_FildeshAT(cmd->args); ++i) {
      const char* arg = (*cmd->args)[i];
      if (eq_cstr("--line-buffered", arg)) {
#ifdef __APPLE__
        const char line_buffering_flag[] = "-l";
#else
        const char line_buffering_flag[] = "-u";
#endif
        (*cmd->args)[i] = strdup_FildeshAlloc(cmd->alloc, line_buffering_flag);
      }
    }
  }
  else if (eq_cstr("tr", (*cmd->args)[0])) {
    if (count_of_FildeshAT(cmd->args) == 3 &&
        1 == strlen((*cmd->args)[1]) &&
        1 == strlen((*cmd->args)[2])) {
      (*cmd->args)[0] = strdup_FildeshAlloc(cmd->alloc, "replace_string");
    }
  }
}

static
  void
add_inheritfd_flags_Command(char*** argv, Command* cmd, bool inprocess) {
  unsigned i;

  if (inprocess) {
    /* Let command inherit all input file descriptors except:
     * - The fd providing executable bytes (or signaling that they are ready).
     * - The fds providing input arguments. See inprocess else clause.
     */
    for (i = 0; i < count_of_FildeshAT(cmd->is); ++i) {
      const fildesh_fd_t fd = (*cmd->is)[i];
      if (fd != cmd->exec_fd) {
        push_FildeshAT(argv, lace_strdup("-inheritfd") );
        push_FildeshAT(argv, lace_fd_strdup(fd) );
      }
    }
    /* Let command inherit all output file descriptors except:
     * - The fds that must be closed on exit.
     */
    for (i = 0; i < count_of_FildeshAT(cmd->os); ++i) {
      const fildesh_fd_t fd = (*cmd->os)[i];
      bool inherit = true;
      unsigned j;
      for (j = 0; j < count_of_FildeshAT(cmd->exit_fds) && inherit; ++j) {
        inherit = (fd != (*cmd->exit_fds)[j]);
      }
      if (inherit) {
        push_FildeshAT(argv, lace_strdup("-inheritfd"));
        push_FildeshAT(argv, lace_fd_strdup(fd));
      }
    }

    if (cmd->stdis >= 0) {
      push_FildeshAT(argv, lace_strdup("-stdin"));
      push_FildeshAT(argv, lace_fd_path_strdup(cmd->stdis));
      push_FildeshAT(cmd->is, cmd->stdis);
    }
    cmd->stdis = -1;
    if (cmd->stdos >= 0) {
      push_FildeshAT(argv, lace_strdup("-stdout"));
      push_FildeshAT(argv, lace_fd_path_strdup(cmd->stdos));
      push_FildeshAT(cmd->os, cmd->stdos);
    }
    cmd->stdos = -1;
  } else {
    for (i = 0; i < count_of_FildeshAT(cmd->iargs); ++i) {
      add_ios_Command(cmd, (*cmd->iargs)[i].fd, -1);
    }
  }
  close_FildeshAT(cmd->iargs);

  if (cmd->exec_fd >= 0) {
    if (!cmd->exec_fd_has_bytes) {
      push_FildeshAT(argv, lace_strdup("-waitfd"));
      push_FildeshAT(argv, lace_fd_strdup(cmd->exec_fd));
    }
    add_ios_Command(cmd, cmd->exec_fd, -1);
  }
  cmd->exec_fd = -1;

  for (i = 0; i < count_of_FildeshAT(cmd->exit_fds); ++i) {
    push_FildeshAT(argv, lace_strdup("-exitfd"));
    push_FildeshAT(argv, lace_fd_strdup((*cmd->exit_fds)[i]));
  }
  close_FildeshAT(cmd->exit_fds);

  if (cmd->status_fd >= 0) {
    push_FildeshAT(argv, lace_strdup("-o?"));
    push_FildeshAT(argv, lace_fd_path_strdup(cmd->status_fd));
    push_FildeshAT(cmd->os, cmd->status_fd);
  }
  cmd->status_fd = -1;
}

  static int
spawn_commands(const char* fildesh_exe, Command** cmds,
               FildeshKV* alias_map, bool forkonly)
{
  typedef struct uint2 uint2;
  struct uint2 {
    unsigned s[2];
  };
  DECLARE_FildeshAT(char*, argv);
  DECLARE_FildeshAT(uint2, fdargs);
  unsigned i;
  int istat = 0;

  init_FildeshAT(argv);
  init_FildeshAT(fdargs);

  for (i = 0; i < count_of_FildeshAT(cmds) && istat == 0; ++i)
  {
    Command* cmd = &(*cmds)[i];
    bool use_thread = false;
    unsigned argi, j;

    if (cmd->kind != RunCommand && cmd->kind != DefCommand)  continue;

    for (argi = 0; argi < count_of_FildeshAT(cmd->args); ++argi)
    {
      if (!(*cmd->args)[argi]) {
        uint2 p;
        assert(count_of_FildeshAT(fdargs) < count_of_FildeshAT(cmd->iargs));
        p.s[0] = argi;
        p.s[1] = (*cmd->iargs)[count_of_FildeshAT(fdargs)].fd;
        push_FildeshAT(fdargs, p);
      }
    }
    assert(count_of_FildeshAT(fdargs) == count_of_FildeshAT(cmd->iargs));

    fix_known_flags_Command(cmd, alias_map);

    if (cmd->exec_fd >= 0 || count_of_FildeshAT(fdargs) > 0 ||
        cmd->status_fd >=0 || count_of_FildeshAT(cmd->exit_fds) > 0)
    {
      const char* execfd_exe = lookup_strmap(alias_map, "execfd");
      char* execfd_fmt = NULL;
      if (execfd_exe) {
        push_FildeshAT(argv, lace_strdup(execfd_exe));
      } else {
        if (!forkonly) {
          use_thread = true;
        }
        push_FildeshAT(argv, lace_strdup(fildesh_exe));
        push_FildeshAT(argv, lace_strdup("-as"));
        push_FildeshAT(argv, lace_strdup("execfd"));
      }
      if (cmd->exec_fd >= 0) {
        push_FildeshAT(argv, lace_strdup("-exe"));
        push_FildeshAT(argv, lace_strdup((*cmd->args)[0]));
        if (cmd->exec_fd_has_bytes) {
          uint2 p;
          p.s[0] = 0;
          p.s[1] = cmd->exec_fd;
          push_FildeshAT(fdargs, p);
        }
      }

      add_inheritfd_flags_Command(argv, cmd, use_thread);

      execfd_fmt = (char*)malloc(2*count_of_FildeshAT(cmd->args));
      for (j = 0; j < count_of_FildeshAT(cmd->args); ++j) {
        execfd_fmt[2*j] = 'a';
        execfd_fmt[2*j+1] = '_';
      }
      execfd_fmt[2*count_of_FildeshAT(cmd->args)-1] = '\0';

      for (j = 0; j < count_of_FildeshAT(fdargs); ++j) {
        uint2 p = (*fdargs)[j];
        (*cmd->args)[p.s[0]] = strdup_fd_Command(cmd, p.s[1]);
        execfd_fmt[2*p.s[0]] = 'x';
      }

      push_FildeshAT(argv, execfd_fmt);
      push_FildeshAT(argv, lace_strdup("--"));
      push_FildeshAT(argv, lace_strdup((*cmd->args)[0]));
    }
    else if (fildesh_specific_util ((*cmd->args)[0])) {
      if (!forkonly) {
        use_thread = fildesh_builtin_is_threadsafe((*cmd->args)[0]);
      }
      push_FildeshAT(argv, lace_strdup(fildesh_exe));
      push_FildeshAT(argv, lace_strdup("-as"));
      push_FildeshAT(argv, lace_strdup((*cmd->args)[0]));
    }
    else {
      push_FildeshAT(argv, lace_strdup((*cmd->args)[0]));
    }

    for (j = 1; j < count_of_FildeshAT(cmd->args); ++j)
      push_FildeshAT(argv, lace_strdup((*cmd->args)[j]));

    push_FildeshAT(argv, NULL);

    if (cmd->exec_doc)
    {
      cmd->exec_doc = 0;
      fildesh_compat_file_chmod_u_rwx((*cmd->args)[0], 1, 1, 1);
    }

    if (use_thread) {
      BuiltinCommandThreadArg* arg = (BuiltinCommandThreadArg*)
        malloc(sizeof(BuiltinCommandThreadArg));
      arg->command = cmd;
      arg->argv = (char**)malloc(sizeof(char*) * count_of_FildeshAT(argv));
      memcpy(arg->argv, (*argv), sizeof(char*) * count_of_FildeshAT(argv));
      cmd->pid = 0;
      istat = pthread_create(
          &cmd->thread, NULL, builtin_command_thread_fn, arg);
      if (istat < 0) {
        fildesh_log_errorf("Could not pthread_create(). File: %s", (*argv)[0]);
      }
    } else {
      fildesh_compat_fd_t* fds_to_inherit =
        build_fds_to_inherit_Command(cmd);
      cmd->pid = fildesh_compat_fd_spawnvp(
          cmd->stdis, cmd->stdos, 2, fds_to_inherit, (const char**)(*argv));
      free(fds_to_inherit);
      cmd->stdis = -1;
      cmd->stdos = -1;
      close_Command(cmd);
      if (cmd->pid < 0) {
        fildesh_log_errorf("Could not spawnvp(). File: %s", (*argv)[0]);
        istat = -1;
      }
      for (argi = 0; argi < count_of_FildeshAT(argv); ++argi)
        free((*argv)[argi]);
    }
    mpop_FildeshAT(fdargs, count_of_FildeshAT(fdargs));
    mpop_FildeshAT(argv, count_of_FildeshAT(argv));
  }
  close_FildeshAT(argv);
  close_FildeshAT(fdargs);
  return istat;
}


/** Returns an appropriate exit status.**/
static int handle_flag_tmpdir_from_env(const char* var) {
  const char* d = NULL;
  int istat;
  if (var) {
    d = getenv(var);
  }
  if (!d) {
    fildesh_log_errorf("Failed to read environment variable: %s", var);
    return 64;
  }
  istat = fildesh_compat_sh_setenv("TMPDIR", d);
#ifdef _MSC_VER
  if (istat == 0) {
    istat = fildesh_compat_sh_setenv("TEMP", d);
  }
#endif
  if (istat != 0) {
    fildesh_compat_errno_trace();
    fildesh_log_error("Failed to set testing temporary directory.");
    return 71;
  }
  return 0;
}


  int
fildesh_builtin_fildesh_main(unsigned argc, char** argv,
                             FildeshX** inputv, FildeshO** outputv)
{
  char* fildesh_exe = argv[0];
  DECLARE_FildeshAT(const char*, script_args);
  Command** cmds = NULL;
  bool use_stdin = true;
  FildeshX* script_in = NULL;
  FildeshO tmp_out[1] = {DEFAULT_FildeshO};
  FildeshAlloc* global_alloc;
  CommandHookup* cmd_hookup;
  unsigned argi = 1;
  unsigned i;
  FildeshKV alias_map[1] = {DEFAULT_FildeshKV_SINGLE_LIST};
  bool forkonly = false;
  bool exiting = false;
  int exstatus = 0;
  int istat;

  init_FildeshAT(script_args);

  /* We shouldn't have an error yet.*/
  fildesh_compat_errno_clear();

  /* With all the signal handling below,
   * primarily to clean up the temp directory,
   * it's not clear how to act as a proper builtin.
   */
  assert(!inputv);
  assert(!outputv);

  global_alloc = open_FildeshAlloc();
  cmd_hookup = new_CommandHookup(global_alloc);

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
       sym = declare_SymVal(&cmd_hookup->map, HereDocVal, k, global_alloc);
       if (!sym) {istat = -1; break;}
       sym->as.here_doc = v;
     } else {
        fildesh_log_errorf("Bad -a arg: %s", k);
        exstatus = 64;
      }
    }
    else if (eq_cstr(arg, "-tmpdir_from_env")) {
      exstatus = handle_flag_tmpdir_from_env(argv[argi++]);
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
        push_FildeshAT(script_args, "/dev/fd/something");
        script_in = open_arg_FildeshXF(argi-1, argv, inputv);
        if (!script_in) {
          fildesh_log_errorf("Cannot read script from builtin.");
          exstatus = 66;
        }
        break;
      }
      push_FildeshAT(script_args, arg);
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
    close_FildeshAT(script_args);
    close_FildeshKV(alias_map);
    free_CommandHookup(cmd_hookup, &istat);
    close_FildeshAlloc(global_alloc);
    return exstatus;
  }
  push_fildesh_exit_callback(close_FildeshAlloc_generic, global_alloc);

  {
    DECLARE_FildeshAT(Command, tmp_cmds);
    init_FildeshAT(tmp_cmds);
    cmds = (Command**)malloc(sizeof(tmp_cmds));
    memcpy(cmds, tmp_cmds, sizeof(tmp_cmds));
  }
  push_fildesh_exit_callback(lose_Commands, cmds);

  if (use_stdin) {
    script_in = open_arg_FildeshXF(0, argv, inputv);
    cmd_hookup->stdin_fd = -1;
    push_FildeshAT(script_args, "/dev/stdin");
  }

  while (argi < argc) {
    const char* arg = argv[argi++];
    push_FildeshAT(script_args, arg);
  }

  if (count_of_FildeshAT(script_args) > 0) {
    SymVal* sym;
    SymValKind sym_kind;
    Command* cmd = grow1_FildeshAT(cmds);
    truncate_FildeshO(tmp_out);
    print_int_FildeshO(tmp_out, (int)(count_of_FildeshAT(script_args)-1));

    init_Command(cmd, global_alloc);
    cmd->kind = HereDocCommand;
    cmd->line_num = 0;
    cmd->line = strdup_FildeshAlloc(global_alloc, "$(H: #)");
    cmd->doc = strdup_FildeshO(tmp_out, global_alloc);

    sym_kind = parse_sym(cmd->line, false);
    assert(sym_kind == HereDocVal);
    sym = declare_SymVal(&cmd_hookup->map, HereDocVal, cmd->line, global_alloc);
    assert(sym);
    sym->as.here_doc = cmd->doc;

    while (count_of_FildeshAT(script_args) < 10) {
      push_FildeshAT(script_args, "");
    }
  }

  for (i = 0; i < count_of_FildeshAT(script_args); ++i) {
    SymVal* sym;
    SymValKind sym_kind;
    Command* cmd = grow1_FildeshAT(cmds);

    truncate_FildeshO(tmp_out);
    puts_FildeshO(tmp_out, "$(H: ");
    print_int_FildeshO(tmp_out, (int)i);
    putc_FildeshO(tmp_out, ')');

    init_Command(cmd, global_alloc);
    cmd->kind = HereDocCommand;
    cmd->line_num = 0;
    cmd->line = strdup_FildeshO(tmp_out, global_alloc);
    cmd->doc = strdup_FildeshAlloc(global_alloc, (*script_args)[i]);

    sym_kind = parse_sym(cmd->line, false);
    assert(sym_kind == HereDocVal);
    sym = declare_SymVal(&cmd_hookup->map, HereDocVal, cmd->line, global_alloc);
    assert(sym);
    sym->as.here_doc = cmd->doc;
  }
  close_FildeshAT(script_args);

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
        cmd_hookup,
        cmds, script_in, filename_FildeshXF(script_in),
        &cmd_hookup->map, scope_alloc, global_alloc, tmp_out);
    if (istat != 0 ||
        (count_of_FildeshAT(cmds) == 0 || last_FildeshAT(cmds).kind != BarrierCommand))
    {
      close_FildeshX(script_in);
      fildesh_compat_errno_trace();
      script_in = NULL;
    }
    if (istat == 0) {
      fildesh_compat_errno_trace();
      istat = setup_commands(cmds, cmd_hookup, global_alloc);
      fildesh_compat_errno_trace();
    }
    if (exstatus == 0 && istat != 0) {
      exstatus = 65;
    }
    if (false && exstatus == 0) {
      for (i = 0; i < count_of_FildeshAT(cmds); ++i) {
        output_Command (stderr, &(*cmds)[i]);
      }
    }

    if (exstatus == 0) {
      fildesh_compat_errno_trace();
      istat = spawn_commands(fildesh_exe, cmds, alias_map, forkonly);
      fildesh_compat_errno_trace();
    }
    if (exstatus == 0 && istat != 0) {
      exstatus = 126;
    }

    istat = 0;
    for (i = 0; i < count_of_FildeshAT(cmds); ++i) {
      Command* cmd = &(*cmds)[i];
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
    mpop_FildeshAT(cmds, count_of_FildeshAT(cmds));
    fildesh_compat_errno_trace();
    close_FildeshAlloc(scope_alloc);

    if (exstatus == 0) {
      exstatus = istat;
    }
  }
  close_FildeshX(script_in);  /* Just in case we missed it.*/
  free_CommandHookup(cmd_hookup, &istat);
  close_FildeshKV(alias_map);
  close_FildeshO(tmp_out);

  return exstatus;
}

