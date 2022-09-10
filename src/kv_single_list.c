
#include "kve.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>

static const FildeshKV_id_t FildeshKV_NULL_ID = ~(size_t)0;
static const FildeshKV_id_t FildeshKV_NULL_INDEX = ~(size_t)0 >> 3;

#define assert_trivial_joint(joint)  assert(joint == get_index_FildeshKVE_joint(joint))

  FildeshKV_id_t
lookup_FildeshKV(const FildeshKV* map, const void* k, size_t ksize)
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
    if (0 == cmp_FildeshKVE_(ksize, k, e->joint, e->size, e->kv)) {
      return 2*i;
    }
    if (0 != get_splitkexists_bit_FildeshKVE_size(e->size)) {
      if (0 == cmp_split_FildeshKVE_(ksize, k, e->size, e->split)) {
        return 2*i+1;
      }
    }
    d -= 1;
  }
  assert(d > 0 && "List should not have a cycle.");
  return FildeshKV_NULL_ID;
}

FildeshKV_id_t any_id_FildeshKV(const FildeshKV* map) {
  if (map->freelist_head == 0) {
    return FildeshKV_NULL_ID;
  }
  return 0;
}

  void*
lookup_value_FildeshKV(FildeshKV* map, const void* k, size_t ksize)
{
  FildeshKV_id_t id = lookup_FildeshKV(map, k, ksize);
  FildeshKVE* e;
  if (id == FildeshKV_NULL_ID) {return NULL;}
  e = &map->at[id/2];
  if (0 == (id & 1)) {
    return (void*) value_FildeshKVE(e);
  }
  return (void*) splitvalue_FildeshKVE(e);
}


static
  FildeshKV_id_t
add_FildeshKV(FildeshKV* map, const void* k, size_t ksize)
{
  size_t allocated_count = fildesh_size_of_lgcount(1, map->allocated_lgcount);
  if (map->freelist_head > 0 &&
      0 == get_splitkexists_bit_FildeshKVE_size(map->at[0].size))
  {
    if (populate_splitkv_FildeshKVE(&map->at[0], ksize, k, 1, NULL)) {
      return 1;
    }
  }
  assert(map->freelist_head <= allocated_count);
  if (map->freelist_head == allocated_count) {
    size_t i;
    grow_FildeshA_(
        (void**)&map->at, &allocated_count, &map->allocated_lgcount,
        sizeof(FildeshKVE), 1, realloc);

    allocated_count = fildesh_size_of_lgcount(1, map->allocated_lgcount);
    assert(map->freelist_head < allocated_count);

    for (i = map->freelist_head; i < allocated_count; ++i) {
      map->at[i] = default_FildeshKVE();
      map->at[i].joint = i+1;
    }
  }

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
  populate_empty_FildeshKVE(&map->at[0], ksize, k, 1, 0);
  return 0;
}

  FildeshKV_id_t
ensure_FildeshKV(FildeshKV* map, const void* k, size_t ksize)
{
  FildeshKV_id_t id = lookup_FildeshKV(map, k, ksize);
  if (!fildesh_nullid(id)) {return id;}
  return add_FildeshKV(map, k, ksize);
}

size_t size_of_key_at_FildeshKV(const FildeshKV* map, FildeshKV_id_t id) {
  const FildeshKVE* e;
  if (fildesh_nullid(id)) {return 0;}
  e = &map->at[id/2];
  if (0 == (id & 1)) {
    return ksize_FildeshKVE_size(e->size);
  }
  return splitksize_FildeshKVE_size(e->size);
}

const void* key_at_FildeshKV(const FildeshKV* map, FildeshKV_id_t id) {
  const FildeshKVE* e;
  if (fildesh_nullid(id)) {return NULL;}
  e = &map->at[id/2];
  if (0 == (id & 1)) {
    if (kdirect_FildeshKVE(e)) {
      return direct_k_FildeshKVE_kv(e->kv);
    }
    return indirect_k_FildeshKVE_kv(e->kv);
  }
  if (splitkdirect_FildeshKVE(e)) {
    return direct_splitk_FildeshKVE_split(e->split);
  }
  return indirect_splitk_FildeshKVE_split(e->split);
}

