#include "test/fuzz/smoke_common.h"

#define NULL_VALUE 255

BEGIN_FUZZ_DATA
/* Map 1->10, remove 1, then map 2->20.*/
{ 2,
  1,
}
NEXT_FUZZ_DATA
/* Map 1->10, then remove 1->10.*/
{ 1,
  1,0,
}
NEXT_FUZZ_DATA
/* Map 1->10, 2->20, then remove 1->10.*/
{ 1,
  2,
  1,0,
}
NEXT_FUZZ_DATA
/* Remove the root of linear tree, then remove leaf.
 *  1           2         2
 *   \           \   -->
 *    2    -->    3
 *     \
 *      3
 */
{ 1,
  2,
  3,
  1,0,
  3,0,
}
NEXT_FUZZ_DATA
/* Remove the root of this tree:
 *    2
 *   / \
 *  1   3
 */
{ 2,
  1,
  3,
  2,0,
}
NEXT_FUZZ_DATA
/* Remove the root of this tree:
 *     _3_
 *   _/   \_
 *  1       5
 *   \     /
 *    2   4
 */
{ 3,
  1,
  2,
  5,
  4,
  3,0,
}
NEXT_FUZZ_DATA
/* Remove node below the root of this tree:
 *        6
 *       /
 *     _3_
 *   _/   \_
 *  1       5
 *   \     /
 *    2   4
 */
{ 6,
  3,
  1,
  2,
  5,
  4,
  3,0,
}
END_FUZZ_DATA
