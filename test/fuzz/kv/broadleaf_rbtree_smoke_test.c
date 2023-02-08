#include "test/fuzz/smoke_common.h"

BEGIN_FUZZ_DATA
#if 1
{ 1,
  2,
  4,
  3,
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
{ 6, 2, 7, 5, 4, 3, 1 }
NEXT_FUZZ_DATA
/* Coverage in  kv_broadleaf.c  fixup_remove_case_0_FildeshKV_BROADLEAF_RBTREE():
 *  ...  if (RedColorOf(b)) {
 */
{ 2, 6, 3, 4, 1, 7, 5, 4,0, }
NEXT_FUZZ_DATA
/* Coverage in  kv_broadleaf.c  fixup_remove_case_1_FildeshKV_BROADLEAF_RBTREE():
 *  ... if (side == 0) {
 */
{ 2, 1, 8, 6, 5, 7, 5,0, 4, 3, 1,0,
}
NEXT_FUZZ_DATA
/* Coverage in  kv_broadleaf.c  fixup_remove_case_1_FildeshKV_BROADLEAF_RBTREE():
 *  ... if (side == 0) {...} else {
 */
{ 7, 8, 1, 3, 4, 2, 4,0, 5, 6, 8,0,
}
NEXT_FUZZ_DATA
/* Coverage in  kv_broadleaf.c  fixup_remove_case_1_FildeshKV_BROADLEAF_RBTREE():
 *  ... if (Nullish(JointOf(b))) {...} else {
 */
{ 2,
  1,
  8,
  9,
  5,
  7,
  3,
  7,0,
  4,
  6,
  9,0,
}
NEXT_FUZZ_DATA
/* Coverage in  kv_rbtree.c  fixup_insert():
 *  ... if (!Nullish(b)) {...} else {
 */
{ 2, 1, 202, 203, 7, 6, 4, 5, 3, 201, }
NEXT_FUZZ_DATA
{ 3,
  4,
  1,
  2,
  4,0,
}
NEXT_FUZZ_DATA
{ 8,
  3,
  8,0,
  8,
  6,
  7,0,
  5,
  2,
  1,
  4,
  1,0,
  8,0,
}
NEXT_FUZZ_DATA
{ 2,
  4,
  3,
  1,
  4,0,
}
NEXT_FUZZ_DATA
{ 1,
  5,
  6,
  2,
  4,
  3,
  6,0,
}
NEXT_FUZZ_DATA
{ 2,
  6,
  7,
  5,
  3,
  1,
  4,
  7,0,
}
NEXT_FUZZ_DATA
{ 3,
  5,
  2,
  6,
  1,
  3,0,
  3,
  4,
  1,0,
}
NEXT_FUZZ_DATA
{ 4,
  3,
  5,
  1,
  2,
  5,0,
}
NEXT_FUZZ_DATA
{ 1,
  9,
  2,
  6,
  6,0,
  4,
  8,
  5,
  6,
  7,
  3,
  6,0,
}
NEXT_FUZZ_DATA
{ 3,
  1,
  7,
  6,
  2,
  4,
  5,
  7,0,
}
NEXT_FUZZ_DATA
{ 1,
  5,
  6,
  2,
  3,
  4,
  7,
  8,
}
NEXT_FUZZ_DATA
{ 1,
  5,
  4,
  2,
  1,
  5,0,
  1,
  2,0,
}
NEXT_FUZZ_DATA
{ 4,
  2,
  1,
  5,
  3,
}
NEXT_FUZZ_DATA
{ 6,
  4,
  1,
  5,
  2,
  3,
}
NEXT_FUZZ_DATA
{0x10,0x30,0x8,0x35,0x22,0xff}
NEXT_FUZZ_DATA
/* Finds 2 leaves.*/
{ 12,
  14,
  13,
  15,
  3,
  2,
  11,
  8,
  1,
  9,
  10,
  4,
  6,
  7,
  5,
}
NEXT_FUZZ_DATA
{ 4,3,1,201,2,202,5,203,204,
}
NEXT_FUZZ_DATA
#endif
{ 3, 203, 1, 2, 202, 204, 201, 1,0, 4
}
END_FUZZ_DATA
