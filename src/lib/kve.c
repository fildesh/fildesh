
#include "kve.h"
#include <assert.h>
#include <string.h>

#define lo_lgksize (sizeof(size_t)*CHAR_BIT/2-2)
#define hi_lgsplitksize (sizeof(size_t)*CHAR_BIT/2-1)

static size_t lo_mask_ksize = ((size_t)1 << lo_lgksize) - 1;
static size_t hi_mask_splitksize = (((size_t)1 << hi_lgsplitksize) - 1) << lo_lgksize;

size_t ksize_FildeshKVE_size(size_t x) {
  if (0 == (x & high_size_bit(0))) {
    return x;
  }
  return x & lo_mask_ksize;
}

size_t splitksize_FildeshKVE_size(size_t x) {
  if (0 == get_splitkexists_bit_FildeshKVE_size(x)) {
    return 0;
  }
  return (x & hi_mask_splitksize) >> lo_lgksize;
}


bool kdirect_FildeshKVE(const FildeshKVE* e) {
  const size_t ksize = ksize_FildeshKVE_size(e->size);
  return kdirect_FildeshKVE_joint(e->joint, ksize);
}

bool splitkdirect_FildeshKVE(const FildeshKVE* e) {
  const size_t ksize = splitksize_FildeshKVE_size(e->size);
  return splitkdirect_FildeshKVE_size(e->size, ksize);
}

  void
assign_v_FildeshKVE(FildeshKVE* e, size_t vsize, const void* v, FildeshAlloc* alloc)
{
  assert(0 != get_vexists_bit_FildeshKVE_joint(e->joint));
  if (v && vsize <= sizeof(e->kv[1])) {
    set0_vrefers_bit_FildeshKVE(e);
    memcpy(&e->kv[1], v, vsize);
  }
  else if (v && alloc) {
    void* p = reserve_FildeshAlloc(alloc, vsize, (vsize & -vsize));
    memcpy(p, v, vsize);
    e->kv[1] = (uintptr_t)p;
  }
  else {
    set1_vrefers_bit_FildeshKVE(e);
    e->kv[1] = (uintptr_t) v;
  }
}

  void
assign_splitv_FildeshKVE(FildeshKVE* e, size_t vsize, const void* v, FildeshAlloc* alloc)
{
  assert(0 != get_splitvexists_bit_FildeshKVE_size(e->size));
  if (v && vsize <= sizeof(e->split[1])) {
    set0_splitvrefers_bit_FildeshKVE(e);
    memcpy(&e->split[1], v, vsize);
  }
  else if (v && alloc) {
    void* p = reserve_FildeshAlloc(alloc, vsize, (vsize & -vsize));
    memcpy(p, v, vsize);
    e->split[1] = (uintptr_t)p;
  }
  else {
    set1_splitvrefers_bit_FildeshKVE(e);
    e->split[1] = (uintptr_t)v;
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
  if (0 != (e_size >> lo_lgksize)) {
    return false;
  }
  if (0 != (ksize >> hi_lgsplitksize)) {
    return false;
  }

  e_size |= (ksize << lo_lgksize);
  e_size |= splitkexists_bit_FildeshKVE_size();
  if (vexists) {
    e_size |= splitvexists_bit_FildeshKVE_size();
  }
  else {
    e_size &= ~splitvexists_bit_FildeshKVE_size();
  }
  *p_size = e_size;
  return true;
}

  bool
populate_splitkv_FildeshKVE(FildeshKVE* e,
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
  assert(0 != get_splitkexists_bit_FildeshKVE_size(e->size));

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

  bool
populate_demote_FildeshKVE(FildeshKVE* e,
                           size_t ksize, const void* k,
                           size_t vsize, const void* v,
                           FildeshAlloc* alloc)
{
  size_t e_size = ksize;
  const bool vexists = (0 != get_vexists_bit_FildeshKVE_joint(e->joint));
  if (!populate_splitk_FildeshKVE_size(&e_size, e->size, vexists)) {
    return false;
  }
  assert(0 != get_splitkexists_bit_FildeshKVE_size(e_size));
  e_size |= (e->joint & vrefers_bit_FildeshKVE_joint());
  e->split[0] = e->kv[0];
  e->split[1] = e->kv[1];
  populate_empty_FildeshKVE(e, ksize, k, vsize, v, alloc);
  e->size = e_size;
  return true;
}

  void
move_kv_to_empty_FildeshKVE(FildeshKVE* dst, FildeshKVE* src)
{
  const size_t vmask = (
      vexists_bit_FildeshKVE_joint() | vrefers_bit_FildeshKVE_joint());
  dst->joint = (dst->joint & ~vmask) | (src->joint & vmask);
  dst->kv[0] = src->kv[0];
  dst->kv[1] = src->kv[1];
  if (0 == get_splitkexists_bit_FildeshKVE_size(src->size)) {
    dst->size = src->size;
  }
  else {
    dst->size = ksize_FildeshKVE_size(src->size);
    promote_splitk_FildeshKVE(src);
  }
}

  void
move_splitkv_to_empty_FildeshKVE(FildeshKVE* dst, FildeshKVE* src)
{
  dst->size = splitksize_FildeshKVE_size(src->size);
  dst->kv[0] = src->split[0];
  dst->kv[1] = src->split[1];
  erase_splitk_FildeshKVE(src);
}

  bool
maybe_fuse_FildeshKVE(FildeshKVE* dst, unsigned side, const FildeshKVE* src)
{
  assert(!splitkexists_FildeshKVE(dst));
  assert(!splitkexists_FildeshKVE(src));

  if (side == 0) {
    const size_t vmask = (
        vexists_bit_FildeshKVE_joint() | vrefers_bit_FildeshKVE_joint());
    size_t e_size = src->size;
    const bool dst_vexists = (0 != get_vexists_bit_FildeshKVE_joint(dst->joint));
    if (!populate_splitk_FildeshKVE_size(&e_size, dst->size, dst_vexists)) {
      return false;
    }
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
    if (!populate_splitk_FildeshKVE_size(&e_size, src->size, src_vexists)) {
      return false;
    }
    dst->size = (e_size | (src->joint & vrefers_bit_FildeshKVE_joint()));
    dst->split[0] = src->kv[0];
    dst->split[1] = src->kv[1];
  }
  return true;
}

  void
erase_k_FildeshKVE(FildeshKVE* e)
{
  assert(kexists_FildeshKVE(e));
  assert(0 == get_splitkexists_bit_FildeshKVE_size(e->size));
  e->kv[0] = FildeshKV_NULL_INDEX;
  e->kv[1] = FildeshKV_NULL_INDEX;
  e->joint = get_index_FildeshKVE_joint(e->joint);
}

  void
erase_splitk_FildeshKVE(FildeshKVE* e)
{
  assert(0 != get_splitkexists_bit_FildeshKVE_size(e->size));
  e->split[0] = FildeshKV_NULL_INDEX;
  e->split[1] = FildeshKV_NULL_INDEX;
  e->size = ksize_FildeshKVE_size(e->size);
}

  void
promote_splitk_FildeshKVE(FildeshKVE* e)
{
  assert(0 != get_splitkexists_bit_FildeshKVE_size(e->size));
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
  return memcmp(indirect_splitk_FildeshKVE_split(e->split), key, actual_size);
}
