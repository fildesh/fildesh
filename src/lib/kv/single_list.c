
#include "kv.h"
#include <stdlib.h>
#include <string.h>

static FildeshKV_id_t
first_id_FildeshKV_SINGLE_LIST(const FildeshKV*);
static FildeshKV_id_t
next_id_FildeshKV_SINGLE_LIST(const FildeshKV*, FildeshKV_id_t);
static FildeshKV_id_t
lookup_FildeshKV_SINGLE_LIST(const FildeshKV*, const void*, size_t);
static FildeshKV_id_t
ensure_FildeshKV_SINGLE_LIST(FildeshKV*, const void*, size_t, FildeshAlloc*);
static void
remove_FildeshKV_SINGLE_LIST(FildeshKV*, FildeshKV_id_t);

const FildeshKV_VTable DEFAULT_SINGLE_LIST_FildeshKV_VTable = {
  first_id_FildeshKV_SINGLE_LIST,
  next_id_FildeshKV_SINGLE_LIST,
  lookup_FildeshKV_SINGLE_LIST,
  ensure_FildeshKV_SINGLE_LIST,
  remove_FildeshKV_SINGLE_LIST,
};

  FildeshKV_id_t
first_id_FildeshKV_SINGLE_LIST(const FildeshKV* map) {
  if (map->freelist_head == 0) {
    return FildeshKV_NULL_ID;
  }
  return 0;
}

  FildeshKV_id_t
next_id_FildeshKV_SINGLE_LIST(const FildeshKV* map, FildeshKV_id_t id) {
  size_t i = id/2;
  assert(!fildesh_nullid(id));
  if (((id & 1) == 0) && splitkexists_FildeshKVE(&map->at[i])) {
    return id + 1;
  }
  i = get_index_FildeshKVE_joint(map->at[i].joint);
  if (i == FildeshKV_NULL_INDEX) {
    return FildeshKV_NULL_ID;
  }
  return 2*i;
}

  FildeshKV_id_t
lookup_FildeshKV_SINGLE_LIST(const FildeshKV* map, const void* k, size_t ksize)
{
  size_t d = 1+fildesh_size_of_lgcount(1, map->allocated_lgcount);
  size_t i;
  if (map->freelist_head == 0) {
    return FildeshKV_NULL_ID;
  }
  assert(map->allocated_lgcount > 0);
  for (i = 0;
       i != FildeshKV_NULL_INDEX && d > 0;
       i = get_index_FildeshKVE_joint(map->at[i].joint))
  {
    const FildeshKVE* const e = &map->at[i];
    if (0 == cmp_k_FildeshKVE(e, ksize, k)) {
      return 2*i;
    }
    if (splitkexists_FildeshKVE(e)) {
      if (0 == cmp_splitk_FildeshKVE(e, ksize, k)) {
        return 2*i+1;
      }
    }
    d -= 1;
  }
  assert(d > 0 && "List should not have a cycle.");
  return FildeshKV_NULL_ID;
}

  void
maybe_grow_FildeshKV_SINGLE_LIST(FildeshKV* map)
{
  size_t allocated_count = fildesh_size_of_lgcount(1, map->allocated_lgcount);
  assert(map->freelist_head <= allocated_count);
  if (map->freelist_head == allocated_count) {
    size_t i;
    grow_FildeshA_(
        (void**)&map->at, &allocated_count, &map->allocated_lgcount,
        sizeof(FildeshKVE), 1);

    allocated_count = fildesh_size_of_lgcount(1, map->allocated_lgcount);
    assert(map->freelist_head < allocated_count);

    for (i = map->freelist_head; i < allocated_count; ++i) {
      map->at[i] = default_FildeshKVE();
      map->at[i].joint = i+1;
    }
  }
}

static
  FildeshKV_id_t
