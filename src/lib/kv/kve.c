
#include "kve.h"
#include <assert.h>
#include <string.h>

  void
assign_v_FildeshKVE(FildeshKVE* e, size_t vsize, const void* v, FildeshAlloc* alloc)
{
  assert(0 != get_vexists_bit_FildeshKVE_joint(e->joint));
  if (v && vsize <= sizeof(e->kv[1])) {
    memcpy(&e->kv[1], v, vsize);
    set0_vrefers_bit_FildeshKVE(e);
    return;
  }
  set1_vrefers_bit_FildeshKVE(e);
  if (v && alloc) {
    void* p = reserve_FildeshAlloc(alloc, vsize, (vsize & -vsize));
    memcpy(p, v, vsize);
    e->kv[1] = (uintptr_t) p;
  }
  else {
    e->kv[1] = (uintptr_t) v;
  }
}

  void
assign_splitv_FildeshKVE(FildeshKVE* e, size_t vsize, const void* v, FildeshAlloc* alloc)
{
  assert(0 != get_splitvexists_bit_FildeshKVE_size(e->size));
  if (v && vsize <= sizeof(e->split[1])) {
    memcpy(&e->split[1], v, vsize);
    set0_splitvrefers_bit_FildeshKVE(e);
    return;
  }
  set1_splitvrefers_bit_FildeshKVE(e);
  if (v && alloc) {
    void* p = reserve_FildeshAlloc(alloc, vsize, (vsize & -vsize));
    memcpy(p, v, vsize);
    e->split[1] = (uintptr_t) p;
  }
  else {
    e->split[1] = (uintptr_t) v;
  }
}

/** Assuming the node is empty, populate it with a key and value.
 *
 * This only overwrites data, leaving the node's connectivity intact.
 **/
  void
populate_empty_FildeshKVE(FildeshKVE* e,
                          size_t ksize, const void* k,
                          size_t vsize, const void* v,
                          FildeshAlloc* alloc)
{
  bool kdirect;
  /* Assuming no split data, so we can clear e->size.*/
  e->size = ksize;

  if (vsize > 0) {
    set1_vexists_bit_FildeshKVE(e);
    assign_v_FildeshKVE(e, vsize, v, alloc);
    kdirect = (ksize <= sizeof(e->kv[0]));
  }
  else {
    set0_vexists_bit_FildeshKVE(e);
    set1_vrefers_bit_FildeshKVE(e);
    kdirect = (ksize <= sizeof(e->kv[0]) + sizeof(e->kv[1]));
  }
  if (kdirect) {
    memcpy(&e->kv[0], k, ksize);
  }
  else if (alloc) {
    void* p = reserve_FildeshAlloc(alloc, ksize, (ksize & -ksize));
    memcpy(p, k, ksize);
    e->kv[0] = (uintptr_t)p;
  }
  else {
    e->kv[0] = (uintptr_t)k;
  }
}

static
  bool
populate_splitk_FildeshKVE_size(size_t* p_size, size_t ksize, bool vexists)
{
  size_t e_size = *p_size;
  if (0 != (e_size >> FildeshKVE_splitk_lgsize_max)) {
    return false;
  }
  if (0 != (ksize >> FildeshKVE_splitk_lgsize_max)) {
    return false;
  }

  e_size |= (ksize << FildeshKVE_splitk_lgsize_max);
  if (vexists) {
    e_size |= splitvexists_bit_FildeshKVE_size();
    e_size &= ~splitvrefers_bit_FildeshKVE_size();
  }
  else {
    e_size &= ~splitvexists_bit_FildeshKVE_size();
    e_size |= splitvrefers_bit_FildeshKVE_size();
  }
  *p_size = e_size;
  return true;
}

  bool
maybe_populate_splitkv_FildeshKVE(
    FildeshKVE* e,
    size_t ksize, const void* k,
    size_t vsize, const void* v,
    FildeshAlloc* alloc)
{
  const bool vexists = (vsize > 0);
  bool kdirect;
  assert(!splitkexists_FildeshKVE(e));
  if (!populate_splitk_FildeshKVE_size(&e->size, ksize, vexists)) {
    return false;
  }
  assert(splitkexists_FildeshKVE(e));

  if (vexists) {
    assign_splitv_FildeshKVE(e, vsize, v, alloc);
    kdirect = (ksize <= sizeof(e->split[0]));
  }
  else {
    kdirect = (ksize <= sizeof(e->split[0]) + sizeof(e->split[1]));
  }
  if (kdirect) {
    memcpy(&e->split[0], k, ksize);
  }
  else if (alloc) {
    void* p = reserve_FildeshAlloc(alloc, ksize, (ksize & -ksize));
    memcpy(p, k, ksize);
    e->split[0] = (uintptr_t)p;
  }
  else {
    e->split[0] = (uintptr_t)k;
  }
  return true;
}

  void
populate_splitkv_FildeshKVE(
    FildeshKVE* e,
    size_t ksize, const void* k,
    size_t vsize, const void* v,
    FildeshAlloc* alloc)
{
  if (!maybe_populate_splitkv_FildeshKVE(e, ksize, k, vsize, v, alloc)) {
    assert(false);
  }
}

  void
