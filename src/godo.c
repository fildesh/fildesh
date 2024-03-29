
#include <fildesh/fildesh.h>
#include "include/fildesh/fildesh_compat_errno.h"
#include "include/fildesh/fildesh_compat_fd.h"
#include "include/fildesh/fildesh_compat_sh.h"

#include <stdio.h>
#include <stdlib.h>

  int
main_godo(unsigned argc, char** argv)
{
  const char* directory = argv[1];
  if (argc < 3) {
    fildesh_log_warning("Usage: godo PATH COMMAND [ARG...]");
    return 64;
  }

  if (0 != fildesh_compat_sh_chdir(directory)) {
    fildesh_log_errorf("Failed to chdir() to: %s", directory);
    return 66;
  }

  argv = &argv[2];
#ifdef FILDESH_BUILTIN_LIBRARY
  {
    int istat = fildesh_compat_fd_spawnvp_wait(
        0, 1, 2, NULL, (const char**)argv);
    if (istat >= 0) {return istat;}
  }
#else
  fildesh_compat_sh_exec((const char**)argv);
#endif
  /* Flow should not actually get here. */
  return 126;
}

#ifndef FILDESH_BUILTIN_LIBRARY
  int
main(int argc, char** argv)
{
  return main_godo(argc, argv);
}
#endif
