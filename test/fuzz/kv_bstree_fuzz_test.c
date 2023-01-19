#include "kv_fuzz_common.h"

  int
LLVMFuzzerTestOneInput(const uint8_t data[], size_t size) {
  FildeshKV map[1] = {DEFAULT_FildeshKV_BSTREE};
  int istat = kv_fuzz_common(map, data, size);
  close_FildeshKV(map);
  return istat;
}
