/* #define FILDESH_LOG_TRACE_ON 1 */

#include "kv.h"

#define Nullish(x)  (x == FildeshKV_NULL_INDEX)

static inline size_t joint_of(const FildeshKV* map, size_t x) {
  assert(!Nullish(x));
  return get_index_FildeshKVE_joint(map->at[x].joint);
}
#define JointOf(x)  joint_of(map, x)

static inline void assign_joint(const FildeshKV* map, size_t x, size_t y) {
  assert(!Nullish(x));
  set_joint_index_FildeshKVE(&map->at[x], y);
}
#define AssignJoint(x, y)  assign_joint(map, x, y)
#define NullifyJoint(x)  nullify_joint_index_FildeshKVE(&map->at[x])
static inline void maybe_assign_joint(const FildeshKV* map, size_t x, size_t y) {
  if (Nullish(x)) {return;}
  AssignJoint(x, y);
}
#define MaybeAssignJoint(x, y)  maybe_assign_joint(map, x, y)

#define IsBroadLeaf(x)  splitkexists_FildeshKVE(&map->at[x])

static inline size_t split_of(const FildeshKV* map, size_t y, unsigned side) {
  assert(!Nullish(y));
  assert(!IsBroadLeaf(y));
  return map->at[y].split[side];
}
#define SplitOf(y, side)  split_of(map, y, side)

static inline void assign_split(FildeshKV* map, size_t y, unsigned side, size_t x) {
  assert(!Nullish(y));
  assert(!IsBroadLeaf(y));
  map->at[y].split[side] = x;
}
#define AssignSplit(y, side, x)  assign_split(map, y, side, x)
#define NullifySplit(x, side)  AssignSplit(x, side, FildeshKV_NULL_INDEX)
static inline void maybe_assign_split(FildeshKV* map, size_t y, unsigned side, size_t x) {
  if (Nullish(y)) {return;}
  AssignSplit(y, side, x);
}
#define MaybeAssignSplit(y, side, x)  maybe_assign_split(map, y, side, x)

static inline bool is_root(const FildeshKV* map, size_t i) {
  return FildeshKV_NULL_INDEX == JointOf(i);
}
#define IsRoot(i)  is_root(map, i)

static inline bool is_leaf(const FildeshKV* map, size_t i) {
  if (IsBroadLeaf(i)) {
    return true;
  }
  return (Nullish(SplitOf(i, 0)) && Nullish(SplitOf(i, 1)));
}
#define IsLeaf(i)  is_leaf(map, i)


static inline void join(FildeshKV* map, size_t y, unsigned side, size_t x) {
  AssignSplit(y, side, x);
  MaybeAssignJoint(x, y);
}
#define Join(y, side, x)  join(map, y, side, x)

static inline unsigned side_of(const FildeshKV* map, size_t x) {
  if (JointOf(x) == FildeshKV_NULL_INDEX) {
    return 0;
  }
  return (x == SplitOf(JointOf(x), 1)) ? 1 : 0;
}
#define SideOf(x)  side_of(map, x)

static inline void local_swap(FildeshKV* map, size_t* p_y, size_t* p_x) {
  const size_t y = *p_y;
  const size_t x = *p_x;
  FildeshKVE e = map->at[y];
  map->at[y] = map->at[x];
  map->at[x] = e;
  *p_y = x;
  *p_x = y;
}
#define LocalSwap(p_y, p_x)  local_swap(map, p_y, p_x)

static inline void sub_join(FildeshKV* map, size_t* p_y, size_t* p_x) {
  const size_t y = *p_y;
  const size_t x = *p_x;
  const size_t b = JointOf(y);

  if (!IsBroadLeaf(y)) {
    NullifySplit(y, 0);
    NullifySplit(y, 1);
  }
  NullifyJoint(y);

  AssignJoint(x, b);
  if (Nullish(b)) {
    local_swap(map, p_y, p_x);
  }
  else if (y == SplitOf(b, 1)) {
    AssignSplit(b, 1, x);
  }
  else {
    AssignSplit(b, 0, x);
  }
}
#define SubJoin(p_y, p_x) sub_join(map, p_y, p_x)

/* For red-black tree only.*/
#define RedColorOf(x)  red_FildeshKVE(&map->at[x])
#define ColorBlack(x)  set0_red_bit_FildeshKVE(&map->at[x])
#define ColorRed(x)  set1_red_bit_FildeshKVE(&map->at[x])
#define ColorSwap(x, y)  swap_red_bit_FildeshKVE(&map->at[x], &map->at[y])
#define RotateUp(p_a)  rotate_up_FildeshKV_BSTREE(map, p_a)

static inline
  size_t
empty_add_FildeshKV_BSTREE(FildeshKV* map)
{
  size_t x;
  maybe_grow_FildeshKV_SINGLE_LIST(map);
  x = map->freelist_head;
  assert_trivial_joint(map->at[x].joint);
  map->freelist_head = map->at[x].joint;
  map->at[x] = default_FildeshKVE();
  return x;
}

BEGIN_EXTERN_C

FildeshKV_id_t first_id_FildeshKV_BSTREE(const FildeshKV*);
FildeshKV_id_t next_id_FildeshKV_BSTREE(const FildeshKV*, FildeshKV_id_t);
FildeshKV_id_t lookup_FildeshKV_BSTREE(const FildeshKV*, const void*, size_t);
FildeshKV_id_t ensure_FildeshKV_BSTREE(FildeshKV*, const void*, size_t, FildeshAlloc*);
size_t premove_FildeshKV_BSTREE(FildeshKV*, FildeshKV_id_t);
void rotate_up_FildeshKV_BSTREE(FildeshKV*, size_t*);

END_EXTERN_C
