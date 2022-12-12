
#include "fildesh_builtin.h"
#include "include/fildesh/fildesh_compat_sh.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

  int
main_waitdo(unsigned argc, char** argv)
{
  const char* ExeName = argv[0];
  FILE* in = stdin;
  unsigned argi = 1;
  int exstatus = 0;

  if (exstatus == 0) {
    if (argi >= argc) {
      fildesh_log_error("Need a command!");
      exstatus = 64;
    }
  }

  if (exstatus == 0) {
    if (0 == strcmp(argv[argi], "-x")) {
      ++ argi;
      if (argi < argc) {
        in = fopen(argv[argi], "rb");
      }
    }
    if (!in) {
      fildesh_log_errorf("No input?");
      exstatus = 66;
    }
  }

  if (exstatus == 0) {
    if (argi < argc && 0 == strcmp(argv[argi], "--")) {
      ++ argi;
    }
    if (argi >= argc) {
      fildesh_log_errorf("Need a command!");
      exstatus = 64;
    }
  }

  if (exstatus == 0)
  {
    while (! feof (in) && ! ferror (in))  fgetc (in);
    fclose (in);

    if (fildesh_specific_util(argv[argi]))
    {
      return fildesh_builtin_main(argv[argi], argc-argi, &argv[argi]);
    }
    fildesh_compat_sh_exec((const char**)&argv[argi]);

    fildesh_log_errorf("%s - Failed to execute:%s\n", ExeName, argv[2]);
    exstatus = 126;
  }

  fildesh_log_errorf("Usage: %s [-x IN] [--] COMMAND [ARG...]\n", ExeName);
  return exstatus;
}

#ifndef FILDESH_BUILTIN_LIBRARY
  int
main(int argc, char** argv)
{
  return main_waitdo((unsigned)argc, argv);
}
#endif
