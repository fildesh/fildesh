
/* #define FILDESH_LOG_TRACE_ON 1 */
#include "kv_broadleaf.h"

#include "kv_bstree.h"

static
  FildeshKV_id_t
joining_add_FildeshKV_BSTREE(
    FildeshKV* map, size_t a, size_t x, unsigned side,
    const void* k, size_t ksize, FildeshAlloc* alloc)
{
  size_t b = empty_add_FildeshKV_BSTREE(map);
  populate_empty_FildeshKVE(&map->at[b], ksize, k, 1, 0, alloc);
  if (Nullish(a)) {
    LocalSwap(&b, &x);
    fildesh_log_trace("swap");
  }
  else {
    AssignSplit(a, SideOf(x), b);
    fildesh_log_trace("reggo join");
  }
  AssignJoint(b, a);
  AssignSplit(b, 1-side, x);
  AssignJoint(x, b);
  return 2*b;
}

static
  FildeshKV_id_t
taking_add_FildeshKV_BSTREE(
    FildeshKV* map, size_t b, unsigned side,
    const void* k, size_t ksize, FildeshAlloc* alloc)
{
  size_t x = empty_add_FildeshKV_BSTREE(map);
  map->at[x] = map->at[b];
  fildesh_log_trace("taking");

  AssignJoint(x, b);
  if (side == 0) {
    erase_splitk_FildeshKVE(&map->at[x]);
    populate_demote_FildeshKVE(&map->at[x], ksize, k, 1, 0, alloc);
    promote_splitk_FildeshKVE(&map->at[b]);
  }
  else {
    promote_splitk_FildeshKVE(&map->at[x]);
    populate_splitkv_FildeshKVE(&map->at[x], ksize, k, 1, 0, alloc);
    erase_splitk_FildeshKVE(&map->at[b]);
  }
  AssignSplit(b, side, x);
  NullifySplit(b, 1-side);
  return 2*x+side;
}

static
  FildeshKV_id_t
splitting_add_FildeshKV_BSTREE(
    FildeshKV* map, size_t b, unsigned side,
    const void* k, size_t ksize, FildeshAlloc* alloc)
{
  size_t x = empty_add_FildeshKV_BSTREE(map);
  map->at[x] = map->at[b];
  fildesh_log_trace("splitting");

  AssignJoint(x, b);
  if (side == 0) {
    erase_splitk_FildeshKVE(&map->at[x]);
    populate_splitkv_FildeshKVE(&map->at[x], ksize, k, 1, 0, alloc);
    promote_splitk_FildeshKVE(&map->at[b]);
  }
  else {
    promote_splitk_FildeshKVE(&map->at[x]);
    populate_demote_FildeshKVE(&map->at[x], ksize, k, 1, 0, alloc);
    erase_splitk_FildeshKVE(&map->at[b]);
  }
  AssignSplit(b, side, x);
  return 2*x+(1-side);
}

static
  FildeshKV_id_t
leaf_add_FildeshKV_BSTREE(
    FildeshKV* map, size_t a, unsigned side,
    const void* k, size_t ksize, FildeshAlloc* alloc)
{
  size_t x;
  if (!Nullish(a) && Nullish(SplitOf(a, 1-side))) {
    if (side == 0) {
      fildesh_log_trace("leaf0");
      if (populate_demote_FildeshKVE(&map->at[a], ksize, k, 1, 0, alloc)) {
        return 2*a;
      }
    }
    else {
      fildesh_log_trace("leaf1");
      if (populate_splitkv_FildeshKVE(&map->at[a], ksize, k, 1, 0, alloc)) {
        return 2*a+1;
      }
    }
  }
  x = empty_add_FildeshKV_BSTREE(map);
  populate_empty_FildeshKVE(&map->at[x], ksize, k, 1, 0, alloc);
  AssignJoint(x, a);
  MaybeAssignSplit(a, side, x);
  return 2*x;
}

