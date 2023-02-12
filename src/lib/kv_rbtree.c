#include "kv_bstree.h"


static FildeshKV_id_t ensure_FildeshKV_RBTREE(FildeshKV*, const void*, size_t, FildeshAlloc*);
static void remove_FildeshKV_RBTREE(FildeshKV*, FildeshKV_id_t);

const FildeshKV_VTable DEFAULT_RBTREE_FildeshKV_VTable = {
  first_id_FildeshKV_BSTREE,
  next_id_FildeshKV_BSTREE,
  lookup_FildeshKV_BSTREE,
  ensure_FildeshKV_RBTREE,
  remove_FildeshKV_RBTREE,
};

const FildeshKV_VTable DEFAULT_FildeshKV_VTable = {
  first_id_FildeshKV_BSTREE,
  next_id_FildeshKV_BSTREE,
  lookup_FildeshKV_BSTREE,
  ensure_FildeshKV_RBTREE,
  remove_FildeshKV_RBTREE,
};

static
  size_t
fixup_insert(FildeshKV* map, size_t x)
{
  size_t insertion_index = x;
  bool bubbling_insertion = true;

  /* Loop invariant is:
   * - The {x} side of {b} has as many black nodes as the other side.
   * - However, {x} and its parent {b} are both red, so we have to bubble up.
   */
  while (true) {
    size_t b;

    /* {x} is root, just set to black!*/
    if (IsRoot(x)) {
      fildesh_log_trace("Paint it black.");
      ColorBlack(x);
      break;
    }

    b = JointOf(x);
    /* {b} is black, {x} is safe to be red!*/
    if (!RedColorOf(b)) {
      fildesh_log_trace("Keep it red.");
      break;
    }

    /* Case 1.         (continue)
     *
     *    a#              b'+
     *    / \              / \
     *  1*   +b   -->    a#   #x
     *      / \          /|   |\
     *    2#   +'x     1* #2 3# #4
     *        / \
     *      3#   #4
     */
    if (SideOf(x) == SideOf(b)) {
      fildesh_log_trace("Case 1.");
      ColorBlack(x);
      RotateUp(&b);
      x = b;
      bubbling_insertion = false;
    }
    /* Case 2.                        (continue)
     *
     *       a#             a#           x'+
     *       / \            / \           / \
     *     b+   *4  -->   x+   *4  -->  b#   #a
     *     / \            / \           /|   |\
     *   1#   +'x       b#   #3       1# #2 3# *4
     *       / \        / \
     *     2#   #3    1#   #2
     */
    else {
      fildesh_log_trace("Case 2.");
      ColorBlack(b);
      RotateUp(&x);
      RotateUp(&x);
      if (bubbling_insertion) {
        insertion_index = x;
      }
    }
  }
  return insertion_index;
}

  FildeshKV_id_t
ensure_FildeshKV_RBTREE(
    FildeshKV* map, const void* k, size_t ksize, FildeshAlloc* alloc)
{
  const size_t old_freelist_head = map->freelist_head;
  FildeshKV_id_t x_id = ensure_FildeshKV_BSTREE(map, k, ksize, alloc);
  size_t x = x_id/2;
  assert((x_id & 1) == 0);
  if (old_freelist_head != map->freelist_head) {
    ColorRed(x);
    x = fixup_insert(map, x);
    x_id = 2*x;
  }
  return x_id;
}

/**
 * This function is called with {y} removed from the tree,
 * but {y->joint} points to a node whose {side} is one level
 * short on black nodes.
 **/
static
  void
