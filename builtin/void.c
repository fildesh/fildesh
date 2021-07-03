/** Simple utility to gobble a stream.**/
#include "lace.h"

  int
lace_builtin_void_main(unsigned argc, char** argv,
                       LaceX** inputs, LaceO** outputs)
{
  LaceX* in = open_arg_LaceXF(0, argv, inputs);
  (void) argc;
  (void) outputs;
  if (!in) {return 1;}

  while (0 < read_LaceX(in)) {
    in->size = 0;
    flush_LaceX(in);
  }
  close_LaceX(in);
  return 0;
}

#ifndef LACE_BUILTIN_LIBRARY
int main(int argc, char** argv) {
  return lace_builtin_void_main(argc, argv, NULL, NULL);
}
#endif
