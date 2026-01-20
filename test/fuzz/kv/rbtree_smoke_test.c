#include "test/fuzz/smoke_common.h"

#define NULL_VALUE 255

BEGIN_FUZZ_DATA
/* Map 1->10, remove 1, then map 2->20.*/
{ 2, 20,
  1, 10,
}
NEXT_FUZZ_DATA
/* Map 3->30, 1->10, 2->20.*/
{ 3, 30,
  1, 10,
  2, 20,
}
NEXT_FUZZ_DATA
/* Map 1->10, 2->20, 3->30.*/
{ 1, 10,
  2, 20,
  3, 30,
}
NEXT_FUZZ_DATA
/* Map 1->10, then remove 1->10.*/
{ 1, 10,
  1, NULL_VALUE,
}
NEXT_FUZZ_DATA
/* Remove root of balanced tree of size 3, then add another value.*/
{ 2, 20,
  1, 10,
  3, 30,
  2, NULL_VALUE,
  4, 40,
}
NEXT_FUZZ_DATA
{ 3, 30,
  2, 20,
  1, 10,
  4, 40,
  4, NULL_VALUE,
}
END_FUZZ_DATA
