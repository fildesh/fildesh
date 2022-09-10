/** A quick and dirty fuzz test for the sake of coverage.
 *
 * The libfuzzer-based fuzz tests are run manually.
 **/

#include <assert.h>
#include "fuzz_common.h"


#ifdef __cplusplus
extern "C" int LLVMFuzzerInitialize(int* argc, char*** argv);
#else
extern int LLVMFuzzerInitialize(int* argc, char*** argv);
#endif
/* Needed for rules_fuzzing + GCC compilation.
 * We have removed rules_fuzzing, so we keep the code here.
 */
int LLVMFuzzerInitialize(int* argc, char*** argv) {
  (void)argc;
  (void)argv;
  return 0;
}

#define MAX_FUZZ_BYTE_COUNT 127

int main(int argc, char** argv) {
  int istat;
  unsigned i, size;

  istat = LLVMFuzzerInitialize(&argc, &argv);
  assert(istat == 0);

  istat = LLVMFuzzerTestOneInput(NULL, 0);
  assert(istat == 0);

  for (i = 0; i < 256; ++i) {
    unsigned j;
    uint8_t data[2];
    data[0] = i;
    istat = LLVMFuzzerTestOneInput(data, 1);
    assert(istat == 0);
    for (j = i%17; j < 256; j+=17) {
      data[1] = j;
      istat = LLVMFuzzerTestOneInput(data, 2);
      assert(istat == 0);
    }
  }

  for (size = 3; size < MAX_FUZZ_BYTE_COUNT; ++size) {
    uint8_t data[MAX_FUZZ_BYTE_COUNT];
    for (i = 0; i < size; ++i) {
      data[i] = (uint8_t)((i+size) & 0xFF);
      assert(istat == 0);
    }
    istat = LLVMFuzzerTestOneInput(data, size);
    assert(istat == 0);
  }

  return 0;
}
