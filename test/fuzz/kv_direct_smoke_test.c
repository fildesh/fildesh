#include "smoke_common.h"

#define NULL_VALUE 255

BEGIN_FUZZ_DATA
/* Map 1->10, remove 1, then map 1->20.*/
{ 1, 10,
  1, NULL_VALUE,
  1, 20,
}
NEXT_FUZZ_DATA
/* Map 1->10, 2->20, 3->30, then remove 2->10.*/
{ 1, 10,
  2, 20,
  3, 30,
  2, NULL_VALUE,
}
END_FUZZ_DATA
