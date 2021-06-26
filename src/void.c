/** Simple utility to gobble a stream.**/
#include "lace.h"

  int
main_void(int argi, int argc, char** argv)
{
  LaceX* in = open_LaceXF("-");
  LaceO* out = open_LaceOF("-");
  (void) argi;
  (void) argc;
  (void) argv;

  while (0 < read_LaceX(in)) {
    in->size = 0;
    flush_LaceX(in);
  }
  close_LaceX(in);
  close_LaceO(out);
  return 0;
}

#ifndef MAIN_LACE_EXECUTABLE
int main(int argc, char** argv) {
  return main_void(1, argc, argv);
}
#endif
