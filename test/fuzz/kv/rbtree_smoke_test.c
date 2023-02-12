#include "test/fuzz/smoke_common.h"

#define NULL_VALUE 255

BEGIN_FUZZ_DATA
/* Add 2, then add 1.*/
{ 2, 1,
}
NEXT_FUZZ_DATA
/* Add 3, 1, 2.*/
{ 3, 1, 2,
}
NEXT_FUZZ_DATA
/* Add 1, 2, 3.*/
{ 1, 2, 3,
}
NEXT_FUZZ_DATA
/* Add 1, then remove it.*/
{ 1, 1,0,
}
NEXT_FUZZ_DATA
/* Remove root of balanced tree of size 3, then add another value.*/
{ 2, 1, 3,
  2,0,
  4,
}
NEXT_FUZZ_DATA
{ 3, 2, 1, 4,
  4,0,
}
END_FUZZ_DATA