const void* value_at_FildeshKV(const FildeshKV* map, FildeshKV_id_t id) {
  const FildeshKVE* e;
  if (fildesh_nullid(id)) {return NULL;}
  e = &map->at[id/2];
  if (0 == (id & 1)) {
    return value_FildeshKVE(e);
  }
  return splitvalue_FildeshKVE(e);
}

  void
assign_at_FildeshKV(FildeshKV* map, FildeshKV_id_t id, const void* v, size_t vsize)
{
  FildeshKVE* e;
  assert(!fildesh_nullid(id));
  e = &map->at[id/2];
  if (0 == (id & 1)) {
    assert(0 != get_vexists_bit_FildeshKVE_joint(e->joint));
    if (v && vsize <= sizeof(e->kv[1])) {
      set1_vdirect_bit_FildeshKVE(e);
      memcpy(&e->kv[1], v, vsize);
    } else {
      set0_vdirect_bit_FildeshKVE(e);
      e->kv[1] = (uintptr_t) v;
    }
  }
  else {
    assert(0 != get_splitvexists_bit_FildeshKVE_size(e->size));
    if (v && vsize <= sizeof(e->split[1])) {
      set1_splitvdirect_bit_FildeshKVE(e);
      memcpy(&e->split[1], v, vsize);
    } else {
      set0_vdirect_bit_FildeshKVE(e);
      e->split[1] = (uintptr_t) v;
    }
  }
}

static void reclaim_element_FildeshKV(FildeshKV* map, size_t di) {
  assert_trivial_joint(map->freelist_head);
  map->at[di] = default_FildeshKVE();
  map->at[di].joint = map->freelist_head;
  map->freelist_head = di;
}

void remove_at_FildeshKV(FildeshKV* map, FildeshKV_id_t id) {
  const size_t allocated_count =
    fildesh_size_of_lgcount(1, map->allocated_lgcount);
  const size_t ei = id/2;  /* Element index.*/
  FildeshKVE* const e = &map->at[ei];
  size_t i, j, di;

  assert(!fildesh_nullid(id));
  assert(ei < allocated_count);
  assert(0 != get_index_FildeshKVE_joint(e->joint));

  /* Cases: Element holds 2 entries. No need to delete.*/
  if (1 == (id & 1)) {
    erase_splitk_FildeshKVE(e);
    return;
  }
  if (0 != get_splitkexists_bit_FildeshKVE_size(e->size)) {
    promote_splitk_FildeshKVE(e);
    return;
  }

  /* Case: A next element exists.*/
  di = get_index_FildeshKVE_joint(e->joint);
  if (di != FildeshKV_NULL_INDEX) {
    *e = map->at[di];
    reclaim_element_FildeshKV(map, di);
    return;
  }

  /* Case: Removing first element in map.*/
  if (ei == 0) {
    reclaim_element_FildeshKV(map, 0);
    return;
  }

  /* Case: Removing second element in map.*/
  i = get_index_FildeshKVE_joint(map->at[0].joint);
  if (ei == i) {
    set_joint_index_FildeshKVE(&map->at[0], FildeshKV_NULL_INDEX);
    reclaim_element_FildeshKV(map, ei);
    return;
  }

  /* Case: Removing third element in map.*/
  j = get_index_FildeshKVE_joint(map->at[i].joint);
  if (ei == j) {
    set_joint_index_FildeshKVE(&map->at[i], FildeshKV_NULL_INDEX);
    reclaim_element_FildeshKV(map, ei);
    return;
  }

  /* Case: Removing fourth or later element in map.*/
  set_joint_index_FildeshKVE(&map->at[0], j);
  set_joint_index_FildeshKVE(&map->at[i], FildeshKV_NULL_INDEX);
  *e = map->at[i];
  reclaim_element_FildeshKV(map, i);
}

void close_FildeshKV(FildeshKV* map)
{
  if (map->at) {free(map->at);}
}

