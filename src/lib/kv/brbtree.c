#include "src/lib/kv/bstree.h"

static FildeshKV_id_t ensure_FildeshKV_BRBTREE(FildeshKV*, const void*, size_t, FildeshAlloc*);
static void remove_FildeshKV_BRBTREE(FildeshKV*, FildeshKV_id_t);

const FildeshKV_VTable DEFAULT_BRBTREE_FildeshKV_VTable = {
  first_id_FildeshKV_BSTREE,
  next_id_FildeshKV_BSTREE,
  lookup_FildeshKV_BSTREE,
  ensure_FildeshKV_BRBTREE,
  remove_FildeshKV_BRBTREE,
};

static inline bool detect_d2_subtree(const FildeshKV* map, size_t b) {
  if (IsBroadLeaf(b)) {return true;}
  return !IsBroadLeaf(SplitOf(b, 1)) && Nullish(SplitOf(b, 0));
}

/** Find a position to insert.
 *
 * Even results are found.
 * Won't find leaf nodes.
 **/
static
  FildeshKV_id_t
lookup_for_ensure_FildeshKV_BRBTREE(const FildeshKV* map, const FildeshKVE* e)
{
  size_t y = 0;
  if (map->freelist_head == 0) {
    return FildeshKV_NULL_ID;
  }

  while (!IsLeaf(y)) {
    const size_t b = y;
    int si = cmp_FildeshKVE(e, &map->at[y]);
    if (si == 0) {return 2*y;}
    y = SplitOf(y, (si < 0 ? 0 : 1));
    if (Nullish(y)) {return 2*b+1;}
  }
  return 2*y+1;
}

/** Find element that definitely exists.
 *
 * Even results are found.
 * Won't find leaf nodes.
 **/
static
  FildeshKV_id_t
lookup_from_FildeshKV_BRBTREE(const FildeshKV* map, size_t y, const FildeshKVE* e)
{
  int si;
  while (!IsLeaf(y)) {
    const size_t b = y;
    si = cmp_FildeshKVE(e, &map->at[y]);
    if (si == 0) {return 2*y;}
    y = SplitOf(y, (si < 0 ? 0 : 1));
    if (Nullish(y)) {return 2*b+1;}
  }
  si = cmp_FildeshKVE(e, &map->at[y]);
  if (si == 0) {return 2*y;}
  assert(si == 2);
  return 2*y+1;
}

/** Add to single element on left. Might become oversized.**/
static FildeshKV_id_t insert_0_root1_sblack(FildeshKV* map, size_t y, const FildeshKVE* e) {
  if (map->at[y].size > FildeshKVE_splitksize_max ||
      e->size > FildeshKVE_splitksize_max)
  {
    size_t x = empty_add_FildeshKV_BSTREE(map);
    map->at[x] = *e;
    SubJoin(&y, &x);
    Join(x, 1, y);
    ColorRed(y);
    return 2*x;
  }
  fuse_FildeshKVE(&map->at[y], 0, e);
  return 2*y;
}
/** Add to single element on right. Might become oversized.**/
static FildeshKV_id_t insert_1_root1_sblack(FildeshKV* map, size_t y, const FildeshKVE* e) {
  if (map->at[y].size > FildeshKVE_splitksize_max ||
      e->size > FildeshKVE_splitksize_max)
  {
    size_t x = empty_add_FildeshKV_BSTREE(map);
    map->at[x] = *e;
    Join(y, 1, x);
    ColorRed(x);
    return 2*x;
  }
  fuse_FildeshKVE(&map->at[y], 1, e);
  return 2*y+1;
}

/** Add to 3-entry tree at single black node (the root).**/
static FildeshKV_id_t insert_0_root3_sblack(FildeshKV* map, size_t b, const FildeshKVE* e) {
  const size_t z = SplitOf(b, 1);
  const size_t y = empty_add_FildeshKV_BSTREE(map);
  map->at[y] = map->at[b];
  Join(y, 1, z);
  map->at[b] = *e;
  Join(b, 1, y);
  return 2*b;
}

