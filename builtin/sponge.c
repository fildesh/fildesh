/**
 * Read then write.
 **/

#include "fildesh.h"
#include <assert.h>
#include <string.h>

  int
lace_builtin_sponge_main(unsigned argc, char** argv,
                         FildeshX** inputv, FildeshO** outputv)
{
  FildeshX* in = NULL;
  char* data = NULL;
  FildeshO* out = NULL;
  int exstatus = 0;

  if (argc > 2) {
    fildesh_log_errorf("At most one argument expected.");
    return 64;
  }

  in = open_arg_FildeshXF(0, argv, inputv);
  if (!in) {
    fildesh_log_errorf("Cannot open stdin");
    exstatus = 66;
  }

  if (exstatus == 0) {
    data = slurp_FildeshX(in);
  }

  if (data) {
    if (argc == 2) {
      out = open_arg_FildeshOF(1, argv, outputv);
      if (!out) {
        fildesh_log_errorf("Cannot open output file: %s", argv[1]);
        exstatus = 70;
      }
    } else {
      out = open_arg_FildeshOF(0, argv, outputv);
      if (!out) {
        fildesh_log_errorf("Cannot open stdout");
        exstatus = 70;
      }
    }
  }
  if (exstatus == 0) {
    memcpy(grow_FildeshO(out, in->size),
           in->at,
           in->size);
  }

  close_FildeshX(in);
  close_FildeshO(out);
  return exstatus;
}

#ifndef LACE_BUILTIN_LIBRARY
int main(int argc, char** argv) {
  return lace_builtin_sponge_main((unsigned)argc, argv, NULL, NULL);
}
#endif