/**
 * Shift some values around.
 *       a          y
 *      / \   -->  / \
 *     y   z      x  a,z
 **/
static
  FildeshKV_id_t
maybe_shifty_add(
    FildeshKV* map, size_t a, unsigned side, size_t y,
    const void* k, size_t ksize, FildeshAlloc* alloc)
{
  int si;
  size_t z;
  if (!(IsLeaf(y) && !Nullish(a))) {
    return FildeshKV_NULL_ID;
  }
  z = SplitOf(a, 1-side);
  if (!(!Nullish(z) && IsLeaf(z) && !IsBroadLeaf(z))) {
    return FildeshKV_NULL_ID;
  }
  if (!IsBroadLeaf(y) || side == 1) {
    si = - cmp_k_FildeshKVE(&map->at[y], ksize, k);
    if (si == 0) {return 2*y;}
  }
  else {
    si = - cmp_splitk_FildeshKVE(&map->at[y], ksize, k);
    if (si == 0) {return 2*y+1;}
  }

  if (!maybe_fuse_FildeshKVE(&map->at[z], side, &map->at[a])) {
    return FildeshKV_NULL_ID;
  }

  /* Cases where we put the new element in the now-empty {a}.*/
  if ((side == 0 && 0 < si) || (si < 0 && side == 1)) {
    populate_empty_FildeshKVE(&map->at[a], ksize, k, 1, 0, alloc);
    return 2*a;
  }

  if (!IsBroadLeaf(y)) {
    move_kv_to_empty_FildeshKVE(&map->at[a], &map->at[y]);
    populate_empty_FildeshKVE(&map->at[y], ksize, k, 1, 0, alloc);
    return 2*y;
  }

  if (side == 1) {
    move_kv_to_empty_FildeshKVE(&map->at[a], &map->at[y]);
  }
  else {
    move_splitkv_to_empty_FildeshKVE(&map->at[a], &map->at[y]);
  }
  si = - cmp_k_FildeshKVE(&map->at[y], ksize, k);
  if (si == 0) {return 2*y;}

  if (si < 0) {
    if (populate_demote_FildeshKVE(&map->at[y], ksize, k, 1, 0, alloc)) {
      return 2*y;
    }
    return leaf_add_FildeshKV_BSTREE(map, y, 0, k, ksize, alloc);
  }
  if (populate_splitkv_FildeshKVE(&map->at[y], ksize, k, 1, 0, alloc)) {
    return 2*y+1;
  }
  return leaf_add_FildeshKV_BSTREE(map, y, 1, k, ksize, alloc);
}

/** If a node matching {x} exists, return that node.
 * Otherwise, add {x} to the tree and return it.
 **/
  FildeshKV_id_t
ensure_FildeshKV_BROADLEAF_BSTREE(
    FildeshKV* map, const void* k, size_t ksize, FildeshAlloc* alloc)
{
  size_t a = FildeshKV_NULL_INDEX;
  size_t y = (map->freelist_head > 0 ? 0 : FildeshKV_NULL_INDEX);
  unsigned side = 0;

  while (!Nullish(y)) {
    FildeshKV_id_t id;
    int si;
    id = maybe_shifty_add(map, a, side, y, k, ksize, alloc);
    if (!fildesh_nullid(id)) {return id;}

    si = - cmp_k_FildeshKVE(&map->at[y], ksize, k);
    if (si == 0) {return 2*y;}

    if (IsBroadLeaf(y)) {
      if (si < 0) {
        if (side == 0) {
          return taking_add_FildeshKV_BSTREE(map, y, 0, k, ksize, alloc);
        }
        else {
          return joining_add_FildeshKV_BSTREE(map, a, y, 0, k, ksize, alloc);
        }
      }
      si = - cmp_splitk_FildeshKVE(&map->at[y], ksize, k);
      if (si == 0) {return 2*y+1;}
      if (si > 0) {
        if (side == 0) {
          return joining_add_FildeshKV_BSTREE(map, a, y, 1, k, ksize, alloc);
        }
        else {
          return taking_add_FildeshKV_BSTREE(map, y, 1, k, ksize, alloc);
        }
      }
      return splitting_add_FildeshKV_BSTREE(map, y, side, k, ksize, alloc);
    }
    side = (si < 0 ? 0 : 1);
    a = y;
    y = SplitOf(y, side);
  }

  return leaf_add_FildeshKV_BSTREE(map, a, side, k, ksize, alloc);
}

  void
