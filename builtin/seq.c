/** Simple version of `seq`.**/
#include "lace.h"
#include <limits.h>

  int
lace_builtin_seq_main(unsigned argc, char** argv,
                      LaceX** inputs, LaceO** outputs)
{
  LaceO* out = open_arg_LaceOF(0, argv, outputs);
  int beg_int = 0;
  int end_int = 0;
  int i;
  (void) inputs;
  if (!out) {return 1;}
  if (argc != 3) {return 64;}

  if (!fildesh_parse_int(&beg_int, argv[1])) {return 64;}
  if (!fildesh_parse_int(&end_int, argv[2])) {return 64;}

  for (i = beg_int; i < end_int; ++i) {
    print_int_LaceO(out, i);
    putc_LaceO(out, '\n');
    flush_LaceO(out);
  }
  if (i == end_int) {
    print_int_LaceO(out, i);
    putc_LaceO(out, '\n');
  }
  close_LaceO(out);
  return 0;
}

#ifndef LACE_BUILTIN_LIBRARY
int main(int argc, char** argv) {
  return lace_builtin_seq_main(argc, argv, NULL, NULL);
}
#endif
