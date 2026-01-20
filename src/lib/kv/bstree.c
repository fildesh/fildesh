#include "src/lib/kv/bstree.h"

static inline void assign_replacement_info(FildeshKV* map, size_t y, size_t x) {
  map->at[y].size = x;
}
#define AssignReplacementInfo(y, x)  assign_replacement_info(map, y, x)


static void remove_FildeshKV_BSTREE(FildeshKV*, FildeshKV_id_t);

const FildeshKV_VTable DEFAULT_BSTREE_FildeshKV_VTable = {
  first_id_FildeshKV_BSTREE,
  next_id_FildeshKV_BSTREE,
  lookup_FildeshKV_BSTREE,
  ensure_FildeshKV_BSTREE,
  remove_FildeshKV_BSTREE,
};

static
  FildeshKV_id_t
first_id_from_index(const FildeshKV* map, size_t x) {
  size_t y;
  do {
    do {
      y = x;
      if (IsBroadLeaf(y)) {
        return 2*y;
      }
      x = SplitOf(y, 0);
    } while (!Nullish(x));
    x = SplitOf(y, 1);
  } while (!Nullish(x));
  return 2*y;
}

  FildeshKV_id_t
first_id_FildeshKV_BSTREE(const FildeshKV* map) {
  if (map->freelist_head == 0) {
    return FildeshKV_NULL_ID;
  }
  return first_id_from_index(map, 0);
}

  FildeshKV_id_t
next_id_FildeshKV_BSTREE(const FildeshKV* map, FildeshKV_id_t id) {
  const size_t x = id/2;
  assert(!fildesh_nullid(id));
  if (((id & 1) == 0) && IsBroadLeaf(x)) {
    return id + 1;
  }
  if (!IsRoot(x)) {
    const size_t y = JointOf(x);
    if (SplitOf(y, 0) == x && !Nullish(SplitOf(y, 1))) {
      return first_id_from_index(map, SplitOf(y, 1));
    }
    return 2*y;
  }
  return FildeshKV_NULL_ID;
}

  FildeshKV_id_t
lookup_FildeshKV_BSTREE(const FildeshKV* map, const void* k, size_t ksize)
{
  size_t y = (map->freelist_head > 0 ? 0 : FildeshKV_NULL_INDEX);

  while (!Nullish(y)) {
    int si = - cmp_k_FildeshKVE(&map->at[y], ksize, k);
    if (si == 0) {return 2*y;}
    if (IsBroadLeaf(y)) {
      if (si > 0 && 0 == cmp_splitk_FildeshKVE(&map->at[y], ksize, k)) {
        return 2*y+1;
      }
      return FildeshKV_NULL_ID;
    }
    y = SplitOf(y, (si < 0 ? 0 : 1));
  }
  return FildeshKV_NULL_ID;
}

/** If a node matching {x} exists, return that node.
 * Otherwise, add {x} to the tree and return it.
 **/
  FildeshKV_id_t
ensure_FildeshKV_BSTREE(
    FildeshKV* map, const void* k, size_t ksize, FildeshAlloc* alloc)
{
  size_t a = FildeshKV_NULL_INDEX;
  size_t y = (map->freelist_head > 0 ? 0 : FildeshKV_NULL_INDEX);
  size_t x;
  unsigned side = 0;

  while (!Nullish(y)) {
    int si = - cmp_k_FildeshKVE(&map->at[y], ksize, k);
    if (si == 0) {return 2*y;}
    side = (si < 0 ? 0 : 1);
    a = y;
    y = SplitOf(y, side);
  }

  x = empty_add_FildeshKV_BSTREE(map);
  populate_empty_FildeshKVE(&map->at[x], ksize, k, 1, 0, alloc);
  AssignJoint(x, a);
  MaybeAssignSplit(a, side, x);
  return 2*x;
}

/** Remove a given node {y} from the tree.
 *
 * {y} will hold information about what had to be moved,
 * which is used by the red-black tree removal.
 * {y->joint} will hold {b} in the diagrams below.
 * {y->split} will hold the new split of {b} that changed depth.
 **/
  size_t
