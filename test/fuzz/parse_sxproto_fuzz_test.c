#include "fuzz_common.h"

#include <string.h>

#include <fildesh/sxproto.h>

  int
LLVMFuzzerTestOneInput(const uint8_t data[], size_t size) {
  FildeshO* err_out = open_FildeshOF("/dev/null");
  FildeshO* out = open_FildeshOF("/dev/null");
  FildeshX in[1] = {DEFAULT_FildeshX};

  memcpy(grow_FildeshX(in, size), data, size);
  sxproto2textproto(in, out, err_out);

  close_FildeshO(out);
  close_FildeshO(err_out);
  return 0;
}