/** Add to 3-entry tree at single red node.**/
static FildeshKV_id_t insert_0_root3_sred(FildeshKV* map, size_t y, const FildeshKVE* e) {
  size_t a = JointOf(y);
  size_t b = empty_add_FildeshKV_BSTREE(map);
  map->at[b] = map->at[a];
  Join(b, 0, SplitOf(b, 0));
  Join(b, 1, SplitOf(b, 1));
  copy_kv_FildeshKVE(&map->at[a], e);
  NullifySplit(a, 0);
  Join(a, 1, b);
  return 2*a;
}

/** Add to 4-entry tree at single black node (the root).**/
static FildeshKV_id_t insert_0_root4_sblack(FildeshKV* map, size_t b, const FildeshKVE* e) {
  size_t w, x;
  size_t y = SplitOf(b, 1);

  if (Nullish(SplitOf(y, 0))) {
    size_t z = SplitOf(y, 1);
    LocalSwap(&b, &y);
    w = b;
    NullifyJoint(y);
    Join(y, 0, w);
    Join(y, 1, z);
    fuse_FildeshKVE(&map->at[w], 0, e);
    ColorBlack(z);
    return 2*w;
  }

  x = SplitOf(y, 0);
  NullifySplit(y, 0);
  LocalSwap(&b, &x);

  w = empty_add_FildeshKV_BSTREE(map);
  map->at[w] = *e;

  ColorBlack(x);
  NullifyJoint(x);
  Join(x, 0, w);
  Join(x, 1, y);

  ColorRed(b);
  Join(w, 1, b);
  NullifySplit(b, 0);
  NullifySplit(b, 1);
  return 2*w;
}

/** Standard. Left.**/
static FildeshKV_id_t insert_0_d2_bblack(FildeshKV* map, size_t y, const FildeshKVE* e) {
  size_t x = empty_add_FildeshKV_BSTREE(map);
  copy_kv_FildeshKVE(&map->at[x], e);
  SubJoin(&y, &x);
  Join(x, 1, y);
  ColorRed(y);
  return 2*x;
}
/** Oversized. Left.**/
static FildeshKV_id_t insert_0_d2_sblack(FildeshKV* map, size_t y, const FildeshKVE* e) {
  size_t x = empty_add_FildeshKV_BSTREE(map);
  copy_kv_FildeshKVE(&map->at[x], e);
  ColorRed(x);
  Join(y, 0, x);
  return 2*x;
}
/** Standard. Middle.**/
static FildeshKV_id_t insert_1_d2_bblack(FildeshKV* map, size_t y, const FildeshKVE* e) {
  size_t x = empty_add_FildeshKV_BSTREE(map);
  map->at[x] = map->at[y];
  promote_splitk_FildeshKVE(&map->at[x]);
  erase_splitk_FildeshKVE(&map->at[y]);
  Join(y, 1, x);
  ColorRed(x);
  fuse_FildeshKVE(&map->at[x], 0, e);
  return 2*x;
}
/** Oversized. Middle.**/
static FildeshKV_id_t insert_1_d2_sred(FildeshKV* map, size_t y, const FildeshKVE* e) {
  size_t b = JointOf(y);
  size_t x = empty_add_FildeshKV_BSTREE(map);
  copy_kv_FildeshKVE(&map->at[x], e);
  SubJoin(&b, &x);
  Join(x, 0, b);
  Join(x, 1, y);
  ColorBlack(x);
  ColorRed(b);
  ColorRed(y);
  return 2*x;
}
/** Standard. Right. May become oversized.**/
static FildeshKV_id_t insert_2_d2_bblack(FildeshKV* map, size_t y, const FildeshKVE* e) {
  size_t x = empty_add_FildeshKV_BSTREE(map);
  if (e->size > FildeshKVE_splitksize_max) {
    size_t w = empty_add_FildeshKV_BSTREE(map);
    map->at[w] = map->at[y];
    copy_kv_FildeshKVE(&map->at[x], e);
    erase_splitk_FildeshKVE(&map->at[w]);
    promote_splitk_FildeshKVE(&map->at[y]);
    Join(y, 0, w);
    Join(y, 1, x);
    ColorRed(w);
    ColorRed(x);
    return 2*x;
  }
  map->at[x] = map->at[y];
  promote_splitk_FildeshKVE(&map->at[x]);
  erase_splitk_FildeshKVE(&map->at[y]);
  Join(y, 1, x);
  ColorRed(x);
  fuse_FildeshKVE(&map->at[x], 1, e);
  return 2*x+1;
}
/** Oversized. Right.**/
static FildeshKV_id_t insert_2_d2_sred(FildeshKV* map, size_t y, const FildeshKVE* e) {
  size_t b = JointOf(y);
  size_t x = empty_add_FildeshKV_BSTREE(map);
  copy_kv_FildeshKVE(&map->at[x], e);
  SubJoin(&b, &y);
  Join(y, 0, b);
  Join(y, 1, x);
  ColorBlack(y);
  ColorRed(b);
  ColorRed(x);
  return 2*x;
}

