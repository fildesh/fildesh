#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size);
#else
extern int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size);
#endif
