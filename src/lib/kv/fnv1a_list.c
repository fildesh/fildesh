
#include "kv_single_list.h"
#include "kv.h"

static FildeshKV_id_t
any_id_FildeshKV_FNV1A_LIST(const FildeshKV*);
static FildeshKV_id_t
lookup_FildeshKV_FNV1A_LIST(const FildeshKV*, const void*, size_t);
static FildeshKV_id_t
ensure_FildeshKV_FNV1A_LIST(FildeshKV*, const void*, size_t, FildeshAlloc*);

const FildeshKV_VTable DEFAULT_FNV1A_LIST_FildeshKV_VTable = {
  any_id_FildeshKV_FNV1A_LIST,
  lookup_FildeshKV_FNV1A_LIST,
  ensure_FildeshKV_FNV1A_LIST,
  remove_FildeshKV_SINGLE_LIST,
};

  FildeshKV_id_t
any_id_FildeshKV_SINGLE_LIST(const FildeshKV* map) {
  if (map->freelist_head == 0) {
    return FildeshKV_NULL_ID;
  }
  return 0;
}

static size_t fnv1a_hash(const void* k, size_t ksize, unsigned lgsize) {
  const uint64_t fnv_offset = 0xcbf29ce484222325;
  const uint64_t fnv_prime = 0x100000001b3;
  uint64_t hash = fnv_offset;
  size_t i;
  for (i = 0; i < ksize; ++i) {
    unsigned char b = ((unsigned char*)k)[i];
    hash ^= b;
    hash *= fnv_prime;
  }
  /* XOR-folding to produce a `lgsize`-bit hash.
   * See: http://www.isthe.com/chongo/tech/comp/fnv/#xor-fold
   * If `lgsize` is less than 32, we could fold in half before the final fold,
   * but the FNV authors suggest folding from the 64-bit hash directly.
   */
  return (hash>>lgsize) ^ (hash & (((uint64_t)1<<lgsize)-1));
}

  FildeshKV_id_t
any_id_FildeshKV_SINGLE_LIST(const FildeshKV* map) {
  if (map->freelist_head == 0) {
    return FildeshKV_NULL_ID;
  }
  return 0;
}

  FildeshKV_id_t
lookup_FildeshKV_FNV1A_LIST(const FildeshKV* map, const void* k, size_t ksize)
{
  size_t i;
  if (map->freelist_head == 0) {
    return FildeshKV_NULL_ID;
  }
  i = fnv1a_hash(k, ksize, map->allocated_lgcount);
  return lookup_from_list_bucket_FildeshKV(map, i, k, ksize);
}

static
  FildeshKV_id_t
add_FildeshKV_FNV1A_LIST(
    FildeshKV* map, const void* k, size_t ksize, FildeshAlloc* alloc)
{
  size_t allocated_count = fildesh_size_of_lgcount(1, map->allocated_lgcount);
  if (map->freelist_head > 0 &&
      0 == get_splitkexists_bit_FildeshKVE_size(map->at[0].size))
  {
    if (populate_splitkv_FildeshKVE(
            &map->at[0], ksize, k, 1, NULL, alloc)) {
      return 1;
    }
  }
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
ensure_FildeshKV_FNV1A_LIST(
    FildeshKV* map, const void* k, size_t ksize, FildeshAlloc* alloc)
{
  size_t i = fnv1a_hash(k, ksize, map->allocated_lgcount);
  return lookup_from_list_bucket_FildeshKV(map, i, k, ksize);
}

