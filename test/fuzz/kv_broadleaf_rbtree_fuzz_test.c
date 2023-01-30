#include "kv_fuzz_common.h"
#include "test/lib/kv_rbtree_validation.h"

  int
LLVMFuzzerTestOneInput(const uint8_t data[], size_t size) {
  FildeshKV map[1] = {DEFAULT_FildeshKV_BROADLEAF_RBTREE};
  int istat = kv_fuzz_common(map, data, size);
  validate_FildeshKV_RBTREE(map);
  close_FildeshKV(map);
  return istat;
}