/** Oversized. LMiddle.**/
static FildeshKVE lshins_1_d3_sred(FildeshKV* map, size_t y, const FildeshKVE* e) {
  const FildeshKVE g = map->at[y];
  copy_kv_FildeshKVE(&map->at[y], e);
  return g;
}
/** Oversized. RMiddle.**/
static FildeshKVE lshins_2_d3_sred(FildeshKV* map, size_t y, const FildeshKVE* e) {
  const size_t b = JointOf(y);
  const FildeshKVE g = lshins_1_d3_sred(map, SplitOf(b, 0), &map->at[b]);
  copy_kv_FildeshKVE(&map->at[b], e);
  return g;
}
/** Oversized. Right. Shift everything.**/
static FildeshKVE lshins_3_d3_sred(FildeshKV* map, size_t y, const FildeshKVE* e) {
  const FildeshKVE g = lshins_2_d3_sred(map, y, &map->at[y]);
  copy_kv_FildeshKVE(&map->at[y], e);
  return g;
}
/** Standard. LMiddle.**/
static FildeshKVE lshins_1_d3_rred(FildeshKV* map, size_t y, const FildeshKVE* e) {
  const size_t b = JointOf(y);
  const FildeshKVE g = map->at[b];
  copy_kv_FildeshKVE(&map->at[b], e);
  return g;
}
/** Standard. RMiddle.**/
static FildeshKVE lshins_2_d3_rred(FildeshKV* map, size_t y, const FildeshKVE* e) {
  const FildeshKVE g = lshins_1_d3_rred(map, y, &map->at[y]);
  copy_kv_FildeshKVE(&map->at[y], e);
  return g;
}
/** Standard. Right. May become oversized. Shift everything.**/
static FildeshKVE lshins_3_d3_rred(FildeshKV* map, size_t y, const FildeshKVE* e) {
  const FildeshKVE g = lshins_1_d3_rred(map, y, &map->at[y]);
  promote_splitk_FildeshKVE(&map->at[y]);
  if (ksize_FildeshKVE_size(e->size) <= FildeshKVE_splitksize_max) {
    fuse_FildeshKVE(&map->at[y], 1, e);
  }
  else {
    insert_2_d2_sred(map, y, e);
  }
  return g;
}

static FildeshKVE lsh_d3(FildeshKV* map, size_t b) {
  FildeshKVE g;
  const size_t x = SplitOf(b, 0);
  if (Nullish(x)) {
    const size_t a = JointOf(b);
    const size_t y = SplitOf(b, 1);
    g = map->at[b];
    map->at[b] = map->at[y];
    AssignJoint(b, a);
    ColorBlack(b);
    reclaim_element_FildeshKV_SINGLE_LIST(map, y);
  }
  else {
    g = map->at[x];
    NullifySplit(b, 0);
    reclaim_element_FildeshKV_SINGLE_LIST(map, x);
  }
  return g;
}

