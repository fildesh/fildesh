#include "src/builtin/fildesh_builtin.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "include/fildesh/fildesh_compat_fd.h"
#include "include/fildesh/fildesh_compat_file.h"
#include "include/fildesh/fildesh_compat_sh.h"
#include "include/fildesh/fildesh_compat_string.h"

static
  void
show_usage()
{
#define f(s)  fputs(s, stderr); fputc('\n', stderr);
  f("Usage: execfd [OPTIONS] FORMAT -- [arg|fd]*");
  f(" FORMAT uses 2 bytes to represent the type of each argument and whether to concatenate them.");
  f("   a -- Literal string argument.");
  f("   _ -- Readable file descriptor (integer).");
  f("   + -- Concatenate args.");
  f(" OPTIONS:");
  f("  -exe filename -- Name of executable to write. Only used when index 0 is present.");
  f("  -stdin filename -- Standard input for the spawned process.");
  f("  -stdout filename -- Standard output for the spawned process.");
  f("  -inheritfd fd -- File descriptor for spawned process to inherit.");
  f("    Only useful when invoked as builtin.");
  f("  -waitfd fd -- Wait on this file descriptor to close.");
  f("  -exitfd fd -- Close this file descriptor on exit.");
  f("  -o? filename -- Print exit status to this file upon exit.");
#undef f
}

/** Read from the file descriptor /in/ and write to file /name/.
 * If the input stream contains no data, the file will not be
 * written (or overwritten).
 **/
static
  int
pipe_to_file(Fildesh_fd fd, const char* name)
{
  FildeshX* in = open_fd_FildeshX(fd);
  FildeshO* out = NULL;
  int exstatus = 0;

  if (!in) {
    exstatus = 66;
    fildesh_log_errorf("Cannot open input fd: %d", fd);
  }
  if (exstatus == 0) {
    read_FildeshX(in);
    if (in->size == 0) {
      exstatus = 66;
      fildesh_log_errorf("Empty input fd: %d", fd);
    }
  }
  if (exstatus == 0) {
    out = open_FildeshOF(name);
    if (!out) {
      exstatus = 73;
      fildesh_log_errorf("Cannot open output file: %s", name);
    }
  }

  if (exstatus == 0) {
    for (; in->size > 0; read_FildeshX(in)) {
      memcpy(grow_FildeshO(out, in->size), in->at, in->size);
      maybe_flush_FildeshO(out);
      in->size = 0;
    }
  }
  close_FildeshX(in);
  close_FildeshO(out);
  return exstatus;
}


static
  void
readin_fd(FildeshO* buf, Fildesh_fd fd, bool scrap_newline)
{
  FildeshX* in = open_fd_FildeshX(fd);
  if (!in) {return;}
  slurp_FildeshX(in);
  if (scrap_newline && in->size >= 1 && in->at[in->size-1] == '\n') {
    in->size -= 1;
    if (in->size >= 1 && in->at[in->size-1] == '\r') {
      in->size -= 1;
    }
  }
  putslice_FildeshO(buf, *in);
  close_FildeshX(in);
}

  int
