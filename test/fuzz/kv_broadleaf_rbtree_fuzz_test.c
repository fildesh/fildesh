#include "kv_fuzz_common.h"
#include "test/lib/kv_rbtree_validation.h"

  int
LLVMFuzzerTestOneInput(const uint8_t data[], size_t size) {
  size_t i;
  FildeshKV map[1] = {DEFAULT_FildeshKV_BROADLEAF_RBTREE};
  int istat = kv_fuzz_common(map, data, size);
  if (false)
  {
    FildeshO* out = open_FildeshOF("/dev/stderr");
    print_debug_FildeshKV_RBTREE(map, out);
    putc_FildeshO(out, '\n');
    close_FildeshO(out);
    validate_FildeshKV_RBTREE(map);
  }
  for (i = 1; i < size; i+=2) {
    if (data[i] == 0xff) {break;}
    if (i == size-1) {
      validate_growing_FildeshKV_BROADLEAF_RBTREE(map);
    }
  }
  close_FildeshKV(map);
  return istat;
}
