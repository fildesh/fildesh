/** Fuzz test that adds elements to a string->string map.
 *
 * We allow the values to have zero-length, which is interpreted as no value!
 * This approach gives better coverage, but can only be used when testing the
 * `ensure` function, not the `replace` function.
 **/
#include "fuzz_common.h"
#include <assert.h>
#include <stdlib.h>

#include "fildesh.h"


  int
LLVMFuzzerTestOneInput(const uint8_t data[], size_t size) {
  size_t k_index = 0;
  size_t k_size = 0;
  size_t v_index = 0;
  size_t v_size = 0;
  FildeshKV map[1] = {DEFAULT_FildeshKV_SINGLE_LIST};
  size_t i;
  for (i = 0; i < size; ++i) {
    const uint8_t b = data[i];
    if (i+1 == size || (v_index > k_index && b == 0)) {
      ensure_v_FildeshKV(map, (const void*) &data[k_index], k_size,
                         (void*)&data[v_index], v_size);
      k_index = i+1;
      k_size = 0;
      v_index = 0;
      v_size = 0;
    }
    else if (b == 0) {
      v_index = k_index + k_size + 1;
    }
    else if (v_index > k_index) {
      v_size += 1;
    }
    else {
      k_size += 1;
    }
  }
  close_FildeshKV(map);
  return 0;
}