fildesh_builtin_execfd_main(unsigned argc, char** argv,
                            FildeshX** inputv, FildeshO** outputv)
{
  int exstatus = 0;
  unsigned argi;
  unsigned off = 0;
  unsigned i;
  Fildesh_fd stdin_fd = 0;
  Fildesh_fd stdout_fd = 1;
  DECLARE_DEFAULT_FildeshAT(Fildesh_fd, fds_to_inherit);
  DECLARE_DEFAULT_FildeshAT(Fildesh_fd, exitfds);
  DECLARE_DEFAULT_FildeshAT(const char*, spawn_argv);
  FildeshAlloc* alloc = open_FildeshAlloc();
  FildeshO* status_out = NULL;
  char* exe = NULL;
  FildeshO buf_slice = DEFAULT_FildeshO;
  const char* arg_fmt = NULL;

  assert(!inputv);
  assert(!outputv);

  if (argc < 3) {
    exstatus = 64;
  }

  argi = 1;
  while (argv[argi] && 0 != strcmp(argv[argi], "--") && exstatus == 0) {
    if (0 == strcmp(argv[argi], "-exe")) {
      exe = argv[++argi];
    } else if (0 == strcmp(argv[argi], "-stdin")) {
      stdin_fd = fildesh_arg_open_readonly(argv[++argi]);
      if (stdin_fd < 0) {
        fildesh_log_errorf("Cannot open -stdin: %s", argv[argi]);
        exstatus = 66;
      }
    } else if (0 == strcmp(argv[argi], "-stdout")) {
      stdout_fd = fildesh_arg_open_writeonly(argv[++argi]);
      if (stdout_fd < 0) {
        fildesh_log_errorf("Cannot open -stdout: %s", argv[argi]);
        exstatus = 73;
      }
    } else if (0 == strcmp(argv[argi], "-inheritfd")) {
      Fildesh_fd fd = -1;
      if (fildesh_parse_int(&fd, argv[++argi]) && fd >= 0) {
        push_FildeshAT(fds_to_inherit, fd);
      } else {
        fildesh_log_errorf("Cannot parse -inheritfd: %s", argv[argi]);
        exstatus = 64;
      }
    } else if (0 == strcmp(argv[argi], "-waitfd")) {
      Fildesh_fd fd = -1;
      if (fildesh_parse_int(&fd, argv[++argi]) && fd >= 0) {
        wait_close_FildeshX(open_fd_FildeshX(fd));
      } else {
        fildesh_log_errorf("Cannot parse -waitfd: %s", argv[argi]);
        exstatus = 64;
      }
    } else if (0 == strcmp(argv[argi], "-exitfd")) {
      Fildesh_fd fd = -1;
      if (fildesh_parse_int(&fd, argv[++argi]) && fd >= 0) {
        push_FildeshAT(exitfds, fildesh_compat_fd_claim(fd));
      } else {
        fildesh_log_errorf("Cannot parse -exitfd: %s", argv[argi]);
        exstatus = 64;
      }
    } else if (0 == strcmp(argv[argi], "-o?")) {
      status_out = open_arg_FildeshOF(++argi, argv, outputv);
      if (!status_out) {
        fildesh_log_errorf("Cannot open -o?: %s", argv[argi]);
        exstatus = 73;
      }
    } else {
      arg_fmt = argv[argi];
    }
    ++ argi;
  }
  off = argi+1;

  if (exstatus == 0) {
    if (!argv[off-1] || !argv[off] || !arg_fmt) {
      exstatus = 64;
    }
    else if (strlen(arg_fmt) != 2*(argc-off)-1) {
      fildesh_log_errorf("Format string %s length is off by %d.",
                         arg_fmt,
                         (int)strlen(arg_fmt) - (int)(2*(argc-off)-1));
      fildesh_log_errorf("argc:%u off:%u", argc, off);
      exstatus = 64;
    }
  }

  if (exstatus == 0) {
    for (i = 0; i < argc-off; ++i) {
      if (!memchr("ax", arg_fmt[2*i], 2)) {
        fildesh_log_errorf("Unrecognized format character %c.", arg_fmt[2*i]);
        exstatus = 64;
      }
      if (!memchr("_+\0", arg_fmt[2*i+1], 3)) {
        fildesh_log_errorf("Unrecognized format delimiter %c.", arg_fmt[2*i+1]);
        exstatus = 64;
      }
    }
  }

  if (exstatus == 0 && fildesh_builtin_main_fn_lookup(argv[off])) {
    push_FildeshAT(spawn_argv, argv[0]);
    push_FildeshAT(spawn_argv, "-as");
  }

  for (i = 0; off+i < argc && exstatus == 0; ++i) {
    int fd = -1;

    if (arg_fmt[2*i] == 'a') {
      putstr_FildeshO(&buf_slice, argv[off+i]);
    }
    else if (!fildesh_parse_int(&fd, argv[off+i]) || fd < 0) {
      fildesh_log_errorf("Cannot parse fd from arg: %s", argv[off+i]);
      exstatus = 64;
    }
    else if (i == 0) {
      if (exe) {
        exstatus = pipe_to_file(fd, exe);
        if (exstatus == 0) {
          putstr_FildeshO(&buf_slice, exe);
        }
      }
      else {
        fildesh_log_error("Need to provide -exe argument.");
        exstatus = 64;
      }
    }
    else {
      readin_fd(&buf_slice, fd, true);
    }

    if (arg_fmt[2*i+1] == '_' ||
        arg_fmt[2*i+1] == '\0' ||
        off+i+1 == argc)
    {
      push_FildeshAT(spawn_argv, strdup_FildeshO(&buf_slice, alloc));
      truncate_FildeshO(&buf_slice);
    }
  }

  close_FildeshO(&buf_slice);

  if (exstatus == 0 && exe) {
    fildesh_compat_file_chmod_u_rwx(exe, 1, 1, 1);
  }

  if (exstatus != 0) {
    show_usage();
  } else {
    push_FildeshAT(fds_to_inherit, -1);
    push_FildeshAT(spawn_argv, NULL);
    exstatus = fildesh_compat_fd_spawnvp_wait(
        stdin_fd, stdout_fd, 2, (*fds_to_inherit),
        *spawn_argv);
    if (status_out && exstatus >= 0) {
      print_int_FildeshO(status_out, exstatus);
      exstatus = 0;
    }
  }

  close_FildeshAT(spawn_argv);
  close_FildeshO(status_out);
  close_FildeshAlloc(alloc);

  for (i = 0; i < count_of_FildeshAT(exitfds); ++i) {
    fildesh_compat_fd_close((*exitfds)[i]);
  }
  close_FildeshAT(exitfds);
  close_FildeshAT(fds_to_inherit);

  if (exstatus < 0) {exstatus = 126;}
  return exstatus;
}

#if !defined(FILDESH_BUILTIN_LIBRARY) && !defined(UNIT_TESTING)
  int
main(int argc, char** argv)
{
  return fildesh_builtin_execfd_main((unsigned)argc, argv, NULL, NULL);
}
#endif
