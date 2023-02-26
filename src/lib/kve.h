#ifndef FILDESH_KVE_H_
#define FILDESH_KVE_H_
#include <fildesh/fildesh.h>

/** Node for linked list, hash map, and red-black tree.
 **/
struct FildeshKVE {
  /** Index of "parent" or "previous" node with extra data in high 3 bits.
   *
   * High bits: vexists, vrefers, red.
   * We can use these 3 bits because size_t is at least 2 bytes
   * and this struct is at least 12 bytes.
   * Setting vexists==0 && vrefers==0 means the node is unoccupied.
   **/
  size_t joint;
  /** The primary key size with the split key size packed in.
   *
   * High bits: splitvexists, splitvrefers.
   * We assume 2 bit can be used because key sizes are generally small.
   * The splitvexists bit is conditional on splitkexists.
   * The splitvrefers bit is conditional on splitvexists.
   * Setting splitvexists==0 && splitvrefers==0 means the node is not doubled.
   **/
  size_t size;
  /** Stores indices of "child" nodes or possibly a kv-pair.**/
  uintptr_t split[2];
  /** Stores a kv-pair.**/
  uintptr_t kv[2];
};

#define FildeshKV_NULL_INDEX (~(size_t)0 >> 3)
#define DEFAULT_FildeshKVE  { \
  FildeshKV_NULL_INDEX, 0, \
  { FildeshKV_NULL_INDEX, FildeshKV_NULL_INDEX }, \
  { 0, 0 }, \
}

#define FildeshKVE_splitk_lgsize_max (sizeof(size_t)*CHAR_BIT/2-1)
static const size_t FildeshKVE_splitksize_max = ((size_t)1 << FildeshKVE_splitk_lgsize_max) - 1;


static inline FildeshKVE default_FildeshKVE() {
  FildeshKVE e = DEFAULT_FildeshKVE;
  return e;
}

static inline unsigned high_byte_bit(unsigned bitpos) {
  return (unsigned)1 << (CHAR_BIT-(bitpos+1));
}
static inline size_t high_size_bit(size_t bitpos) {
  return (size_t)1 << (CHAR_BIT * sizeof(size_t) - (bitpos+1));
}
static inline size_t
shiftmaskhi_size(size_t size, unsigned bigbitpos, unsigned bitcount) {
  return ((size >> (CHAR_BIT * sizeof(size_t) - (bigbitpos + bitcount)))
          & (((size_t)1 << bitcount)-1));
}

static inline size_t red_bit_FildeshKVE_joint() { return high_size_bit(2); }
static inline size_t vexists_bit_FildeshKVE_joint() { return high_size_bit(0); }
static inline size_t vrefers_bit_FildeshKVE_joint() { return high_size_bit(1); }
static inline size_t splitvexists_bit_FildeshKVE_size() { return high_size_bit(0); }
static inline size_t splitvrefers_bit_FildeshKVE_size() { return high_size_bit(1); }

static inline size_t get_red_bit_FildeshKVE_joint(size_t x) {
  return x & red_bit_FildeshKVE_joint();
}
static inline size_t get_vexists_bit_FildeshKVE_joint(size_t x) {
  return x & vexists_bit_FildeshKVE_joint();
}
static inline size_t get_vrefers_bit_FildeshKVE_joint(size_t x) {
  return x & vrefers_bit_FildeshKVE_joint();
}
static inline size_t get_index_FildeshKVE_joint(size_t x) {
  return x & (~(size_t)0 >> 3);
}
static inline size_t get_splitvexists_bit_FildeshKVE_size(size_t x) {
  return x & splitvexists_bit_FildeshKVE_size();
}
static inline size_t get_splitvrefers_bit_FildeshKVE_size(size_t x) {
  return x & splitvrefers_bit_FildeshKVE_size();
}

static inline bool kexists_FildeshKVE_joint(size_t x) {
  const size_t mask = (
      vexists_bit_FildeshKVE_joint() |
      vrefers_bit_FildeshKVE_joint());
  return 0 != (x & mask);
}
static inline bool splitkexists_FildeshKVE_size(size_t x) {
  const size_t mask = (
      splitvexists_bit_FildeshKVE_size() |
      splitvrefers_bit_FildeshKVE_size());
  return 0 != (x & mask);
}

