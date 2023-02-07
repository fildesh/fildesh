#include "kv_bstree.h"

#include "kv_broadleaf.h"


static FildeshKV_id_t ensure_FildeshKV_RBTREE(FildeshKV*, const void*, size_t, FildeshAlloc*);
static FildeshKV_id_t ensure_FildeshKV_BROADLEAF_RBTREE(FildeshKV*, const void*, size_t, FildeshAlloc*);
static void remove_FildeshKV_RBTREE(FildeshKV*, FildeshKV_id_t);

const FildeshKV_VTable DEFAULT_RBTREE_FildeshKV_VTable = {
  first_id_FildeshKV_BSTREE,
  next_id_FildeshKV_BSTREE,
  lookup_FildeshKV_BSTREE,
  ensure_FildeshKV_RBTREE,
  remove_FildeshKV_RBTREE,
};

const FildeshKV_VTable DEFAULT_BROADLEAF_RBTREE_FildeshKV_VTable = {
  first_id_FildeshKV_BSTREE,
  next_id_FildeshKV_BSTREE,
  lookup_FildeshKV_BSTREE,
  ensure_FildeshKV_BROADLEAF_RBTREE,
  remove_FildeshKV_RBTREE,
};

const FildeshKV_VTable DEFAULT_FildeshKV_VTable = {
  first_id_FildeshKV_BSTREE,
  next_id_FildeshKV_BSTREE,
  lookup_FildeshKV_BSTREE,
  ensure_FildeshKV_BROADLEAF_RBTREE,
  remove_FildeshKV_RBTREE,
};


static
  size_t
first_fixup_insert(FildeshKV* map, size_t x)
{
  size_t w;
  if (IsBroadLeaf(x)) {
    w = x;
    x = JointOf(x);
  }
  else {
    w = SplitOf(x, 0);
    if (Nullish(w)) {
      w = SplitOf(x, 1);
    }
    else {
      assert(Nullish(SplitOf(x, 1)));
    }
  }

  ColorRed(x);
  if (!Nullish(w)) {
    assert(IsBroadLeaf(w));
    ColorSwap(x, w);
    return w;
  }
  return x;
}

static inline
  size_t
next_fixup_insert(FildeshKV* map, size_t x)
{
  const size_t b = JointOf(x);
  /* {x} is root, just set to black!*/
  if (Nullish(b)) {
    fildesh_log_trace("next: Paint it black.");
    ColorBlack(x);
    return FildeshKV_NULL_INDEX;
  }

  /* {b} is black, {x} is safe to be red!*/
  if (!RedColorOf(b)) {
    fildesh_log_trace("next: Keep it red.");
    return FildeshKV_NULL_INDEX;
  }
  return b;
}

static
  size_t
