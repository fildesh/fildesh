#include <stddef.h>
#include <stdint.h>

#include <fildesh/fildesh.h>

#ifdef __cplusplus
extern "C"
#endif
int kv_fuzz_common(FildeshKV* map, const uint8_t data[], size_t size);
