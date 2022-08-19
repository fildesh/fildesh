#include "fuzz_common.h"

/* Needed for rules_fuzzing + GCC compilation.
 * We have removed rules_fuzzing, but this is fine to keep.
 */
int LLVMFuzzerInitialize(int* argc, char*** argv) {
  (void)argc;
  (void)argv;
  return 0;
}
