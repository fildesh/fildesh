
#include "kve.h"
#include <assert.h>
#include <string.h>

size_t ksize_FildeshKVE_size(size_t x) {
  unsigned i;
  if (0 == (x & high_size_bit(0))) {
    return x;
  }
  for (i = 1; i < sizeof(size_t)-1; ++i) {
    if (0 == (x & high_size_bit(i*CHAR_BIT))) {
      return x & (high_size_bit(i*CHAR_BIT)-1);
    }
  }
  return x & (((size_t)1 << CHAR_BIT)-1);
}

size_t splitksize_FildeshKVE_size(size_t x) {
  unsigned i;
  size_t y;
  if (0 == get_splitkexists_bit_FildeshKVE_size(x)) {
    return 0;
  }
  if (0 == get_splitvexists_bit_FildeshKVE_size(x)) {
    y = shiftmaskhi_size(x, 2, CHAR_BIT-2);
  } else {
    y = shiftmaskhi_size(x, 3, CHAR_BIT-3);
  }
  for (i = 1; i < sizeof(size_t)-1; ++i) {
    if (0 == (x & high_size_bit(i*CHAR_BIT))) {
      break;
    }
    y <<= CHAR_BIT-1;
    y |= shiftmaskhi_size(x, i*CHAR_BIT+1, CHAR_BIT-1);
  }
  return y;
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
  size_t tmp_ksize = ksize;
  unsigned i;
  if (vexists) {
    tmp_ksize >>= (CHAR_BIT-3);
  } else {
    tmp_ksize >>= (CHAR_BIT-2);
  }
  if (0 != shiftmaskhi_size(e_size, 0, CHAR_BIT)) {
    return false;
  }

  /* Counting from high.*/
  for (i = 1; i < sizeof(size_t)-1 && tmp_ksize > 0; ++i) {
    if (0 != shiftmaskhi_size(e_size, i*CHAR_BIT, CHAR_BIT)) {
      return false;
    }
    tmp_ksize >>= CHAR_BIT-1;
  }
  if (tmp_ksize > 0) {
    return false;
  }
  if (i < sizeof(size_t)-1 && 0 != (e_size & high_size_bit(i*CHAR_BIT))) {
    return false;
  }

  tmp_ksize = ksize;
  /* Counting from low.*/
  for (i = sizeof(size_t)-i; i < sizeof(size_t); ++i) {
    const size_t z = high_byte_bit(0) | (tmp_ksize & (high_byte_bit(0)-1));
    e_size |= z << (i*CHAR_BIT);
    tmp_ksize >>= CHAR_BIT-1;
  }

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
  if (!populate_splitk_FildeshKVE_size(&e->size, ksize, vexists)) {
    return false;
  }

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
  const bool vexists = (vsize > 0);
  if (!populate_splitk_FildeshKVE_size(&e_size, e->size, vexists)) {
    return false;
  }
  e->split[0] = e->kv[0];
  e->split[1] = e->kv[1];
  populate_empty_FildeshKVE(e, ksize, k, vsize, v, alloc);
  e->size = e_size;
  return true;
}

  void
move_kv_to_empty_FildeshKVE(FildeshKVE* dst, FildeshKVE* src)
{
  dst->size = ksize_FildeshKVE_size(src->size);
  dst->kv[0] = src->kv[0];
  dst->kv[1] = src->kv[1];
  promote_splitk_FildeshKVE(src);
}

  void
move_splitkv_to_empty_FildeshKVE(FildeshKVE* dst, FildeshKVE* src)
{
  dst->size = splitksize_FildeshKVE_size(src->size);
  dst->kv[0] = src->split[0];
  dst->kv[1] = src->split[1];
  erase_splitk_FildeshKVE(src);
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