fixup_insert(FildeshKV* map, FildeshKV_id_t insertion_id)
{
  size_t x = insertion_id/2;
  size_t b;
  x = first_fixup_insert(map, x);

  assert(IsLeaf(x));
  b = next_fixup_insert(map, x);
  if (!Nullish(b)) {
    const bool bubbling_insertion = (insertion_id/2 == x);
    unsigned b_side = SideOf(b);
    size_t a = JointOf(b);
    size_t c = SplitOf(a, 1-b_side);
    /* Easy. Just flip the colors of a,b,c.
     *     a#        a'+
     *     / \        / \
     *    +b  +c --> #b  #c
     *    |          |
     *  x'+y?      x'+y?
     */
    if (!Nullish(c)) {
      ColorBlack(b);
      ColorBlack(c);
      ColorRed(a);
      x = a;
    }
    else if (b_side == SideOf(x)) {
      /* Case 1 in the loop will handle this.*/
    }
    else if (!IsBroadLeaf(x)) {
      /* Case 2 in the loop will handle this.*/
    }
    else if (b_side == 0) {
      /* Case 0.1 for leaf.
       *    a#       y'+
       *    /         / \
       *  b+   -->  b#x a#
       *    \
       *    x+y
       */
      AssignJoint(x, JointOf(a));
      LocalSwap(&x, &a);
      {
        FildeshKVE e = map->at[x];
        erase_splitk_FildeshKVE(&e);
        fuse_FildeshKVE(&map->at[b], 1, &e);
      }
      promote_splitk_FildeshKVE(&map->at[x]);
      Join(x, 0, b);
      ColorBlack(b);
      Join(x, 1, a);
      NullifySplit(a, 0);

      if (bubbling_insertion) {
        if ((insertion_id & 1) == 0) {
          insertion_id = 2*b+1;
        }
        else {
          insertion_id = 2*x;
        }
      }
    }
    else {
      /* Case 0.2 for leaf.
       *  a'#           y'+
       *     \           / \
       *      +b  -->  a#x  #b
       *     /
       *   x+y
       */
      AssignJoint(x, JointOf(a));
      LocalSwap(&x, &a);
      {
        FildeshKVE e = map->at[x];
        erase_splitk_FildeshKVE(&e);
        fuse_FildeshKVE(&map->at[a], 1, &e);
      }
      promote_splitk_FildeshKVE(&map->at[x]);
      Join(x, 0, a);
      Join(x, 1, b);
      ColorBlack(b);
      NullifySplit(b, 0);

      if (bubbling_insertion) {
        if ((insertion_id & 1) == 0) {
          insertion_id = 2*a+1;
        }
        else {
          insertion_id = 2*x;
        }
      }
    }
  }

  /* Loop invariant is:
   * - The {x} side of {b} has as many black nodes as the other side.
   * - However, {x} and its parent {b} are both red, so we have to bubble up.
   */
  for (b = next_fixup_insert(map, x); !Nullish(b);
       b = next_fixup_insert(map, x))
  {
    /* Case 1.         (continue)
     *
     *    a#              b'+
     *    / \              / \
     *  1*   +b   -->    a#   #x
     *      / \          /|   |\
     *    2#   +'x     1* #2 3# #4
     *        / \
     *      3#   #4
     *
     * Note: For broadleaf trees, there's no opportunity to fuse here.
     */
    if (SideOf(x) == SideOf(b)) {
      const bool bubbling_insertion = (insertion_id/2 == b);
      fildesh_log_trace("Case 1.");
      ColorBlack(x);
      RotateUp(&b);
      x = b;
      if (bubbling_insertion) {
        insertion_id = 2*b;
      }
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
     *
     * Note: For broadleaf trees, there's no opportunity to fuse here.
     */
    else {
      const bool bubbling_insertion = (insertion_id/2 == x);
      fildesh_log_trace("Case 2.");
      ColorBlack(b);
      RotateUp(&x);
      RotateUp(&x);
      if (bubbling_insertion) {
        insertion_id = (2 * x) | (insertion_id & 1);
      }
    }
  }
  return insertion_id;
}

  FildeshKV_id_t
ensure_FildeshKV_RBTREE(
    FildeshKV* map, const void* k, size_t ksize, FildeshAlloc* alloc)
{
  const size_t old_freelist_head = map->freelist_head;
  FildeshKV_id_t x_id = ensure_FildeshKV_BSTREE(map, k, ksize, alloc);
  assert((x_id & 1) == 0);
  if (old_freelist_head != map->freelist_head) {
    x_id = fixup_insert(map, x_id);
  }
  return x_id;
}

  FildeshKV_id_t
ensure_FildeshKV_BROADLEAF_RBTREE(
    FildeshKV* map, const void* k, size_t ksize, FildeshAlloc* alloc)
{
  const size_t old_freelist_head = map->freelist_head;
  FildeshKV_id_t x_id = ensure_FildeshKV_BROADLEAF_BSTREE(map, k, ksize, alloc);
  if (old_freelist_head != map->freelist_head) {
    x_id = fixup_insert(map, x_id);
  }
  return x_id;
}

/**
 * This function is called with {y} removed from the tree,
 * but {y->joint} points to a node whose {side} is one level
 * short on black nodes.
 **/
static
  size_t
fixup_remove(FildeshKV* map, size_t y, unsigned side, size_t removal_index)
{
  if (RedColorOf(y)) {
    fildesh_log_trace("Paint it black.");
    ColorBlack(y);
    return removal_index;
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
    if (IsBroadLeaf(a)) {
      if (fixup_remove_case_0_FildeshKV_BROADLEAF_RBTREE(map, b, a)) {
        break;
      }
      y = b;
      side = SideOf(y);
      continue;
    }
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
      if (IsBroadLeaf(x)) {
        fixup_remove_case_1_FildeshKV_BROADLEAF_RBTREE(
            map, removal_index, side, b, a, x);
        removal_index = FildeshKV_NULL_INDEX;
        break;
      }
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
  return removal_index;
}

  void
remove_FildeshKV_RBTREE(FildeshKV* map, FildeshKV_id_t y_id)
{
  size_t y = y_id/2;
  size_t z;

  if ((y_id & 1) != 0) {
    erase_splitk_FildeshKVE(&map->at[y]);
    return;
  }
  y = premove_FildeshKV_BSTREE(map, y);
  if (Nullish(y)) {
    return;
  }
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
    y = fixup_remove(map, SplitOf(y, 0), 0, y);
  }
  else if (!Nullish(SplitOf(y, 1))) {
    y = fixup_remove(map, SplitOf(y, 1), 1, y);
  }
  else if (!Nullish(JointOf(y))) {
    /* Assume {y} is a leaf in the tree to simplify fixup.*/
    unsigned side = Nullish(SplitOf(JointOf(y), 1)) ? 1 : 0;
    assert(!Nullish(SplitOf(JointOf(y), 1-side)));
    y = fixup_remove(map, y, side, y);
  }
  else {
    fildesh_log_trace("JointOf(y) is null.");
  }
  if (!Nullish(y)) {
    reclaim_element_FildeshKV_SINGLE_LIST(map, y);
  }
}