add_FildeshKV_SINLGE_LIST(
    FildeshKV* map, const void* k, size_t ksize, FildeshAlloc* alloc)
{
  if (map->freelist_head > 0 &&
      !splitkexists_FildeshKVE(&map->at[0]))
  {
    if (maybe_populate_splitkv_FildeshKVE(
            &map->at[0], ksize, k, 1, NULL, alloc)) {
      return 1;
    }
  }

  maybe_grow_FildeshKV_SINGLE_LIST(map);

  if (map->freelist_head == 0) {
    assert_trivial_joint(map->at[0].joint);
    map->freelist_head = map->at[0].joint;
    map->at[0] = default_FildeshKVE();
    map->at[0].joint = FildeshKV_NULL_INDEX;
  }
  else {
    const size_t i = map->freelist_head;
    assert_trivial_joint(map->at[i].joint);
    map->freelist_head = map->at[i].joint;
    assert(map->freelist_head != i);
    map->at[i] = map->at[0];
    map->at[0] = default_FildeshKVE();
    map->at[0].joint = i;
  }
  populate_empty_FildeshKVE(&map->at[0], ksize, k, 1, 0, alloc);
  return 0;
}

  FildeshKV_id_t
ensure_FildeshKV_SINGLE_LIST(
    FildeshKV* map, const void* k, size_t ksize, FildeshAlloc* alloc)
{
  FildeshKV_id_t id = lookup_FildeshKV_SINGLE_LIST(map, k, ksize);
  if (!fildesh_nullid(id)) {return id;}
  return add_FildeshKV_SINLGE_LIST(map, k, ksize, alloc);
}

  void
reclaim_element_FildeshKV_SINGLE_LIST(FildeshKV* map, size_t di)
{
  assert_trivial_joint(map->freelist_head);
  map->at[di] = default_FildeshKVE();
  map->at[di].joint = map->freelist_head;
  map->freelist_head = di;
}

void remove_FildeshKV_SINGLE_LIST(FildeshKV* map, FildeshKV_id_t id) {
  const size_t ei = id/2;  /* Element index.*/
  FildeshKVE* const e = &map->at[ei];
  size_t i, j, di;

  assert(!fildesh_nullid(id));
  assert(ei < fildesh_size_of_lgcount(1, map->allocated_lgcount));
  assert(0 != get_index_FildeshKVE_joint(e->joint));

  /* Cases: Element holds 2 entries. No need to delete.*/
  if (1 == (id & 1)) {
    erase_splitk_FildeshKVE(e);
    return;
  }
  if (splitkexists_FildeshKVE(e)) {
    promote_splitk_FildeshKVE(e);
    return;
  }

  /* Case: A next element exists.*/
  di = get_index_FildeshKVE_joint(e->joint);
  if (di != FildeshKV_NULL_INDEX) {
    *e = map->at[di];
    reclaim_element_FildeshKV_SINGLE_LIST(map, di);
    return;
  }

  /* Case: Removing first element in map.*/
  if (ei == 0) {
    reclaim_element_FildeshKV_SINGLE_LIST(map, 0);
    return;
  }

  /* Case: Removing second element in map.*/
  i = get_index_FildeshKVE_joint(map->at[0].joint);
  if (ei == i) {
    set_joint_index_FildeshKVE(&map->at[0], FildeshKV_NULL_INDEX);
    reclaim_element_FildeshKV_SINGLE_LIST(map, ei);
    return;
  }

  /* Case: Removing third element in map.*/
  j = get_index_FildeshKVE_joint(map->at[i].joint);
  if (ei == j) {
    set_joint_index_FildeshKVE(&map->at[i], FildeshKV_NULL_INDEX);
    reclaim_element_FildeshKV_SINGLE_LIST(map, ei);
    return;
  }

  /* Case: Removing fourth or later element in map.*/
  set_joint_index_FildeshKVE(&map->at[0], j);
  set_joint_index_FildeshKVE(&map->at[i], FildeshKV_NULL_INDEX);
  *e = map->at[i];
  reclaim_element_FildeshKV_SINGLE_LIST(map, i);
}