/** Oversized. RMiddle. May become broadleaf.**/
static FildeshKVE rshins_2_d3_sred(FildeshKV* map, size_t y, const FildeshKVE* e) {
  const FildeshKVE g = map->at[y];
  copy_kv_FildeshKVE(&map->at[y], e);
  if (ksize_FildeshKVE_size(e->size) <= FildeshKVE_splitksize_max) {
    const size_t b = JointOf(y);
    const size_t x = SplitOf(b, 0);
    fuse_FildeshKVE(&map->at[y], 0, &map->at[b]);
    copy_kv_FildeshKVE(&map->at[b], &map->at[x]);
    NullifySplit(b, 0);
    reclaim_element_FildeshKV_SINGLE_LIST(map, x);
  }
  return g;
}
/** Oversized. LMiddle.**/
static FildeshKVE rshins_1_d3_sred(FildeshKV* map, size_t y, const FildeshKVE* e) {
  const size_t b = JointOf(y);
  const size_t z = SplitOf(b, 1);
  FildeshKVE g = map->at[b];
  g = rshins_2_d3_sred(map, z, &g);
  y = SplitOf(b, 0);
  if (Nullish(y)) {
    copy_kv_FildeshKVE(&map->at[z], e);
  }
  else {
    copy_kv_FildeshKVE(&map->at[b], e);
  }
  return g;
}
/** Oversized. Left. Shift everything.**/
static FildeshKVE rshins_0_d3_sred(FildeshKV* map, size_t y, const FildeshKVE* e) {
  const size_t b = JointOf(y);
  FildeshKVE g = map->at[y];
  g = rshins_1_d3_sred(map, y, &g);
  y = SplitOf(b, 0);
  if (Nullish(y)) {
    copy_kv_FildeshKVE(&map->at[b], e);
  }
  else {
    copy_kv_FildeshKVE(&map->at[y], e);
  }
  return g;
}
/** Standard. RMiddle.**/
static FildeshKVE rshins_2_d3_rred(FildeshKV* map, size_t y, const FildeshKVE* e) {
  FildeshKVE g = map->at[y];
  promote_splitk_FildeshKVE(&g);
  erase_splitk_FildeshKVE(&map->at[y]);
  fuse_FildeshKVE(&map->at[y], 1, e);
  return g;
}
/** Standard. LMiddle.**/
static FildeshKVE rshins_1_d3_rred(FildeshKV* map, size_t y, const FildeshKVE* e) {
  FildeshKVE g = map->at[y];
  promote_splitk_FildeshKVE(&g);
  erase_splitk_FildeshKVE(&map->at[y]);
  fuse_FildeshKVE(&map->at[y], 0, e);
  return g;
}
/** Standard. Left. Shift everything.**/
static FildeshKVE rshins_0_d3_sblack(FildeshKV* map, size_t b, const FildeshKVE* e) {
  const FildeshKVE g = rshins_1_d3_rred(map, SplitOf(b, 1), &map->at[b]);
  copy_kv_FildeshKVE(&map->at[b], e);
  return g;
}

static FildeshKVE rsh_d3(FildeshKV* map, size_t b) {
  const size_t y = SplitOf(b, 1);
  FildeshKVE g = map->at[y];
  if (IsBroadLeaf(y)) {
    promote_splitk_FildeshKVE(&g);
    erase_splitk_FildeshKVE(&map->at[y]);
    fuse_FildeshKVE(&map->at[b], 1, &map->at[y]);
    reclaim_element_FildeshKV_SINGLE_LIST(map, y);
  }
  else if (map->at[b].size <= FildeshKVE_splitksize_max) {
    const size_t x = SplitOf(b, 0);
    fuse_FildeshKVE(&map->at[b], 0, &map->at[x]);
    reclaim_element_FildeshKV_SINGLE_LIST(map, x);
  }
  else {
    const size_t x = SplitOf(b, 0);
    copy_kv_FildeshKVE(&map->at[y], &map->at[b]);
    copy_kv_FildeshKVE(&map->at[b], &map->at[x]);
    NullifySplit(b, 0);
    reclaim_element_FildeshKV_SINGLE_LIST(map, x);
  }
  return g;
}

