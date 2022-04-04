#include "fuzz_common.h"

/* GCC needs this defined.*/
int LLVMFuzzerInitialize(int* argc, char*** argv) {
  (void)argc;
  (void)argv;
  return 0;
}
