#include "fuzz_common.h"

#include <fildesh/fildesh.h>

#include "src/lib/kv.h"

BEGIN_EXTERN_C

int kv_fuzz_common(FildeshKV* map, const uint8_t data[], size_t size);

END_EXTERN_C
