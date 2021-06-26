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

  if (argv[1]) {
    lhs = open_LaceXF(argv[1]);
  } else if (inputs && inputs[1]) {
    lhs = inputs[1];
  }
  if (argv[2]) {
    rhs = open_LaceXF(argv[2]);
  } else if (inputs && inputs[2]) {
    rhs = inputs[2];
  }

  if (!lhs || !rhs) {
    close_LaceX(lhs);
    close_LaceX(rhs);
    return 64;
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
  int
main(int argc, char** argv)
{
  return lace_builtin_cmp_main(argc, argv, NULL, NULL);
}
#endif
