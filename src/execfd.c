
#include "lace.h"
#include "utilace.h"
#include "cx/bittable.h"
#include "cx/syscx.h"

#include <stdio.h>
#include <sys/stat.h>

static
  void
show_usage_and_exit ()
{
#define f(s)  fputs(s, stderr); fputc('\n', stderr);
  f( "Usage: execfd [-exe name] [argi]* -- [arg|fd]*" );
  f( "  Each /argi/ is an index which has a file descriptor in it." );
  f( "  Index zero is the executable name." );
#undef f
  failout_sysCx ("Bad args...");
}

/** Read from the file descriptor /in/ and write to file /name/.
 * If the input stream contains no data, the file will not be
 * written (or overwritten).
 **/
static
  void
pipe_to_file(lace_fd_t fd, const char* name)
{
  LaceX* in = open_fd_LaceXF(fd);
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
  if (in) {close_LaceX(in);}
  if (out) {close_LaceO(out);}
}


static
  char*
readin_fd(lace_fd_t fd, bool scrap_newline)
{
  LaceX* in = open_fd_LaceXF(fd);
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
  unsigned argi = 1;
  unsigned off = 0;
  BitTable bt = cons2_BitTable (argc, 0);
  char* exe = 0;

  while (argv[argi] && 0 != strcmp(argv[argi], "--"))
  {
    if (0 == strcmp(argv[argi], "-exe")) {
      exe = argv[++argi];
    } else {
      int idx = 0;
      if (lace_parse_int(&idx, argv[argi]) && idx >= 0) {
        set1_BitTable(bt, (unsigned)idx);
      } else {
        DBog1( "Cannot parse index from arg: %s", argv[argi] );
        show_usage_and_exit ();
      }
    }
    ++ argi;
  }
  if (!argv[argi]) {
    show_usage_and_exit ();
  }
  off = ++ argi;

  while (argv[argi])
  {
    if (test_BitTable (bt, argi-off))
    {
      int fd = -1;
      if (!lace_parse_int(&fd, argv[argi]) || fd < 0) {
        DBog1( "Cannot parse fd from arg: %s", argv[argi] );
        show_usage_and_exit ();
      }
      else if (argi == off) {
        if (!exe) {
          failout_sysCx ("Need to provide -exe argument.");
        }

        pipe_to_file (fd, exe);
        argv[argi] = exe;
        chmodu_sysCx (argv[argi], true, true, true);
      }
      else {
        argv[argi] = readin_fd (fd, true);
        push_losefn1_sysCx ((void (*) (void*)) free, argv[argi]);
      }
    }
    ++ argi;
  }

  lose_BitTable (&bt);

  if (lace_specific_util(argv[off])) {
    return lace_builtin_main(argc-off, &argv[off]);
  }
  execvp_sysCx (&argv[off]);
  return 1;
}

#ifndef LACE_BUILTIN_LIBRARY
  int
main(int argc, char** argv)
{
  int argi = init_sysCx(&argc, &argv);
  int istat = main_execfd(argc-(argi-1), &argv[argi-1]);
  return istat;
}
#endif
