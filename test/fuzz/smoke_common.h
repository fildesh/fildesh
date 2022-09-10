#include "fuzz_common.h"
#include <assert.h>

typedef struct FildeshBytestring FildeshBytestring;
struct FildeshBytestring {
  const uint8_t* at;
  size_t size;
};

#define BEGIN_FUZZ_DATA_CASE \
  { \
    static const uint8_t data[] =

#define END_FUZZ_DATA_CASE \
    ; \
    istat = LLVMFuzzerTestOneInput(data, sizeof(data)); \
    assert(istat == 0); \
  }

#define BEGIN_FUZZ_DATA \
  int main() { \
    int istat; \
    BEGIN_FUZZ_DATA_CASE

#define NEXT_FUZZ_DATA \
    END_FUZZ_DATA_CASE \
    BEGIN_FUZZ_DATA_CASE

#define END_FUZZ_DATA \
    END_FUZZ_DATA_CASE \
    return 0; \
  }