static inline bool red_FildeshKVE(const FildeshKVE* e) {
  return 0 != get_red_bit_FildeshKVE_joint(e->joint);
}
static inline bool kexists_FildeshKVE(const FildeshKVE* e) {
  return kexists_FildeshKVE_joint(e->joint);
}
static inline bool splitkexists_FildeshKVE(const FildeshKVE* e) {
  return splitkexists_FildeshKVE_size(e->size);
}
static inline size_t ksize_FildeshKVE_size(size_t x) {
  if (!splitkexists_FildeshKVE_size(x)) {
    return x;
  }
  return x & FildeshKVE_splitksize_max;
}
static inline size_t splitksize_FildeshKVE_size(size_t x) {
  /* Assume it exists. No check.*/
  return (x >> FildeshKVE_splitk_lgsize_max) & FildeshKVE_splitksize_max;
}


static inline void set0_red_bit_FildeshKVE(FildeshKVE* e) {
  e->joint &= ~red_bit_FildeshKVE_joint();
}
static inline void set1_red_bit_FildeshKVE(FildeshKVE* e) {
  e->joint |= red_bit_FildeshKVE_joint();
}
static inline void swap_red_bit_FildeshKVE(FildeshKVE* lhs, FildeshKVE* rhs) {
  const size_t lhs_bit = get_red_bit_FildeshKVE_joint(lhs->joint);
  const size_t rhs_bit = get_red_bit_FildeshKVE_joint(rhs->joint);
  if (lhs_bit != rhs_bit) {
    rhs->joint ^= red_bit_FildeshKVE_joint();
    lhs->joint ^= red_bit_FildeshKVE_joint();
  }
}
static inline void set0_vexists_bit_FildeshKVE(FildeshKVE* e) {
  e->joint &= ~vexists_bit_FildeshKVE_joint();
}
static inline void set1_vexists_bit_FildeshKVE(FildeshKVE* e) {
  e->joint |= vexists_bit_FildeshKVE_joint();
}
static inline void set0_vrefers_bit_FildeshKVE(FildeshKVE* e) {
  e->joint &= ~vrefers_bit_FildeshKVE_joint();
}
static inline void set1_vrefers_bit_FildeshKVE(FildeshKVE* e) {
  e->joint |= vrefers_bit_FildeshKVE_joint();
}
static inline void set0_splitvexists_bit_FildeshKVE(FildeshKVE* e) {
  e->size &= ~splitvexists_bit_FildeshKVE_size();
}
static inline void set1_splitvexists_bit_FildeshKVE(FildeshKVE* e) {
  e->size |= splitvexists_bit_FildeshKVE_size();
}
static inline void set0_splitvrefers_bit_FildeshKVE(FildeshKVE* e) {
  e->size &= ~splitvrefers_bit_FildeshKVE_size();
}
static inline void set1_splitvrefers_bit_FildeshKVE(FildeshKVE* e) {
  e->size |= splitvrefers_bit_FildeshKVE_size();
}
static inline void set_joint_index_FildeshKVE(FildeshKVE* e, size_t index) {
  e->joint ^= get_index_FildeshKVE_joint(e->joint);
  e->joint ^= get_index_FildeshKVE_joint(index);
}
static inline void nullify_joint_index_FildeshKVE(FildeshKVE* e) {
  e->joint |= FildeshKV_NULL_INDEX;
}

static inline bool kdirect_FildeshKVE_joint(size_t ejoint, size_t actual_ksize) {
  if (0 != get_vexists_bit_FildeshKVE_joint(ejoint)) {
    return (sizeof(uintptr_t) >= actual_ksize);
  }
  return (2*sizeof(uintptr_t) >= actual_ksize);
}
static inline bool splitkdirect_FildeshKVE_size(size_t esize, size_t actual_ksize) {
  if (0 != get_splitvexists_bit_FildeshKVE_size(esize)) {
    return (sizeof(uintptr_t) >= actual_ksize);
  }
  return (2*sizeof(uintptr_t) >= actual_ksize);
}
static inline bool kdirect_FildeshKVE(const FildeshKVE* e) {
  const size_t ksize = ksize_FildeshKVE_size(e->size);
  return kdirect_FildeshKVE_joint(e->joint, ksize);
}
static inline bool splitkdirect_FildeshKVE(const FildeshKVE* e) {
  const size_t ksize = splitksize_FildeshKVE_size(e->size);
  return splitkdirect_FildeshKVE_size(e->size, ksize);
}