maybe_fuse_FildeshKV_BROADLEAF_RBTREE(FildeshKV* map, size_t b)
{
  size_t w = SplitOf(b, 0);
  size_t x = SplitOf(b, 1);

  assert(!RedColorOf(b) && "Only called on black nodes.");
  if (Nullish(w) && Nullish(x)) {
    return;
  }
  if (!Nullish(w)) {
    if (!IsLeaf(w) || IsBroadLeaf(w) || !RedColorOf(w)) {
      return;
    }
  }
  if (!Nullish(x)) {
    if (!IsLeaf(x) || IsBroadLeaf(x) || !RedColorOf(x)) {
      return;
    }
  }

  if (Nullish(w)) {
    if (maybe_fuse_FildeshKVE(&map->at[b], 1, &map->at[x])) {
      reclaim_element_FildeshKV_SINGLE_LIST(map, x);
    }
  }
  else if (Nullish(x)) {
    if (maybe_fuse_FildeshKVE(&map->at[b], 0, &map->at[w])) {
      reclaim_element_FildeshKV_SINGLE_LIST(map, w);
    }
  }
  else {
    if (maybe_fuse_FildeshKVE(&map->at[w], 1, &map->at[b])) {
      LocalSwap(&b, &x);
      AssignJoint(x, JointOf(b));
      AssignSplit(x, 0, w);
      NullifySplit(x, 1);
      reclaim_element_FildeshKV_SINGLE_LIST(map, b);
    }
  }
}

/* Case of split {a}.
 *
 *      b+             b#   (done)
 *      / \            / \
 *    a#x  #'y  -->  a+x  +'y  {y} doesn't in the tree.
 *
 *      b#             b'#   (continue)
 *      / \            / \
 *    a#x  #'y  -->  a+x  +'y
 *
 */
  bool
fixup_remove_case_0_FildeshKV_BROADLEAF_RBTREE(
    FildeshKV* map, size_t b, size_t a)
{
  assert(!RedColorOf(a));
  assert(Nullish(SplitOf(b, 1-SideOf(a))));
  ColorRed(a);
  if (RedColorOf(b)) {
    ColorBlack(b);
    return true;
  }
  return false;
}

/** Case of split {x}.
 *
 *      b*             z*
 *      / \            / \
 *    a#   #'y  -->  a#   #b
 *    / \            / \
 *  w+? x+z        w*  x+
 **/
  void
fixup_remove_case_1_FildeshKV_BROADLEAF_RBTREE(
    FildeshKV* map, size_t y, unsigned side,
    size_t b, size_t a, size_t x)
{
  size_t z = y;
  fildesh_log_trace("split x");
  assert(Nullish(SplitOf(b, side)));

  if (side == 0) {
    move_kv_to_empty_FildeshKVE(&map->at[z], &map->at[x]);
  }
  else {
    move_splitkv_to_empty_FildeshKVE(&map->at[z], &map->at[x]);
  }
  AssignJoint(z, JointOf(b));
  if (Nullish(JointOf(b))) {
    LocalSwap(&z, &b);
  }
  else {
    AssignSplit(JointOf(b), SideOf(b), z);
  }
  AssignSplit(z, side, b);
  AssignSplit(z, 1-side, a);
  AssignJoint(a, z);
  AssignJoint(b, z);
  ColorBlack(z);
  ColorSwap(z, b);
  NullifySplit(b, 1-side);
}
