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
NEXT_FUZZ_DATA
{
  0x05, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x03, 0x03,
  0x00, 0x00, 0xff, 0x1c, 0xfd, 0xf7, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0x1b, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0xfd, 0x02, 0x2e, 0x06, 0x28, 0x00, 0x00, 0xff, 0xff, 0x2d, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x08, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x2f, 0x11, 0x01, 0x11,
  0x10, 0xf6, 0x00, 0x20, 0x71, 0x00, 0x00, 0x11, 0x00, 0xfd, 0xff, 0x04,
  0x00, 0x00, 0x00, 0x32, 0x01, 0x0a, 0x00, 0x00, 0x24, 0x03, 0x00, 0x05,
  0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x27, 0xff, 0x00, 0x00, 0x11, 0x11,
  0x11, 0x00, 0x00, 0x00, 0x00, 0x83, 0x00, 0x00, 0xd6, 0x00, 0x11, 0xff,
  0xff, 0x05, 0x03, 0x03, 0x00, 0xfd, 0xff, 0xff, 0xff, 0xff, 0x1b, 0xff,
  0x03, 0x00, 0x00, 0x06, 0xff, 0x1c, 0xfd, 0xf7, 0xff, 0xff, 0x06, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0x00, 0xff, 0x03, 0x03, 0x00,
  0x00, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0x1b, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xfd,
  0x02, 0x2e, 0x06, 0x28, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x09, 0x00,
  0x5e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xd6, 0x00, 0x11,
  0xff, 0xff, 0x05, 0x03, 0x03, 0x00, 0xfd, 0xff, 0xff, 0xff, 0xff, 0x1b,
  0xff, 0x03, 0x00, 0x00, 0x06, 0xff, 0x1c, 0xfd, 0xf7, 0xff, 0xff, 0x06,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0x00, 0xff, 0x03, 0x03,
  0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0x1b, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0xfd, 0x02, 0x2e, 0x06, 0x28, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x09,
  0x00, 0x5e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0xb3, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00,
  0x00,
}
NEXT_FUZZ_DATA
{ 5,
  1, 0,0,
  0x42, 0x26, 0x24, 0,0,
  30, 0,0,
  0x06, 0x83, 0xfe, 0xff, 0xff, 0,0,
  0xff, 0xfe, 0xff, 0xff, 0x07, 0x26, 0x8d, 0x8d, 0x8d, 0x8d, 0x8d, 0x8d, 0x05, 0xfe, 0xff, 0xff, 0xff, 0xff, 0xf7, 0xff, 0xff, 0xf7, 0,0,
  15, 0,0,
  24, 0,0,
  17, 0,0,
  12, 0,0,
  0x38, 0x38, 0,0,
  5, 0,0,
  20, 0,0,
  0x05, 0x9a, 0,0,
  0x93, 0x93, 0x93, 0x93, 0x93, 0x93, 0x93, 0x93, 0x93, 0x93, 0x93, 0x93, 0x93, 0x93, 0x93, 0x93, 0x93, 0x93, 0x93, 0x93, 0x93, 0x93, 0x93, 0x93, 0,0,
  0x33, 0x07, 0xcc, 0x33, 0,0,
  0x05, 0x03, 0x9a, 0x5e, 0x5e, 0x5e, 0,0,
  0x05, 0x9a, 0,0,
  21, 0,0,
  0x0a, 0x38, 0x38, 0x38, 0x38, 0x38, 0x38, 0x38, 0x38, 0,0,
  2, 0,0,
  4, 0,0,
  29, 0,0,
  11, 0,0,
  16, 0,0,
  9, 0,0,
  0xff, 0xff, 0xff, 0xff, 0x5e, 0,0,
  7, 0,0,
  0x7b, 0xff, 0,0,
  0xe1, 0x99, 0x05, 0,0,
  0x44, 0x33, 0xcc, 0x33, 0,0,
  22, 0,0,
  0x80, 0x05, 0x4b, 0x4b, 0x4b, 0x4b, 0x4b, 0x4a, 0x4b, 0x4b, 0xff, 0x05, 0,0,
  23, 0,0,
  0x31, 0xff, 0xf2, 0,0,
  0x24, 0xff, 0,0,
  0xa3, 0xff, 0,0,
  3, 0,0,
  26, 0,0,
  0xff, 0x04, 0,0,
  28, 0,0,
  0x8f, 0x8f, 0,0,
  0x5e, 0x5e, 0x5e, 0,0,
  14, 0,0,
  19, 0,0,
  0x88, 0x05, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x5e, 0,0,
  13, 0,0,
  27, 0,0,
  0x80, 0x05, 0x05, 0xfc, 0,0,
  0x4b, 0x4b, 0x4b, 0x4b, 0,0,
  0x8d, 0x8d, 0x8d, 0x8d, 0x4b, 0x05, 0,0,
  0xff, 0x2d, 0xf9, 0xff, 0xfe, 0xff, 0xf7, 0xb0, 0,0,
  0x42, 0x26, 0,0,
  0x06, 0xf9, 0,0,
  0x7c, 0xde, 0xff, 0xff, 0xff, 0xfc, 0xff, 0x40, 0,0,
  0x26, 0x93, 0x93, 0x3b, 0x20, 0x53, 0x93, 0x93, 0,0,
  0xe2, 0xe2, 0xff, 0x60, 0x07, 0x08, 0x08, 0xde, 0x05, 0x7b, 0,0,
  0x42, 0x26, 0xff, 0,0,
  0x01, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x8d, 0x8d, 0x7e, 0x8d, 0x8d, 0x8d, 0x8d, 0,0,
  0xe2, 0xe2, 0xff, 0x60, 0x07, 0x08, 0x08, 0xde, 0,0,
  0x86, 0xff, 0,0,
  0x8d, 0x4b, 0x05, 0x26, 0x4c, 0,0,
  0x93, 0x93, 0x93, 0x93, 0x93, 0x93, 0x93, 0x93, 0x93, 0x93, 0x93, 0x93, 0x93, 0x93, 0x93, 0x93, 0x93, 0x93, 0,0,
  0x5d, 0x5d, 0x5d, 0x5d, 0x5d, 0x5d, 0x5d, 0x5d, 0x5d, 0x5d, 0x5d, 0x5d, 0x5d, 0x5d, 0x5d, 0x5d, 0,0,
  18, 0,0,
  10, 0,0,
  0x93, 0x93, 0x93, 0x93, 0x93, 0x93, 0x93, 0x93, 0x93, 0x93, 0x93, 0x93, 0x93, 0x93, 0x93, 0,0,
  0x93, 0x05, 0,0,
  0x3b, 0x20, 0x53, 0xff, 0x10, 0,0,
  0x93, 0x93, 0,0,
  0x05, 0x3b, 0x20, 0x53, 0xff, 0,0,
  0x93, 0x93, 0x93, 0x93, 0x93, 0x93, 0x93, 0x93, 0x93, 0x93, 0,0,
  25, 0,0,
  6, 0,0,
  0x86, 0xff, 0xa9, 0,0,
  8, 0,0,
  0x86, 0xff, 0,0,
  0x0d, 0x0a, 0,0,
  0x03, 0x9a, 0xd9, 0xd9, 0,0,
  0xcc, 0x05, 0x9a, 0,0,
  0x38, 0x38, 0,0,
  0x93, 0x93, 0x93, 0x93, 0x93, 0x93, 0x93, 0x93, 0x93, 0x93, 0x93, 0x93, 0x93, 0x93, 0x93, 0x93, 0x93, 0x93, 0x93, 0x93, 0x93, 0x93, 0x93, 0x93, 0x93, 0,0,
  0x40, 0x05, 0,0,
  0x4b, 0x05, 0,0,
}
END_FUZZ_DATA