static inline const void* direct_k_FildeshKVE_kv(const uintptr_t* kv) {
  return &kv[0];
}
static inline const void* direct_splitk_FildeshKVE_split(const uintptr_t* split) {
  return &split[0];
}
static inline const void* indirect_k_FildeshKVE_kv(const uintptr_t* kv) {
  return (const void*) kv[0];
}
static inline const void* indirect_splitk_FildeshKVE_split(const uintptr_t* split) {
  return (const void*) split[0];
}

static inline
  const void*
value_FildeshKVE(const FildeshKVE* e)
{
  if (0 != get_vexists_bit_FildeshKVE_joint(e->joint)) {
    if (0 != get_vrefers_bit_FildeshKVE_joint(e->joint)) {
      return (const void*)e->kv[1];
    } else {
      return (const void*)&e->kv[1];
    }
  }
  return NULL;
}

static inline
  const void*
splitvalue_FildeshKVE(const FildeshKVE* e)
{
  if (splitkexists_FildeshKVE(e)) {
    if (0 != get_splitvexists_bit_FildeshKVE_size(e->size)) {
      if (0 != get_splitvrefers_bit_FildeshKVE_size(e->size)) {
        return (const void*)e->split[1];
      } else {
        return (const void*)&e->split[1];
      }
    }
  }
  return NULL;
}

static inline void assign_memref_v_FildeshKVE(FildeshKVE* e, const void* v) {
  set1_vrefers_bit_FildeshKVE(e);
  e->kv[1] = (uintptr_t) v;
}
static inline void assign_memref_splitv_FildeshKVE(FildeshKVE* e, const void* v) {
  set1_splitvrefers_bit_FildeshKVE(e);
  e->split[1] = (uintptr_t) v;
}

static inline
  void
copy_kv_FildeshKVE(FildeshKVE* dst, const FildeshKVE* src)
{
  const size_t joint_mask = (
      vexists_bit_FildeshKVE_joint() | vrefers_bit_FildeshKVE_joint());
  dst->kv[0] = src->kv[0];
  dst->kv[1] = src->kv[1];
  if (splitkexists_FildeshKVE_size(dst->size)) {
    const size_t size_mask = FildeshKVE_splitksize_max;
    dst->size = (dst->size & ~size_mask) | (size_mask & src->size);
  }
  else {
    dst->size = ksize_FildeshKVE_size(src->size);
  }
  dst->joint = (dst->joint & ~joint_mask) | (joint_mask & src->joint);
}

void
assign_v_FildeshKVE(FildeshKVE*, size_t, const void*, FildeshAlloc*);
void
assign_splitv_FildeshKVE(FildeshKVE*, size_t, const void*, FildeshAlloc*);

void
populate_empty_FildeshKVE(FildeshKVE* e,
                          size_t ksize, const void* k,
                          size_t vsize, const void* v,
                          FildeshAlloc* alloc);
bool
maybe_populate_splitkv_FildeshKVE(
    FildeshKVE* e,
    size_t ksize, const void* k,
    size_t vsize, const void* v,
    FildeshAlloc* alloc);
void
populate_splitkv_FildeshKVE(FildeshKVE* e,
                            size_t ksize, const void* k,
                            size_t vsize, const void* v,
                            FildeshAlloc* alloc);
void
fuse_FildeshKVE(FildeshKVE* dst, unsigned side, const FildeshKVE* src);
void
erase_k_FildeshKVE(FildeshKVE* e);
void
erase_splitk_FildeshKVE(FildeshKVE* e);
void
promote_splitk_FildeshKVE(FildeshKVE* e);
int
cmp_k_FildeshKVE(const FildeshKVE* e, size_t keysize, const void* key);
int
cmp_splitk_FildeshKVE(const FildeshKVE* e, size_t keysize, const void* key);
int
cmp_FildeshKVE(const FildeshKVE* e, const FildeshKVE* g);

#endif