fixup_remove(FildeshKV* map, size_t y, unsigned side)
{
  if (RedColorOf(y)) {
    fildesh_log_trace("Paint it black.");
    ColorBlack(y);
    return;
  }

  /* Loop invariant is:
   * - The {y} side of {b} is short by 1 black node.
   * - However, {y} is black, so we have to bubble up and fill the gap.
   * - Note that {y} may be a dummy node and not actually exist.
   */
  while (!IsRoot(y)) {
    size_t b, a, w, x;
    b = JointOf(y);
    a = SplitOf(b, 1-side);
    assert(!Nullish(a));
    w = SplitOf(a, 1-side);
    x = SplitOf(a, side);

    /* Case 1.                         (done)
     *
     *      b*             b*            x*
     *      / \            / \           / \
     *    a#   #'y  -->  x+   #y  -->  a#   #b
     *    / \            / \           /|   |\
     *  w*   +x        a#   #2       w* #1 2# #y
     *       / \       / \
     *     1#   #2   w*   #1
     */
    if (!Nullish(x) && RedColorOf(x)) {
      fildesh_log_trace("Case 1.");
      if (RedColorOf(b)) {
        ColorBlack(b);
      }
      else {
        ColorBlack(x);
      }
      RotateUp(&x);
      RotateUp(&x);
      break;
    }

    /* Case 2.           (done)
     *
     *      b+             a#
     *      / \            / \
     *    a#   #'y  -->  w*   +b
     *    / \                / \
     *  w*   #x            x#   #y
     */
    if (RedColorOf(b)) {
      fildesh_log_trace("Case 2.");
      RotateUp(&a);
      break;
    }

    /* Case 3.         (continue, match case 1 or 2)
     *
     *      b#             a#
     *      / \            / \
     *    a+   #'y  -->  w#   +b
     *    / \                / \
     *  w#   #x            x#   #'y
     */
    if (RedColorOf(a)) {
      fildesh_log_trace("Case 3.");
      ColorBlack(a);
      ColorRed(b);
      RotateUp(&a);
      b = SplitOf(a, side);
      if (Nullish(SplitOf(b, side))) {
        AssignJoint(y, b);
      }
      continue;  /* Match case 1 or 2.*/
    }

    /* Case 4.           (done)
     *
     *      b#             a#
     *      / \            / \
     *    a#   #'y  -->  w#   #b
     *    / \                / \
     *  w+   *x            x*   #y
     */
    if (!Nullish(w) && RedColorOf(w)) {
      fildesh_log_trace("Case 4.");
      ColorBlack(w);
      RotateUp(&a);
      break;
    }

    /* Case 5.         (continue)
     *
     *      b#            b'#
     *      / \            / \
     *    a#   #'y  -->  a+   #y
     *    / \            / \
     *  w#   #x        w#   #x
     */
    fildesh_log_trace("Case 5.");
    ColorRed(a);
    y = b;
    side = SideOf(y);
  }
}

  void
remove_FildeshKV_RBTREE(FildeshKV* map, FildeshKV_id_t y_id)
{
  size_t y, z;

  y = premove_FildeshKV_BSTREE(map, y_id/2);
#define ReplacementOf(y)  map->at[y].size
  z = ReplacementOf(y);
#undef ReplacementOf

  if (!Nullish(z)) {
    /* Recolor the node that replaced {y}.*/
    ColorSwap(y, z);
  }

  if (RedColorOf(y)) {
    /* If the removed color is red, then it is still a red-black tree.*/
    fildesh_log_trace("Removed red.");
  }
  else if (!Nullish(SplitOf(y, 0))) {
    fixup_remove(map, SplitOf(y, 0), 0);
  }
  else if (!Nullish(SplitOf(y, 1))) {
    fixup_remove(map, SplitOf(y, 1), 1);
  }
  else if (!Nullish(JointOf(y))) {
    /* Assume {y} is a leaf in the tree to simplify fixup.*/
    unsigned side = Nullish(SplitOf(JointOf(y), 1)) ? 1 : 0;
    assert(!Nullish(SplitOf(JointOf(y), 1-side)));
    fixup_remove(map, y, side);
  }
  else {
    fildesh_log_trace("JointOf(y) is null.");
  }
  reclaim_element_FildeshKV_SINGLE_LIST(map, y);
}

