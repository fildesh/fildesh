
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

/** Assuming the node is empty, populate it with a key and value.
 *
 * This only overwrites data, leaving the node's connectivity intact.
 **/
  void
populate_empty_FildeshKVE(FildeshKVE* e,
                          size_t ksize, const void* k,
                          size_t vsize, const void* v)
{
  bool kdirect;
  /* Assuming no split data, so we can clear e->size.*/
  e->size = ksize;

  if (vsize > 0) {
    set1_vexists_bit_FildeshKVE(e);
    if (v && vsize <= sizeof(e->kv[1])) {
      set1_vdirect_bit_FildeshKVE(e);
      memcpy(&e->kv[1], v, vsize);
    } else {
      set0_vdirect_bit_FildeshKVE(e);
      e->kv[1] = (uintptr_t) v;
    }
    kdirect = (ksize <= sizeof(e->kv[0]));
  } else {
    set0_vexists_bit_FildeshKVE(e);
    set0_vdirect_bit_FildeshKVE(e);
    kdirect = (ksize <= sizeof(e->kv[0]) + sizeof(e->kv[1]));
  }
  if (kdirect) {
    memcpy(&e->kv[0], k, ksize);
  } else {
    e->kv[0] = (uintptr_t)k;
  }
}


  bool
populate_splitkv_FildeshKVE(FildeshKVE* e,
                            size_t ksize, const void* k,
                            size_t vsize, const void* v)
{
  const bool vexists = (vsize > 0);
  bool kdirect;
  size_t tmp_ksize = ksize;
  unsigned i;
  if (vexists) {
    tmp_ksize >>= (CHAR_BIT-3);
  } else {
    tmp_ksize >>= (CHAR_BIT-2);
  }
  if (0 != shiftmaskhi_size(e->size, 0, CHAR_BIT)) {
    return false;
  }

  /* Counting from high.*/
  for (i = 1; i < sizeof(size_t)-1 && tmp_ksize > 0; ++i) {
    if (0 != shiftmaskhi_size(e->size, i*CHAR_BIT, CHAR_BIT)) {
      return false;
    }
    tmp_ksize >>= CHAR_BIT-1;
  }
  if (tmp_ksize > 0) {
    return false;
  }
  if (i < sizeof(size_t)-1 && 0 != (e->size & high_size_bit(i*CHAR_BIT))) {
    return false;
  }

  tmp_ksize = ksize;
  /* Counting from low.*/
  for (i = sizeof(size_t)-i; i < sizeof(size_t); ++i) {
    const size_t z = high_byte_bit(0) | (tmp_ksize & (high_byte_bit(0)-1));
    e->size |= z << (i*CHAR_BIT);
    tmp_ksize >>= CHAR_BIT-1;
  }

  if (vexists) {
    set1_splitvexists_bit_FildeshKVE(e);
    if (v && vsize <= sizeof(e->split[1])) {
      set1_splitvdirect_bit_FildeshKVE(e);
      memcpy(&e->split[1], v, vsize);
    } else {
      set0_splitvdirect_bit_FildeshKVE(e);
      e->split[1] = (uintptr_t) v;
    }
    kdirect = (ksize <= sizeof(e->split[0]));
  } else {
    set0_splitvexists_bit_FildeshKVE(e);
    kdirect = (ksize <= sizeof(e->split[0]) + sizeof(e->split[1]));
  }
  if (kdirect) {
    memcpy(&e->split[0], k, ksize);
  } else {
    e->split[0] = (uintptr_t)k;
  }
  return true;
}

  void
erase_splitk_FildeshKVE(FildeshKVE* e)
{
  assert(0 != get_splitkexists_bit_FildeshKVE_size(e->size));
  e->split[0] = 0;
  e->split[1] = 0;
  e->size = ksize_FildeshKVE_size(e->size);
}

  void
promote_splitk_FildeshKVE(FildeshKVE* e)
{
  assert(0 != get_splitkexists_bit_FildeshKVE_size(e->size));
  {
    /* The exists and direct bits are consistent across `joint` and `size`.*/
    const size_t splitvbits = (splitvexists_bit_FildeshKVE_size() |
                               splitvdirect_bit_FildeshKVE_size());
    e->joint &= ~splitvbits;
    e->joint |= (splitvbits & e->size);
  }
  e->size = splitksize_FildeshKVE_size(e->size);
  e->kv[0] = e->split[0];
  e->kv[1] = e->split[1];
  e->split[0] = 0;
  e->split[1] = 0;
}

  int
cmp_FildeshKVE_(size_t keysize, const void* key,
                size_t ejoint, size_t esize,
                const uintptr_t ekv[2])
{
  const size_t actual_size = ksize_FildeshKVE_size(esize);
  if (keysize != actual_size) {
    return (keysize < actual_size ? -1 : 1);
  }
  if (kdirect_FildeshKVE_joint(ejoint, actual_size)) {
    return memcmp(key, direct_k_FildeshKVE_kv(ekv), actual_size);
  } else {
    return memcmp(key, indirect_k_FildeshKVE_kv(ekv), actual_size);
  }
}

  int
cmp_split_FildeshKVE_(size_t keysize, const void* key,
                   size_t esize, const uintptr_t esplit[2])
{
  const size_t actual_size = splitksize_FildeshKVE_size(esize);
  if (keysize != actual_size) {
    return (keysize < actual_size ? -1 : 1);
  }
  if (splitkdirect_FildeshKVE_size(esize, actual_size)) {
    return memcmp(key, direct_splitk_FildeshKVE_split(esplit), actual_size);
  } else {
    return memcmp(key, indirect_splitk_FildeshKVE_split(esplit), actual_size);
  }
}
