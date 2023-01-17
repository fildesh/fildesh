#include "smoke_common.h"

#define NULL_VALUE 255

BEGIN_FUZZ_DATA
/* Map 1->10, remove 1, then map 1->20.*/
{ 1, 10,
  1, NULL_VALUE,
  1, 20,
}
NEXT_FUZZ_DATA
/* Map 1->10, 2->20, 3->30, then remove 2->20.*/
{ 1, 10,
  2, 20,
  3, 30,
  2, NULL_VALUE,
}
NEXT_FUZZ_DATA
/* Remove first element in SINGLE_LIST map.*/
{ 11,111,
  11,NULL_VALUE,
}
NEXT_FUZZ_DATA
/* Remove second element in SINGLE_LIST map.*/
{ 21,121,
  22,122,
  11,111,
  22,NULL_VALUE,
  21,NULL_VALUE,
}
NEXT_FUZZ_DATA
/* Remove third element in SINGLE_LIST map.*/
{ 31,131,
  32,132,
  21,121,
  22,122,
  11,111,
  32,NULL_VALUE,
  31,NULL_VALUE,
}
NEXT_FUZZ_DATA
/* Remove fourth element in SINGLE_LIST map.*/
{ 41,141,
  42,142,
  31,131,
  32,132,
  21,121,
  22,122,
  11,111,
  42,NULL_VALUE,
  41,NULL_VALUE,
}
END_FUZZ_DATA
