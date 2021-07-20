
#include "lace.h"
#include "lace_compat_fd.h"
#include "lace_compat_file.h"
#include "lace_compat_sh.h"
#include "utilace.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static
  void
show_usage()
{
#define f(s)  fputs(s, stderr); fputc('\n', stderr);
  f( "Usage: execfd [-exe name] [argi]* -- [arg|fd]*" );
  f( "  Each /argi/ is an index which has a file descriptor in it." );
  f( "  Index zero is the executable name." );
#undef f
}

/** Read from the file descriptor /in/ and write to file /name/.
 * If the input stream contains no data, the file will not be
 * written (or overwritten).
 **/
static
  void
pipe_to_file(lace_fd_t fd, const char* name)
{
  LaceX* in = open_fd_LaceX(fd);
  LaceO* out = NULL;

  if (in) {
    read_LaceX(in);
    if (in->size > 0) {
      out = open_LaceOF(name);
    }
  }

  if (in && out) {
    for (; in->size > 0; read_LaceX(in)) {
      memcpy(grow_LaceO(out, in->size), in->at, in->size);
      maybe_flush_LaceO(out);
      in->size = 0;
    }
  }
  close_LaceX(in);
  close_LaceO(out);
}


static
  char*
readin_fd(lace_fd_t fd, bool scrap_newline)
{
  LaceX* in = open_fd_LaceX(fd);
  char* s = slurp_LaceX(in);
  if (scrap_newline && in->size >= 1 && s[in->size-1] == '\n') {
    s[in->size-1] = '\0';
    if (in->size >= 2 && s[in->size-2] == '\r') {
      s[in->size-2] = '\0';
    }
  }
  in->at = NULL;
  in->alloc_lgsize = 0;
  close_LaceX(in);
  return s;
}

  int
main_execfd(unsigned argc, char** argv)
{
  int exstatus = 0;
  unsigned argi = 1;
  unsigned off = 0;
  unsigned i;
  lace_fd_t stdin_fd = 0;
  lace_fd_t stdout_fd = 1;
  unsigned char* bt = (unsigned char*) malloc(argc);
  char* exe = 0;

  memset(bt, 0, argc);

  while (argv[argi] && 0 != strcmp(argv[argi], "--") && exstatus == 0)
  {
    if (0 == strcmp(argv[argi], "-exe")) {
      exe = argv[++argi];
    } else if (0 == strcmp(argv[argi], "-stdin")) {
      stdin_fd = lace_arg_open_readonly(argv[++argi]);
      if (stdout_fd < 0) {
        lace_log_errorf("Cannot open -stdin: %s", argv[argi]);
        exstatus = 64;
      }
    } else if (0 == strcmp(argv[argi], "-stdout")) {
      stdout_fd = lace_arg_open_writeonly(argv[++argi]);
      if (stdout_fd < 0) {
        lace_log_errorf("Cannot open -stdout: %s", argv[argi]);
        exstatus = 64;
      }
    } else {
      int idx = 0;
      if (lace_parse_int(&idx, argv[argi]) && idx >= 0) {
        bt[idx] = 1;
      } else {
        lace_log_errorf("Cannot parse index from arg: %s", argv[argi]);
        show_usage();
        exstatus = 64;
      }
    }
    ++ argi;
  }

  if (exstatus == 0) {
    if (argv[argi] && argv[argi+1]) {
      argi += 1;
      off = argi;
    } else {
      exstatus = 64;
    }
  }

  for (i = 0; i < argc-off && exstatus == 0; ++i) {
    int fd = -1;
    if (bt[i] == 0) {continue;}
    if (!lace_parse_int(&fd, argv[off+i]) || fd < 0) {
      lace_log_errorf("Cannot parse fd from arg: %s", argv[off+i]);
      exstatus = 64;
    } else if (i == 0) {
      if (exe) {
        pipe_to_file(fd, exe);
        argv[off+i] = exe;
        lace_compat_file_chmod_u_rwx(argv[off+i], 1, 1, 1);
      } else {
        lace_log_error("Need to provide -exe argument.");
        exstatus = 64;
      }
    } else {
      argv[off+i] = readin_fd(fd, true);
    }
  }

  if (exstatus != 0) {
    show_usage();
    free(bt);
    return exstatus;
  }

#if defined(LACE_BUILTIN_LIBRARY) || defined(UNIT_TESTING)
  if (lace_specific_util(argv[off])) {
    argv[off-2] = argv[0];
    argv[off-1] = "-as";
    exstatus = lace_compat_fd_spawnvp_wait(
        stdin_fd, stdout_fd, 2, NULL, (const char**)&argv[off-2]);
  } else {
    exstatus = lace_compat_fd_spawnvp_wait(
        stdin_fd, stdout_fd, 2, NULL, (const char**)&argv[off]);
  }
#else
  exstatus = -1;
  lace_compat_fd_move_to(0, stdin_fd);
  lace_compat_fd_move_to(1, stdout_fd);
  lace_compat_sh_exec((const char**)&argv[off]);
#endif

  for (i = 1; i < argc-off; ++i) {
    if (bt[i] != 0) {
      free(argv[off+i]);
    }
  }
  free(bt);

  if (exstatus < 0) {exstatus = 126;}
  return exstatus;
}

#if !defined(LACE_BUILTIN_LIBRARY) && !defined(UNIT_TESTING)
  int
main(int argc, char** argv)
{
  return main_execfd((unsigned)argc, argv);
}
#endif
