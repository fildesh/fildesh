#include "smoke_common.h"

#define NULL_VALUE 255

BEGIN_FUZZ_DATA
{ 1, 10,
  2, 20,
  4, 40,
  3, 30,
}
NEXT_FUZZ_DATA
{ 3, 30,
  4, 40,
  1, 10,
  2, 20,
  4, NULL_VALUE,
}
NEXT_FUZZ_DATA
{ 8, 8,
  3, 3,
  8, NULL_VALUE,
  8, 8,
  6, 6,
  7, NULL_VALUE,
  5, 5,
  2, 2,
  1, 1,
  4, 4,
  1, NULL_VALUE,
  8, NULL_VALUE,
}
NEXT_FUZZ_DATA
{ 2, 20,
  4, 40,
  3, 30,
  1, 10,
  4, NULL_VALUE,
}
NEXT_FUZZ_DATA
{ 1, 10,
  5, 50,
  6, 60,
  2, 20,
  4, 40,
  3, 30,
  6, NULL_VALUE,
}
NEXT_FUZZ_DATA
{ 2, 20,
  6, 60,
  7, 70,
  5, 50,
  3, 30,
  1, 10,
  4, 40,
  7, NULL_VALUE,
}
NEXT_FUZZ_DATA
{ 3,3,
  5,5,
  2,2,
  6,6,
  1,1,
  3,NULL_VALUE,
  3,3,
  4,4,
  1,NULL_VALUE,
}
NEXT_FUZZ_DATA
{ 4,4,
  3,3,
  5,5,
  1,1,
  2,2,
  5,NULL_VALUE,
}
NEXT_FUZZ_DATA
{ 1, 1,
  9, 9,
  2, 2,
  6, 6,
  6, NULL_VALUE,
  4, 4,
  8, 8,
  5, 5,
  6, 6,
  7, 7,
  3, 3,
  6, NULL_VALUE,
}
END_FUZZ_DATA