static FildeshKVE shins_0_d3_sblack(FildeshKV* map, size_t b, const FildeshKVE* e, unsigned b_side) {
  return b_side == 1 ?
    *e :
    rshins_0_d3_sblack(map, b, e);
}
static FildeshKVE shins_0_d3_sred(FildeshKV* map, size_t y, const FildeshKVE* e, unsigned b_side) {
  return b_side == 1 ?
    *e :
    rshins_0_d3_sred(map, y, e);
}
static FildeshKVE shins_1_d3_sred(FildeshKV* map, size_t y, const FildeshKVE* e, unsigned b_side) {
  return b_side == 1 ?
    lshins_1_d3_sred(map, y, e) :
    rshins_1_d3_sred(map, y, e);
}
static FildeshKVE shins_2_d3_sred(FildeshKV* map, size_t y, const FildeshKVE* e, unsigned b_side) {
  return b_side == 1 ?
    lshins_2_d3_sred(map, y, e) :
    rshins_2_d3_sred(map, y, e);
}
static FildeshKVE shins_3_d3_sred(FildeshKV* map, size_t y, const FildeshKVE* e, unsigned b_side) {
  return b_side == 1 ?
    lshins_3_d3_sred(map, y, e) :
    *e;
}
static FildeshKVE shins_1_d3_rred(FildeshKV* map, size_t y, const FildeshKVE* e, unsigned b_side) {
  return b_side == 1 ?
    lshins_1_d3_rred(map, y, e) :
    rshins_1_d3_rred(map, y, e);
}
static FildeshKVE shins_2_d3_rred(FildeshKV* map, size_t y, const FildeshKVE* e, unsigned b_side) {
  return b_side == 1 ?
    lshins_2_d3_rred(map, y, e) :
    rshins_2_d3_rred(map, y, e);
}
static FildeshKVE shins_3_d3_rred(FildeshKV* map, size_t y, const FildeshKVE* e, unsigned b_side) {
  return b_side == 1 ?
    lshins_3_d3_rred(map, y, e) :
    *e;
}

/** Insert that involves shifting stuff around.
 *
 * It's easy if the subtree has 2 entries like:
 *      *      |   *
 *     /       |  /  Oversized keys prevent
 *    ##       | #   entries from being fused
 *  Broadleaf. |  \  into broadleaf nodes.
 *             |   +
 *
 * In that case, it just becomes a subtree with 3 entries like:
 *      *   |      *
 *     /    |     /  Oversized keys prevent
 *    #     |    #   entries from being fused
 *     \    |   / \  into broadleaf nodes.
 *     ++   |  +   +
 *
 * If adding to a 3-entry  | Otherwise, when the sibling has 3 entries as well,
 * subtree, we try to add  | we make 3 subtrees of 2 entries each. The low joint
 * to its sibling subtree. | is red and may require red-black tree rotations.
 *      *          *       |     *            *
 *     / \        / \      |    / \          / \
 *    #  ##  ->  #   #     |   #   #   ->   +  ##
 *     \          \   \    |    \   \      / \
 *     ++         ++  ++   |    ++  ++    ## ##
 * These are all broadleaf cases. The same rules apply for oversized keys.
 *
 * The general cases above cover everything but trees of 1, 3, or 4 entries.
 * 1-entry trees consist of a single black node. Simple but special nonetheless.
 * - insert_0_root1_sblack()
 * - insert_1_root1_sblack()
 * 2-entry and 3-entry trees are just jointless subtrees, but inserting
 * into a 3-entry tree is complicated by the fact that it has no sibling.
 * - insert_0_root3_sblack()
 * - insert_0_root3_sred()
 * 4-entry trees are just 3-entry subtrees with a black joint (root) node.
 * They are not valid red-black trees, but insertion creates valid 5-entry ones.
 *      #          #       |     #           *
 *       \        / \      |      \         / \    Oversized keys prevent
 *        #  ->  ## ##     |       #   ->  #   #   entries from being fused
 *         \    Broadleaf. |      / \       \   \  into broadleaf nodes.
 *         ++              |     +   +       +   +
 * This case is simplified by the general 3-entry subtree insertion logic,
 * which leaves us with the easier problem of inserting the minimal entry.
 * - insert_0_root4_sblack()
 **/
