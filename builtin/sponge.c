/**
 * Read then write.
 **/

#include "lace.h"
#include <assert.h>
#include <string.h>

  int
lace_builtin_sponge_main(unsigned argc, char** argv,
                         LaceX** inputv, LaceO** outputv)
{
  LaceX* in = NULL;
  char* data = NULL;
  LaceO* out = NULL;
  int exstatus = 0;

  if (argc > 2) {
    lace_log_errorf("At most one argument expected.");
    return 64;
  }

  in = open_arg_LaceXF(0, argv, inputv);
  if (!in) {
    lace_log_errorf("Cannot open stdin");
    exstatus = 66;
  }

  if (exstatus == 0) {
    data = slurp_LaceX(in);
  }

  if (data) {
    if (argc == 2) {
      out = open_arg_LaceOF(1, argv, outputv);
      if (!out) {
        lace_log_errorf("Cannot open output file: %s", argv[1]);
        exstatus = 70;
      }
    } else {
      out = open_arg_LaceOF(0, argv, outputv);
      if (!out) {
        lace_log_errorf("Cannot open stdout");
        exstatus = 70;
      }
    }
  }
  if (exstatus == 0) {
    memcpy(grow_LaceO(out, in->size),
           in->at,
           in->size);
  }

  close_LaceX(in);
  close_LaceO(out);
  return exstatus;
}

#ifndef LACE_BUILTIN_LIBRARY
int main(int argc, char** argv) {
  return lace_builtin_sponge_main((unsigned)argc, argv, NULL, NULL);
}
#endif
