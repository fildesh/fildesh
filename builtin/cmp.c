/**
 * Compare two files.
 **/

#include "lace.h"

  int
lace_builtin_cmp_main(int argc, char** argv,
                      LaceX** inputs, LaceO** outputs)
{
  LaceX* lhs = NULL;
  LaceX* rhs = NULL;
  bool equal = true;
  if (outputs || argc != 3) {
    return 64;
  }

  lhs = open_arg_LaceXF(1, argv, inputs);
  rhs = open_arg_LaceXF(2, argv, inputs);
  if (!lhs || !rhs) {
    close_LaceX(lhs);
    close_LaceX(rhs);
    return 66;
  }

  read_LaceX(lhs);
  read_LaceX(rhs);
  while (lhs->off < lhs->size && rhs->off < rhs->size) {
    if (lhs->at[lhs->off++] != rhs->at[rhs->off++]) {
      equal = false;
      break;
    }
    if (lhs->off == lhs->size) {
      flush_LaceX(lhs);
      read_LaceX(lhs);
    }
    if (rhs->off == rhs->size) {
      flush_LaceX(rhs);
      read_LaceX(rhs);
    }
  }

  equal = equal && (lhs->off == lhs->size && rhs->off == rhs->size);

  close_LaceX(lhs);
  close_LaceX(rhs);
  return (equal ? 0 : 1);
}

#ifndef LACE_BUILTIN_LIBRARY
int main(int argc, char** argv) {
  return lace_builtin_cmp_main(argc, argv, NULL, NULL);
}
#endif