static
  FildeshKV_id_t
insert_FildeshKV_BRBTREE(FildeshKV* map, size_t y, int si, const FildeshKVE* e)
{
  FildeshKVE g, h;
  size_t b = y;
  size_t a;
  unsigned b_side;
  FildeshKV_id_t id;

  if (RedColorOf(y)) {
    b = JointOf(y);
    if (!IsBroadLeaf(y) && Nullish(SplitOf(b, 0))) {
      if (si < 0) {
        return insert_1_d2_sred(map, y, e);
      }
      return insert_2_d2_sred(map, y, e);
    }
  }
  else {
    size_t z;
    if (IsBroadLeaf(y)) {
      if (si < 0) {
        return insert_0_d2_bblack(map, y, e);
      }
      else if (si == 1) {
        return insert_1_d2_bblack(map, y, e);
      }
      return insert_2_d2_bblack(map, y, e);
    }
    if (IsLeaf(y)) {
      if (si < 0) {
        return insert_0_root1_sblack(map, y, e);
      }
      return insert_1_root1_sblack(map, y, e);
    }
    assert(si < 0);
    assert(Nullish(SplitOf(y, 0)));
    z = SplitOf(y, 1);
    if (!RedColorOf(z)) {
      assert(Nullish(JointOf(y)));
      return insert_0_root4_sblack(map, y, e);
    }
    if (!IsBroadLeaf(z)) {
      return insert_0_d2_sblack(map, y, e);
    }
  }

  a = JointOf(b);
  if (Nullish(a) || SplitOf(a, 1) == b) {
    b_side = 1;
  }
  else {
    b_side = 0;
  }

  if (RedColorOf(y)) {
    if (IsBroadLeaf(y)) {
      if (si < 0) {
        g = shins_1_d3_rred(map, y, e, b_side);
      }
      else if (si == 1) {
        g = shins_2_d3_rred(map, y, e, b_side);
      }
      else {
        g = shins_3_d3_rred(map, y, e, b_side);
      }
    }
    else if (SplitOf(b, 0) == y) {
      if (si < 0) {
        g = shins_0_d3_sred(map, y, e, b_side);
      }
      else {
        g = shins_1_d3_sred(map, y, e, b_side);
      }
    }
    else {
      if (si < 0) {
        g = shins_2_d3_sred(map, y, e, b_side);
      }
      else {
        g = shins_3_d3_sred(map, y, e, b_side);
      }
    }
  }
  else {
    g = shins_0_d3_sblack(map, y, e, b_side);
  }

  if (Nullish(a)) {
    assert(b_side == 1);
    y = SplitOf(b, 0);
    if (Nullish(y)) {
      insert_0_root3_sblack(map, b, &g);
    }
    else {
      insert_0_root3_sred(map, y, &g);
    }
    return lookup_from_FildeshKV_BRBTREE(map, b, e);
  }
  if (Nullish(SplitOf(a, 0))) {
    assert(b_side == 1);
    h = map->at[a];
    copy_kv_FildeshKVE(&map->at[a], &g);
    insert_0_root4_sblack(map, a, &h);
    return lookup_from_FildeshKV_BRBTREE(map, a, e);
  }
  if (RedColorOf(SplitOf(a, 1-b_side))) {
    h = map->at[a];
    copy_kv_FildeshKVE(&map->at[a], &g);
    y = SplitOf(a, 1-b_side);
    y = SplitOf(y, b_side);
    if (!IsLeaf(y) && !Nullish(SplitOf(y, b_side))) {
      y = SplitOf(y, b_side);
    }
    if (b_side == 0) {
      si = -1;
    }
    else if (IsBroadLeaf(y)) {
      si = 3;
    }
    else {
      si = 1;
    }
    insert_FildeshKV_BRBTREE(map, y, si, &h);
    return lookup_from_FildeshKV_BRBTREE(map, a, e);
  }
  if (detect_d2_subtree(map, SplitOf(a, 1-b_side))) {
    h = map->at[a];
    copy_kv_FildeshKVE(&map->at[a], &g);
    y = SplitOf(a, 1-b_side);
    if (IsBroadLeaf(y)) {
      if (b_side == 0) {
        insert_0_d2_bblack(map, y, &h);
      }
      else {
        insert_2_d2_bblack(map, y, &h);
      }
    }
    else {
      if (b_side == 0) {
        insert_0_d2_sblack(map, y, &h);
      }
      else {
        insert_2_d2_sred(map, SplitOf(y, 1), &h);
      }
    }
    return lookup_from_FildeshKV_BRBTREE(map, a, e);
  }

  y = empty_add_FildeshKV_BSTREE(map);
  copy_kv_FildeshKVE(&map->at[y], &map->at[a]);

  h = lsh_d3(map, SplitOf(a, 1));
  copy_kv_FildeshKVE(&map->at[a], &h);

  h = rsh_d3(map, SplitOf(a, 0));
  b = empty_add_FildeshKV_BSTREE(map);
  copy_kv_FildeshKVE(&map->at[b], &h);
  Join(b, 0, SplitOf(a, 0));
  Join(a, 0, b);
  ColorRed(b);

  Join(b, 1, y);
  if (b_side == 0) {
    insert_0_root1_sblack(map, y, &g);
  }
  else {
    insert_1_root1_sblack(map, y, &g);
  }

  id = lookup_from_FildeshKV_BRBTREE(map, a, e);
  y = fixup_insert_FildeshKV_RBTREE(map, b, id/2);
  return (2 * y) | (id & 1);
}

  FildeshKV_id_t