fuse_FildeshKVE(FildeshKVE* dst, unsigned side, const FildeshKVE* src)
{
  assert(!splitkexists_FildeshKVE(dst));
  assert(!splitkexists_FildeshKVE(src));
  assert(dst->size <= FildeshKVE_splitksize_max);
  assert(src->size <= FildeshKVE_splitksize_max);

  if (side == 0) {
    const size_t vmask = (
        vexists_bit_FildeshKVE_joint() | vrefers_bit_FildeshKVE_joint());
    size_t e_size = src->size;
    const bool dst_vexists = (0 != get_vexists_bit_FildeshKVE_joint(dst->joint));
    populate_splitk_FildeshKVE_size(&e_size, dst->size, dst_vexists);
    dst->size = (e_size | (dst->joint & vrefers_bit_FildeshKVE_joint()));
    dst->joint = (dst->joint & ~vmask) | (src->joint & vmask);
    dst->split[0] = dst->kv[0];
    dst->split[1] = dst->kv[1];
    dst->kv[0] = src->kv[0];
    dst->kv[1] = src->kv[1];
  }
  else {
    size_t e_size = dst->size;
    const bool src_vexists = (0 != get_vexists_bit_FildeshKVE_joint(src->joint));
    populate_splitk_FildeshKVE_size(&e_size, src->size, src_vexists);
    dst->size = (e_size | (src->joint & vrefers_bit_FildeshKVE_joint()));
    dst->split[0] = src->kv[0];
    dst->split[1] = src->kv[1];
  }
}

  void
erase_k_FildeshKVE(FildeshKVE* e)
{
  assert(kexists_FildeshKVE(e));
  assert(!splitkexists_FildeshKVE(e));
  e->kv[0] = FildeshKV_NULL_INDEX;
  e->kv[1] = FildeshKV_NULL_INDEX;
  e->joint = get_index_FildeshKVE_joint(e->joint);
}

  void
erase_splitk_FildeshKVE(FildeshKVE* e)
{
  assert(splitkexists_FildeshKVE(e));
  e->split[0] = FildeshKV_NULL_INDEX;
  e->split[1] = FildeshKV_NULL_INDEX;
  e->size = ksize_FildeshKVE_size(e->size);
}

  void
promote_splitk_FildeshKVE(FildeshKVE* e)
{
  assert(splitkexists_FildeshKVE(e));
  {
    /* The exists and direct bits are consistent across `joint` and `size`.*/
    const size_t splitvbits = (splitvexists_bit_FildeshKVE_size() |
                               splitvrefers_bit_FildeshKVE_size());
    e->joint &= ~splitvbits;
    e->joint |= (splitvbits & e->size);
  }
  e->size = splitksize_FildeshKVE_size(e->size);
  e->kv[0] = e->split[0];
  e->kv[1] = e->split[1];
  e->split[0] = FildeshKV_NULL_INDEX;
  e->split[1] = FildeshKV_NULL_INDEX;
}

  int
cmp_k_FildeshKVE(const FildeshKVE* e, size_t keysize, const void* key)
{
  const size_t actual_size = ksize_FildeshKVE_size(e->size);
  if (keysize != actual_size) {
    return (actual_size < keysize ? -1 : 1);
  }
  if (kdirect_FildeshKVE_joint(e->joint, actual_size)) {
    return memcmp(direct_k_FildeshKVE_kv(e->kv), key, actual_size);
  }
  if (key == (const void*)e->kv[0]) {return 0;}
  return memcmp(indirect_k_FildeshKVE_kv(e->kv), key, actual_size);
}

  int
cmp_splitk_FildeshKVE(const FildeshKVE* e, size_t keysize, const void* key)
{
  const size_t actual_size = splitksize_FildeshKVE_size(e->size);
  if (keysize != actual_size) {
    return (actual_size < keysize ? -1 : 1);
  }
  if (splitkdirect_FildeshKVE_size(e->size, actual_size)) {
    return memcmp(direct_splitk_FildeshKVE_split(e->split), key, actual_size);
  }
  if (key == (const void*)e->split[0]) {return 0;}
  return memcmp(indirect_splitk_FildeshKVE_split(e->split), key, actual_size);
}

  int
cmp_FildeshKVE(const FildeshKVE* e, const FildeshKVE* g)
{
  const size_t e_size = ksize_FildeshKVE_size(e->size);
  const size_t g_size = ksize_FildeshKVE_size(g->size);
  const void* const e_key = kdirect_FildeshKVE(e) ?
    direct_k_FildeshKVE_kv(e->kv) : indirect_k_FildeshKVE_kv(e->kv);
  assert(!splitkexists_FildeshKVE(e));
  if (e_size < g_size) {
    return -1;
  }
  if (e_size == g_size) {
    const void* const g_key = kdirect_FildeshKVE(g) ?
      direct_k_FildeshKVE_kv(g->kv) : indirect_k_FildeshKVE_kv(g->kv);
    int si = (e_key == g_key ? 0 : memcmp(e_key, g_key, e_size));
    if (si <= 0) {return si;}
  }
  if (splitkexists_FildeshKVE(g)) {
    const size_t h_size = splitksize_FildeshKVE_size(g->size);
    if (e_size < h_size) {return 1;}
    if (e_size == h_size) {
      const void* const h_key = splitkdirect_FildeshKVE(g) ?
        direct_splitk_FildeshKVE_split(g->split) :
        indirect_splitk_FildeshKVE_split(g->split);
      int si = (e_key == h_key ? 0 : memcmp(e_key, h_key, e_size));
      if (si < 0) {return 1;}
      if (si == 0) {return 2;}
    }
    return 3;
  }
  return 1;
}
