#include "test/fuzz/smoke_common.h"

BEGIN_FUZZ_DATA
/* Add 1, remove 1, then change 1's value.*/
{ 1, 1,0, 1,
}
NEXT_FUZZ_DATA
/* Add 1, then change 1's value.*/
{ 1, 1,
}
NEXT_FUZZ_DATA
/* Add 1, 2, 3, then change 1's value.*/
{ 1, 2, 3, 1,
}
NEXT_FUZZ_DATA
/* Add 1, 2, 3, then change 2's value.*/
{ 1, 2, 3, 2,
}
NEXT_FUZZ_DATA
/* Add 1, 2, 3, then remove 2.*/
{ 1, 2, 3, 2,0,
}
NEXT_FUZZ_DATA
/* Remove first element in SINGLE_LIST map.*/
{ 2, 2,0,
}
NEXT_FUZZ_DATA
/* Remove second element in SINGLE_LIST map.*/
{ 4, 5, 2, 5,0, 4,0,
}
NEXT_FUZZ_DATA
/* Remove third element in SINGLE_LIST map.*/
{ 6, 7,
  4, 5,
  2,
  7,0,
  6,0,
}
NEXT_FUZZ_DATA
/* Remove fourth element in SINGLE_LIST map.*/
{ 8, 9,
  6, 7,
  4, 5,
  2,
  9,0,
  8,0,
}
NEXT_FUZZ_DATA
/* Add element that can't be stored in the same node.*/
{ 1, 201,
}
NEXT_FUZZ_DATA
/* Add element that can't be stored in the same node.*/
{ 201, 1,
}
END_FUZZ_DATA