premove_FildeshKV_BSTREE(FildeshKV* map, size_t y)
{
  unsigned side_y = SideOf(y);
  unsigned side;
#define oside (1-side)
  FildeshKV_id_t b;
  FildeshKV_id_t x;

  /* This is the only case that leaves {y->joint} unchanged.
   * {y} can be the root.
   * {x} can be null.
   **** OLD ********* NEW *** AUX ***
   *     b*            b*      b*
   *    / \           / \      |
   *  0*   y'  -->  0*   *x    y
   *      /                     \
   *    x*                       *x
   */
  for (side = 0; side < 2; ++side) {
    b = JointOf(y);

    if (Nullish(SplitOf(y, oside))) {
      x = SplitOf(y, side);
      if (Nullish(x)) {
        MaybeAssignSplit(b, side_y, x);
        AssignReplacementInfo(y, x);
        return y;
      }
      SubJoin(&y, &x);
      /* Update {x} neighbors.*/
      MaybeAssignJoint(SplitOf(x, 0), x);
      MaybeAssignJoint(SplitOf(x, 1), x);
      /* Populate {y} for return.*/
      /* noop: AssignJoint(y, b); */
      AssignSplit(y, side_y, x);
      AssignReplacementInfo(y, x);
      return y;
    }
  }

  /* We know {1} exists.
   ****** OLD ******** NEW ***** AUX ***
   *       y'           b         b
   *      / \          / \        |
   *     b   1  -->  0*   1       y
   *    /                        /
   *  0*                       0*
   */
  for (side = 0; side < 2; ++side) {
    b = SplitOf(y, side);

    if (Nullish(SplitOf(b, oside))) {
#if 1
      AssignSplit(b, oside, SplitOf(y, oside));
      AssignJoint(b, JointOf(y));
      LocalSwap(&y, &b);
#else
      AssignSplit(b, oside, SplitOf(y, oside));
      SubJoin(&y, &b);
#endif
      /* Update {b} neighbors.*/
      MaybeAssignJoint(SplitOf(b, side), b);
      /* Populate {y} for return.*/
      AssignJoint(y, b);
      AssignSplit(y, side, SplitOf(b, side));
      NullifySplit(y, oside);
      AssignReplacementInfo(y, b);
      return y;
    }
  }

  /* We know both {0} and {3} exist.
   * {x} descends from {0} or {3} to be closest in value to {y}.
   * {b} follows one step behind {x}.
   **** OLD ******** NEW **** AUX ***
   *     y'           x        b
   *    / \          / \       |
   *  0*   3       0*   3      y
   *   .\.    -->   .\.         \
   *     b            b          *2
   *    / \          / \
   *  1*   x       1*   *2
   *      /
   *    2*
   */
  side = 0;  /* Arbitrary side.*/
  b = SplitOf(y, side);
  x = SplitOf(b, oside);
  do {
    b = x;
    x = SplitOf(b, oside);
  } while (!Nullish(x));
  x = b;
  b = JointOf(x);

  assert(oside == SideOf(x));
  Join(b, oside, SplitOf(x, side));

  AssignJoint(x, JointOf(y));
  if (Nullish(JointOf(y))) {
    LocalSwap(&y, &x);
  }
  else {
    AssignSplit(JointOf(y), side_y, x);
  }
  /* Update {x} and neighbors.*/
  Join(x, 0, SplitOf(y, 0));
  Join(x, 1, SplitOf(y, 1));

  /* Populate {y} for return.*/
  AssignJoint(y, b);
  NullifySplit(y, side);
  AssignSplit(y, oside, SplitOf(b, oside));
  AssignReplacementInfo(y, x);
  return y;
#undef oside
}

  void
remove_FildeshKV_BSTREE(FildeshKV* map, FildeshKV_id_t y_id)
{
  size_t y = y_id/2;
  y = premove_FildeshKV_BSTREE(map, y);
  reclaim_element_FildeshKV_SINGLE_LIST(map, y);
}

/** Do a tree rotation:
 *
 *       b             a
 *      / \           / \
 *     a   *z  -->  x*   b
 *    / \               / \
 *  x*   *y           y*   *z
 *
 * {b} always starts as {a}'s joint.
 **/
  void
rotate_up_FildeshKV_BSTREE(FildeshKV* map, size_t* p_a)
{
  size_t a = *p_a;
  size_t b = JointOf(a);
  const unsigned side = SideOf(a);
  const unsigned oside = 1-side;
  size_t y = SplitOf(a, oside);

  if (IsRoot(b)) {
    AssignJoint(a, FildeshKV_NULL_INDEX);
    LocalSwap(&b, &a);
    *p_a = a;
    /* Update {a} and {b} outer neighbors (they don't change below).*/
    MaybeAssignJoint(SplitOf(a, side), a);
    MaybeAssignJoint(SplitOf(b, oside), b);
  }
  else {
    Join(JointOf(b), SideOf(b), a);
  }
  Join(a, oside, b);
  Join(b, side, y);
}

