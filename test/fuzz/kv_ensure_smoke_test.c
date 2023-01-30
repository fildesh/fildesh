#include "smoke_common.h"

BEGIN_FUZZ_DATA
/* A zero-length key with a 1-length value.*/
{ 1, 0, 90, 0,
}
NEXT_FUZZ_DATA
/* Just a key. Test needs to fill in a NUL delimiter.*/
{ 1, 90, 90,
}
NEXT_FUZZ_DATA
/* Replace splitvalue with pointer (when sizeof(void*) <= 8).*/
{ 1,
  1,0, 1,0,
  2,0, 1,0,
  2,0, 1,2,3,4,5,6,7,8,9,0,
}
NEXT_FUZZ_DATA
/* Use a pointer for the splitkey (when sizeof(void*) <= 8).*/
{ 1,
  11,0, 1,0,
  1,2,3,4,5,6,7,8,9,0, 1,0,
}
NEXT_FUZZ_DATA
{ 5,
  4,0, 4,0,
  2,0, 2,0,
  1,0, 1,0,
  5,0, 5,0,
  3,0, 3,0,
}
NEXT_FUZZ_DATA
{ 0x5,
  0x1,0x0,0x0,
  0x2e,0x0,0x0,
  0x26,0x0,0x0,
  0x0,0x0,
  0x4,0x0,0x6,0x0,
  0x0,
}
NEXT_FUZZ_DATA
{ 5,
  6,0, 6,0,
  4,0, 4,0,
  1,0, 1,0,
  5,0, 5,0,
  2,0, 2,0,
  3,0, 3,0,
}
NEXT_FUZZ_DATA
/* Instead of this. It should be this.
 *        6#             6#
 *        / \            / \
 *      4+   #7        3+   #7
 *      / \            / \
 *    3#   #5        1#2 4#5
 *    /
 *  1+2
 */
{ 5,
  6,0, 6,0,
  2,0, 2,0,
  7,0, 7,0,
  5,0, 5,0,
  4,0, 4,0,
  3,0, 3,0,
  1,0, 1,0,
}
NEXT_FUZZ_DATA
{ 5,

  1,0,
  1,0,

  2,2,2,2,2,2,2,2,2,2,0,
  2,0,

  3,3,3,3,3,3,3,0,
  3,0,

  4,4,4,4,4,4,4,0,
  4,4,4,4,4,4,4,4,4,0,

  5,5,5,5,5,5,5,5,5,0,
  5,5,5,5,5,5,5,5,5,0,

  5,5,5,5,5,5,5,5,5,0,
  0,

  7,7,0,
  7,0,
}
END_FUZZ_DATA
