#include "smoke_common.h"

BEGIN_FUZZ_DATA
/* A zero-length key with a 1-length value.*/
{ 0, 90, 0,
}
NEXT_FUZZ_DATA
/* Just a key. Test needs to fill in a NUL delimiter.*/
{ 90, 90,
}
END_FUZZ_DATA
