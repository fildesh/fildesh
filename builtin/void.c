/** Simple utility to gobble a stream.**/
#include "fildesh.h"

  int
lace_builtin_void_main(unsigned argc, char** argv,
                       FildeshX** inputs, FildeshO** outputs)
{
  FildeshX* in = open_arg_FildeshXF(0, argv, inputs);
  (void) argc;
  if (!in) {return 1;}

  while (0 < read_FildeshX(in)) {
    in->size = 0;
    flush_FildeshX(in);
  }
  close_FildeshX(in);

  if (outputs && outputs[0]) {
    close_FildeshO(open_arg_FildeshOF(0, argv, outputs));
  }
  return 0;
}

#ifndef LACE_BUILTIN_LIBRARY
int main(int argc, char** argv) {
  return lace_builtin_void_main(argc, argv, NULL, NULL);
}
#endif
