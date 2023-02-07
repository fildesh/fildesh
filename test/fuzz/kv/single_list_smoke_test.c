#include "test/fuzz/smoke_common.h"

#define NULL_VALUE 255

BEGIN_FUZZ_DATA
/* Map 1->10, remove 1, then map 1->20.*/
{ 1,
  1,0,
  1,
}
NEXT_FUZZ_DATA
/* Map 1->10, 2->20, 3->30, then remove 2->20.*/
{ 1,
  2,
  3,
  2,0,
}
NEXT_FUZZ_DATA
/* Remove first element in SINGLE_LIST map.*/
{ 2,
  2,0,
}
NEXT_FUZZ_DATA
/* Remove second element in SINGLE_LIST map.*/
{ 4,
  5,
  2,
  5,0,
  4,0,
}
NEXT_FUZZ_DATA
/* Remove third element in SINGLE_LIST map.*/
{ 6,131,
  7,132,
  4,121,
  5,122,
  2,111,
  7,NULL_VALUE,
  6,NULL_VALUE,
}
NEXT_FUZZ_DATA
/* Remove fourth element in SINGLE_LIST map.*/
{ 8,
  9,
  6,
  7,
  4,
  5,
  2,
  9,0,
  8,0,
}
END_FUZZ_DATA
