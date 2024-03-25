/** Simple utility to gobble a stream.**/
#include <fildesh/fildesh.h>

  int
fildesh_builtin_void_main(
    unsigned argc, char** argv,
    FildeshX** inputv, FildeshO** outputv)
{
  FildeshX* in = open_arg_FildeshXF(0, argv, inputv);
  while (in && 0 < read_FildeshX(in)) {
    in->size = 0;
    flush_FildeshX(in);
  }
  close_FildeshX(in);
  if (outputv && outputv[0]) {
    close_FildeshO(open_arg_FildeshOF(0, argv, outputv));
  }
  return (argc == 1 ? 0 : 64);
}

#if !defined(FILDESH_BUILTIN_LIBRARY) && !defined(UNIT_TESTING)
int main(int argc, char** argv) {
  return fildesh_builtin_void_main(argc, argv, NULL, NULL);
}
#endif
