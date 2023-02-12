#include "test/fuzz/smoke_common.h"

BEGIN_FUZZ_DATA
/* Add 1, remove 1, then add 2.*/
{ 2,
  1,
}
NEXT_FUZZ_DATA
/* Add 1, then remove 1.*/
{ 1, 1,0,
}
NEXT_FUZZ_DATA
/* Add 1, then add 2, then remove 1.*/
{ 1, 2, 1,0,
}
NEXT_FUZZ_DATA
/* Remove the root of linear tree, then remove leaf.
 *  1           2         2
 *   \           \   -->
 *    2    -->    3
 *     \
 *      3
 */
{ 1, 2, 3, 1,0, 3,0,
}
NEXT_FUZZ_DATA
/* Remove the root of this tree:
 *    2
 *   / \
 *  1   3
 */
{ 2, 1, 3, 2,0,
}
NEXT_FUZZ_DATA
/* Remove the root of this tree:
 *     _3_
 *   _/   \_
 *  1       5
 *   \     /
 *    2   4
 */
{ 3, 1, 2, 5, 4, 3,0,
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
{ 6, 3, 1, 2, 5, 4, 3,0,
}
END_FUZZ_DATA
