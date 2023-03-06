/** This program functions somewhat like `xargs -0 -x`.
 **/
#include "src/builtin/fildesh_builtin.h"

#include <stdlib.h>
#include <string.h>

#include "include/fildesh/fildesh_compat_sh.h"

  int
main_xargz(unsigned argc, char** argv)
{
  int exstatus = 0;
  unsigned argi = 1;
  FildeshX* in = NULL;
  FildeshAlloc* alloc = open_FildeshAlloc();
  DECLARE_FildeshAT(char*, spawn_argv);
  int (* main_fn)(unsigned, char**) = NULL;

  init_FildeshAT(spawn_argv);

  if (argv[argi] &&
      argv[argi][0] == '-' && argv[argi][1] == '-' && argv[argi][2] == '\0')
  {
    argi += 1;
  }
  if (exstatus == 0) {
    if (!argv[argi]) {
      exstatus = 64;
      fildesh_log_error("Must give a program name.");
    }
    else {
      main_fn = fildesh_builtin_main_fn_lookup(argv[argi]);
    }
  }
  for (; exstatus == 0 && argi < argc; ++argi) {
    push_FildeshAT(spawn_argv, argv[argi]);
  }

  in = open_FildeshXF("-");
  if (exstatus == 0 && in) {
    FildeshX slice;
    for (slice = slicechr_FildeshX(in, '\0');
         slice.at;
         slice = slicechr_FildeshX(in, '\0'))
    {
      char* s = (char*)reserve_FildeshAlloc(alloc, slice.size+1, 1);
      memcpy(s, slice.at, slice.size);
      s[slice.size] = '\0';
      push_FildeshAT(spawn_argv, s);
    }
  }
  close_FildeshX(in);

  if (exstatus == 0) {
    if (main_fn) {
      exstatus = main_fn((unsigned)count_of_FildeshAT(spawn_argv), *spawn_argv);
    }
    else {
      exstatus = fildesh_compat_sh_spawn((const char**)*spawn_argv);
    }
  }

  close_FildeshAT(spawn_argv);
  close_FildeshAlloc(alloc);
  return exstatus;
}
