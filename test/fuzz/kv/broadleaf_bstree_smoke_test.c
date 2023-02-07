#include "test/fuzz/smoke_common.h"


BEGIN_FUZZ_DATA
/* Map 1->10, 2->20, then remove 2->20.*/
{ 1,
  2,
  2,0,
}
NEXT_FUZZ_DATA
/* Removal from a tree:
 *    3         3       2
 *   /   -->   /   -->
 * 1,2        2
 */
{ 1,
  2,
  3,
  1,0,
  3,0,
}
NEXT_FUZZ_DATA
/* Construct a tree:
 *     3
 *    / \
 *  1,2  5
 */
{
  2,
  1,
  3,
  5,
}
NEXT_FUZZ_DATA
{ 3,
  1,
  2,
  3,0,
}
NEXT_FUZZ_DATA
/* Coverage in kv_bstree.c premove_FildeshKV_BSTREE():
 *  ... b = SplitOf(y, 1); if (IsBroadLeaf(b)) {
 */
{ 2, 3, 1, 4, 1, 2,0,
}
NEXT_FUZZ_DATA
/* Coverage in bstree.c premove_FildeshKV_BSTREE():
 *  ... do { if (IsBroadLeaf(y)) {
 */
{ 4, 3, 6, 8, 10, 9, 1, 7, 5, 2, 6,0,
}
NEXT_FUZZ_DATA
/* Coverage in kv_broadleaf.c maybe_shifty_add():
 *  ... if (!IsBroadLeaf(y)) {
 */
{ 1, 2, 3, 4, 3,0, 5,
}
NEXT_FUZZ_DATA
/* Coverage in kv_broadleaf.c taking_add_FildeshKV_BSTREE():
 *  ... if (side == 0) {...} else {
 */
{ 202, 4, 3, 2, 203, 5, 1, 204, 201,
}
END_FUZZ_DATA
