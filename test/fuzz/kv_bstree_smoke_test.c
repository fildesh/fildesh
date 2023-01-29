#include "smoke_common.h"

#define NULL_VALUE 255

BEGIN_FUZZ_DATA
/* Map 1->10, remove 1, then map 2->20.*/
{ 2, 20,
  1, 10,
}
NEXT_FUZZ_DATA
/* Map 1->10, then remove 1->10.*/
{ 1, 10,
  1, NULL_VALUE,
}
NEXT_FUZZ_DATA
/* Map 1->10, 2->20, then remove 1->10.*/
{ 1, 10,
  2, 20,
  1, NULL_VALUE,
}
NEXT_FUZZ_DATA
/* Remove the root of linear tree, then remove leaf.
 *  1           2         2
 *   \           \   -->
 *    2    -->    3
 *     \
 *      3
 */
{ 1, 10,
  2, 20,
  3, 30,
  1, NULL_VALUE,
  3, NULL_VALUE,
}
NEXT_FUZZ_DATA
/* Remove the root of this tree:
 *    2
 *   / \
 *  1   3
 */
{ 2, 20,
  1, 10,
  3, 30,
  2, NULL_VALUE,
}
NEXT_FUZZ_DATA
/* Remove the root of this tree:
 *     _3_
 *   _/   \_
 *  1       5
 *   \     /
 *    2   4
 */
{ 3, 30,
  1, 10,
  2, 20,
  5, 50,
  4, 40,
  3, NULL_VALUE,
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
{ 6, 60,
  3, 30,
  1, 10,
  2, 20,
  5, 50,
  4, 40,
  3, NULL_VALUE,
}
END_FUZZ_DATA
