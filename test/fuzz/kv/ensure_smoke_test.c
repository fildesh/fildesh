#include "test/fuzz/smoke_common.h"

BEGIN_FUZZ_DATA
/* A zero-length key with a 1-length value.*/
{ 1, 0, 90, 0,
}
NEXT_FUZZ_DATA
/* Just a key. Test needs to fill in a NUL delimiter.*/
{ 1, 90, 90,
}
NEXT_FUZZ_DATA
/* Replace value with pointer (when sizeof(void*) <= 8).*/
{ 1,
  1,0, 1,0,
  2,0, 1,0,
  2,0, 1,2,3,4,5,6,7,8,9,0,
}
NEXT_FUZZ_DATA
/* Use a pointer for the key (when sizeof(void*) <= 8).*/
{ 1,
  11,0, 1,0,
  1,2,3,4,5,6,7,8,9,0, 1,0,
}
END_FUZZ_DATA
