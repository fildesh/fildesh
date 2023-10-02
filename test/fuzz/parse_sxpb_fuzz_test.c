#include "fuzz_common.h"

#include <string.h>

#include <fildesh/sxproto.h>

  int
LLVMFuzzerTestOneInput(const uint8_t data[], size_t size) {
  FildeshO* err_out = open_FildeshOF("/dev/null");
  FildeshO* out = open_FildeshOF("/dev/null");

  if (size > 0 && data[size-1] == 0) {
    /* Trim a NUL byte if it exists.*/
    size -= 1;
  }

  {
    FildeshX in = memref_FildeshX(data, size);
    FildeshSxpb* sxpb = slurp_sxpb_close_FildeshX(&in, NULL, err_out);
    if (sxpb) {
      print_txtpb_FildeshO(out, sxpb);
      close_FildeshSxpb(sxpb);
    }
  }

  close_FildeshO(out);
  close_FildeshO(err_out);
  return 0;
}

