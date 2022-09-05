
#include "kve.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>

static
  size_t
lookup_id_FildeshKV(const FildeshKV* map, const void* k, size_t ksize)
{
  const FildeshKVE* e = NULL;
  const size_t allocated_count =
    fildesh_size_of_lgcount(1, map->allocated_lgcount);
  size_t i;
  if (!map->at) {return 0;}
  assert(map->allocated_lgcount > 0);
  for (i = 0; i < allocated_count; i = get_index_FildeshKVE_joint(e->joint)) {
    e = &map->at[i];
    if (0 == cmp_FildeshKVE_(ksize, k, e->joint, e->size, e->kv)) {
      return 2*i;
    }
    if (0 != get_splitkexists_bit_FildeshKVE_size(e->size)) {
      if (0 == cmp_split_FildeshKVE_(ksize, k, e->size, e->split)) {
        return 2*i+1;
      }
    }
  }
  return 2*map->freelist_head;
}

static
  const void*
add_FildeshKV(
    FildeshKV* map,
    const void* k, size_t ksize, const void* v, size_t vsize)
{
  size_t allocated_count = fildesh_size_of_lgcount(1, map->allocated_lgcount);
  if (map->at && 0 == get_splitkexists_bit_FildeshKVE_size(map->at[0].size)) {
    if (populate_splitkv_FildeshKVE(&map->at[0], ksize, k, vsize, v)) {
      return splitvalue_FildeshKVE(&map->at[0]);
    }
  }
  if (map->freelist_head >= allocated_count) {
    size_t i = allocated_count;
    grow_FildeshA_(
        (void**)&map->at, &allocated_count, &map->allocated_lgcount,
        sizeof(FildeshKVE), 1, realloc);
    allocated_count = fildesh_size_of_lgcount(1, map->allocated_lgcount);
    assert(i < allocated_count);
    memset(&map->at[i], 0, sizeof(FildeshKVE)*(allocated_count-i));
    map->freelist_head = i;
    for (; i < allocated_count; ++i) {
      map->at[i].joint = i+1;
    }
  }

  if (map->freelist_head == 0) {
    map->freelist_head = map->at[0].joint;
    get_index_FildeshKVE_joint(map->at[0].joint);
    map->at[0] = default_FildeshKVE();
    map->at[0].joint = get_index_FildeshKVE_joint(~(size_t)0);
  }
  else {
    const size_t i = map->freelist_head;
    map->freelist_head = map->at[i].joint;
    assert(map->freelist_head != i);
    map->at[i] = map->at[0];
    map->at[0] = default_FildeshKVE();
    map->at[0].joint = i;
  }
  populate_empty_FildeshKVE(&map->at[0], ksize, k, vsize, v);
  return value_FildeshKVE(&map->at[0]);
}

  void*
lookup_value_FildeshKV(FildeshKV* map, const void* k, size_t ksize)
{
  return (void*) lookup_const_FildeshKV(map, k, ksize);
}

  const void*
lookup_const_FildeshKV(const FildeshKV* map, const void* k, size_t ksize)
{
  size_t id = lookup_id_FildeshKV(map, k, ksize);
  const FildeshKVE* e;
  if (id == 2*map->freelist_head) {return NULL;}
  e = &map->at[id/2];
  if (0 == (id & 1)) {
    return value_FildeshKVE(e);
  }
  return splitvalue_FildeshKVE(e);
}

  void*
ensure_v_FildeshKV(
    FildeshKV* map,
    const void* k, size_t ksize,
    void* v, size_t vsize)
{
  size_t id = lookup_id_FildeshKV(map, k, ksize);
  if (id != 2*map->freelist_head) {return NULL;}
  return (void*) add_FildeshKV(map, k, ksize, v, vsize);
}

  void*
replace_v_FildeshKV(
    FildeshKV* map,
    const void* k, size_t ksize,
    void* v, size_t vsize)
{
  size_t id = lookup_id_FildeshKV(map, k, ksize);
  if (id != 2*map->freelist_head) {
    if (0 == (id & 1)) {
      return (void*) replace_v_FildeshKVE(&map->at[id/2], vsize, v);
    }
    else {
      return (void*) replace_splitv_FildeshKVE(&map->at[id/2], vsize, v);
    }
  }
  return (void*) add_FildeshKV(map, k, ksize, v, vsize);
}

static
  void
del_index_FildeshKV(FildeshKV* map, size_t i, size_t p)
{
  const size_t allocated_count =
    fildesh_size_of_lgcount(1, map->allocated_lgcount);
  if (i == 0) {
    i = get_index_FildeshKVE_joint(map->at[0].joint);
    if (i >= allocated_count) {
      i = 0;
    }
    else {
      map->at[0] = map->at[i];
    }
  }
  else {
    set_joint_index_FildesKVE(&map->at[p], map->at[i].joint);
  }
  map->at[i] = default_FildeshKVE();
  map->at[i].joint = map->freelist_head;
  map->freelist_head = i;
}

  bool
del_FildeshKV(FildeshKV* map, const void* k, size_t ksize)
{
  FildeshKVE* e = NULL;
  const size_t allocated_count =
    fildesh_size_of_lgcount(1, map->allocated_lgcount);
  size_t i, p;
  if (!map->at) {return 0;}
  assert(map->allocated_lgcount > 0);
  p = 0;
  for (i = 0; i < allocated_count; i = get_index_FildeshKVE_joint(e->joint)) {
    e = &map->at[i];
    if (0 == cmp_FildeshKVE_(ksize, k, e->joint, e->size, e->kv)) {
      if (0 != get_splitkexists_bit_FildeshKVE_size(e->size)) {
        promote_splitk_FildeshKVE(e);
      }
      else {
        del_index_FildeshKV(map, i, p);
      }
      return true;
    }
    if (0 != get_splitkexists_bit_FildeshKVE_size(e->size)) {
      if (0 == cmp_split_FildeshKVE_(ksize, k, e->size, e->split)) {
        erase_splitk_FildeshKVE(e);
        return true;
      }
    }
    p = i;
  }
  return false;
}

void close_FildeshKV(FildeshKV* map)
{
  if (map->at) {free(map->at);}
}