ensure_FildeshKV_BRBTREE(
    FildeshKV* map, const void* k, size_t ksize, FildeshAlloc* alloc)
{
  FildeshKVE e[1] = {DEFAULT_FildeshKVE};
  FildeshKV_id_t y_id;
  size_t y;
  int si;
  populate_empty_FildeshKVE(e, ksize, k, 1, 0, alloc);
  y_id = lookup_for_ensure_FildeshKV_BRBTREE(map, e);
  if (fildesh_nullid(y_id)) {
    y = empty_add_FildeshKV_BSTREE(map);
    copy_kv_FildeshKVE(&map->at[y], e);
    return 2*y;
  }
  if ((y_id & 1) == 0) {
    return y_id;
  }
  y = y_id/2;
  if (!IsLeaf(y)) {
    si = -1;
    assert(Nullish(SplitOf(y, 0)));
    assert(!RedColorOf(y));
  }
  else {
    si = cmp_FildeshKVE(e, &map->at[y]);
    if (si == 0) {return 2*y;}
    if (si == 2) {return 2*y+1;}
  }

  return insert_FildeshKV_BRBTREE(map, y, si, e);
}

  void
remove_FildeshKV_BRBTREE(FildeshKV* map, FildeshKV_id_t y_id)
{
  assert(false && "Not implemented yet.");
  (void) map;
  (void) y_id;
}

