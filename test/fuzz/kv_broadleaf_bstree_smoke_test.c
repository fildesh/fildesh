#include "smoke_common.h"

#define NULL_VALUE 255

BEGIN_FUZZ_DATA
/* Map 1->10, 2->20, then remove 2->20.*/
{ 1, 10,
  2, 20,
  2, NULL_VALUE,
}
NEXT_FUZZ_DATA
/* Removal from a tree:
 *    3         3       2
 *   /   -->   /   -->
 * 1,2        2
 */
{ 1, 10,
  2, 20,
  3, 30,
  1, NULL_VALUE,
  3, NULL_VALUE,
}
NEXT_FUZZ_DATA
/* Construct a tree:
 *     3
 *    / \
 *  1,2  5
 */
{
  2, 20,
  1, 10,
  3, 30,
  5, 50,
}
NEXT_FUZZ_DATA
{ 3, 30,
  1, 10,
  2, 20,
  3, NULL_VALUE,
}
NEXT_FUZZ_DATA
/* Coverage in kv_bstree.c premove_FildeshKV_BSTREE():
 *  ... b = SplitOf(y, 1); if (IsBroadLeaf(b)) {
 */
{ 2,2, 3,3, 1,1, 4,4, 1,1, 2,NULL_VALUE,
}
NEXT_FUZZ_DATA
/* Coverage in bstree.c premove_FildeshKV_BSTREE():
 *  ... do { if (IsBroadLeaf(y)) {
 */
{ 4,4, 3,3, 6,6, 8,8, 10,10, 9,9, 1,1, 7,7, 5,5, 2,2, 6,NULL_VALUE,
}
NEXT_FUZZ_DATA
/* Coverage in kv_broadleaf.c maybe_shifty_add():
 *  ... if (!IsBroadLeaf(y)) {
 */
{ 1,1, 2,2, 3,3, 4,4, 3,NULL_VALUE, 5,5,
}
END_FUZZ_DATA
