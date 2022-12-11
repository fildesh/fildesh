#include "kv.h"
#include <stdlib.h>
#include <string.h>

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
    assign_v_FildeshKVE(e, vsize, v, map->alloc);
  }
  else {
    assign_splitv_FildeshKVE(e, vsize, v, map->alloc);
  }
}

  void
assign_memref_at_FildeshKV(FildeshKV* map, FildeshKV_id_t id, const void* v)
{
  FildeshKVE* e;
  assert(!fildesh_nullid(id));
  e = &map->at[id/2];
  if (0 == (id & 1)) {
    assert(0 != get_vexists_bit_FildeshKVE_joint(e->joint));
    assign_memref_v_FildeshKVE(e, v);
  }
  else {
    assert(0 != get_splitvexists_bit_FildeshKVE_size(e->size));
    assign_memref_splitv_FildeshKVE(e, v);
  }
}

