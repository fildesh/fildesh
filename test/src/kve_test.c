
#include "src/lib/kve.h"
#include <assert.h>
#include <string.h>

#if 0
#include <stdio.h>
#define TRACE_SIZES(label, ksize, vsize) \
  fprintf(stderr, "%s %u %u\n", label, (unsigned)ksize, (unsigned)vsize)
#else
#define TRACE_SIZES(label, ksize, vsize)
#endif

static void trivial_setget_bit_test() {
  const size_t hi3 = high_size_bit(0) | high_size_bit(1) | high_size_bit(2);
  const size_t hi2 = high_size_bit(0) | high_size_bit(1);
  FildeshKVE e;
  e.joint = 0;
  e.size = 0;

  /* Set bits from 0 to 1.*/
  assert(0 == get_red_bit_FildeshKVE_joint(e.joint));
  set1_red_bit_FildeshKVE(&e);
  assert(0 != get_red_bit_FildeshKVE_joint(e.joint));

  assert(!kexists_FildeshKVE(&e));
  assert(0 == get_vexists_bit_FildeshKVE_joint(e.joint));
  set1_vexists_bit_FildeshKVE(&e);
  assert(0 != get_vexists_bit_FildeshKVE_joint(e.joint));
  assert(kexists_FildeshKVE(&e));

  assert(0 == get_vrefers_bit_FildeshKVE_joint(e.joint));
  set1_vrefers_bit_FildeshKVE(&e);
  assert(0 != get_vrefers_bit_FildeshKVE_joint(e.joint));

  assert(!splitkexists_FildeshKVE(&e));
  assert(0 == get_splitvexists_bit_FildeshKVE_size(e.size));
  set1_splitvexists_bit_FildeshKVE(&e);
  assert(0 != get_splitvexists_bit_FildeshKVE_size(e.size));
  assert(splitkexists_FildeshKVE(&e));

  assert(0 == get_splitvrefers_bit_FildeshKVE_size(e.size));
  set1_splitvrefers_bit_FildeshKVE(&e);
  assert(0 != get_splitvrefers_bit_FildeshKVE_size(e.size));

  assert(e.joint == hi3);
  assert(e.size == hi2);
  e.joint = ~(size_t)0;
  e.size = ~(size_t)0;

  /* Set bits from 1 to 0.*/
  assert(0 != get_red_bit_FildeshKVE_joint(e.joint));
  set0_red_bit_FildeshKVE(&e);
  assert(0 == get_red_bit_FildeshKVE_joint(e.joint));

  assert(0 != get_vexists_bit_FildeshKVE_joint(e.joint));
  set0_vexists_bit_FildeshKVE(&e);
  assert(0 == get_vexists_bit_FildeshKVE_joint(e.joint));

  assert(kexists_FildeshKVE(&e));
  assert(0 != get_vrefers_bit_FildeshKVE_joint(e.joint));
  set0_vrefers_bit_FildeshKVE(&e);
  assert(0 == get_vrefers_bit_FildeshKVE_joint(e.joint));
  assert(!kexists_FildeshKVE(&e));

  assert(0 != get_splitvexists_bit_FildeshKVE_size(e.size));
  set0_splitvexists_bit_FildeshKVE(&e);
  assert(0 == get_splitvexists_bit_FildeshKVE_size(e.size));

  assert(splitkexists_FildeshKVE(&e));
  assert(0 != get_splitvrefers_bit_FildeshKVE_size(e.size));
  set0_splitvrefers_bit_FildeshKVE(&e);
  assert(0 == get_splitvrefers_bit_FildeshKVE_size(e.size));
  assert(!splitkexists_FildeshKVE(&e));

  assert(e.joint == ~hi3);
  assert(e.size == ~hi2);
}

static void check_setget(size_t ksize, size_t vsize,
                         size_t splitksize, size_t splitvsize)
{
  static const char b64chars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
  char k[sizeof(uintptr_t)*2+1+256];
  char v[sizeof(uintptr_t)+1];
  char splitk[sizeof(uintptr_t)*2+1+256];
  char splitv[sizeof(uintptr_t)+1];
  unsigned i;
  FildeshKVE e = DEFAULT_FildeshKVE;

  /* Test assumptions.*/
  assert(ksize > 0);
  assert(ksize <= sizeof(k));
  assert(vsize <= sizeof(v));
  assert(splitksize <= sizeof(splitk));
  assert(splitvsize <= sizeof(splitv));
  for (i = 0; i < ksize; ++i) { k[i] = b64chars[i % 64]; }
  for (i = 0; i < vsize; ++i) { v[i] = b64chars[(i+26) % 64]; }
  for (i = 0; i < splitksize; ++i) { splitk[i] = b64chars[(i+52) % 64]; }
  for (i = 0; i < splitvsize; ++i) { splitv[i] = b64chars[(i+62) % 64]; }

  populate_empty_FildeshKVE(&e, ksize, k, vsize, v, NULL);
  /* Size is preserved.*/
  assert(ksize == ksize_FildeshKVE_size(e.size));
  /* Key is compared properly.*/
  assert(0 == cmp_k_FildeshKVE(&e, ksize, k));
  assert(0 != cmp_k_FildeshKVE(&e, vsize, v));
  /* Get the value.*/
  assert(0 == memcmp(v, value_FildeshKVE(&e), vsize));
  /* No splitkey has been set, so that size should be zero.*/
  assert(0 == splitksize_FildeshKVE_size(e.size));

  if (splitksize == 0) {
    return;
  }

  /* Populate data (function asserts success).*/
  populate_splitkv_FildeshKVE(&e, splitksize, splitk, splitvsize, splitv, NULL);
  /* Size is preserved.*/
  assert(ksize == ksize_FildeshKVE_size(e.size));
  assert(splitksize == splitksize_FildeshKVE_size(e.size));
  /* Key is compared properly.*/
  assert(0 == cmp_splitk_FildeshKVE(&e, splitksize, splitk));
  assert(0 != cmp_splitk_FildeshKVE(&e, splitvsize, splitv));
  /* Get the value.*/
  assert(0 == memcmp(splitv, splitvalue_FildeshKVE(&e), splitvsize));
}

static void primary_setget_test() {
  unsigned ksize, vsize;
  for (ksize = 1; ksize <= sizeof(uintptr_t)*2+1; ++ksize) {
    for (vsize = 0; vsize <= sizeof(uintptr_t)+1; ++vsize) {
      TRACE_SIZES("", ksize, vsize);
      check_setget(ksize, vsize, 0, 0);
    }
  }
}

static void split_setget_test() {
  unsigned splitksize, splitvsize;
  static const size_t ksizes[3] = { 1, sizeof(uintptr_t)+1, 258 };
  static const size_t vsizes[3] = { 0, 1, sizeof(uintptr_t)+1 };
  for (splitksize = 1; splitksize <= sizeof(uintptr_t)*2+1; ++splitksize) {
    for (splitvsize = 0; splitvsize <= sizeof(uintptr_t)+1; ++splitvsize) {
      TRACE_SIZES("split", splitksize, splitvsize);
      check_setget(ksizes[0], vsizes[0], splitksize, splitvsize);
      check_setget(ksizes[1], vsizes[0], splitksize, splitvsize);
      check_setget(ksizes[2], vsizes[0], splitksize, splitvsize);
      check_setget(ksizes[0], vsizes[1], splitksize, splitvsize);
      check_setget(ksizes[1], vsizes[1], splitksize, splitvsize);
      check_setget(ksizes[2], vsizes[1], splitksize, splitvsize);
      check_setget(ksizes[0], vsizes[2], splitksize, splitvsize);
      check_setget(ksizes[1], vsizes[2], splitksize, splitvsize);
      check_setget(ksizes[2], vsizes[2], splitksize, splitvsize);
    }
  }
}

static bool
try_edge_split(FildeshKVE* e, size_t ksize, size_t splitksize, size_t splitvsize) {
  static const uintptr_t data[2] = { 0, 0 };
  bool populated;
  *e = default_FildeshKVE();
  populate_empty_FildeshKVE(e, ksize, data, 1, data, NULL);
  populated = maybe_populate_splitkv_FildeshKVE(
      e, splitksize, data, splitvsize, data, NULL);

  assert(ksize == ksize_FildeshKVE_size(e->size));
  assert(value_FildeshKVE(e));

  if (populated) {
    assert(splitksize == splitksize_FildeshKVE_size(e->size));
    if (splitvsize > 0) {
      assert(splitvalue_FildeshKVE(e));
    } else {
      assert(!splitvalue_FildeshKVE(e));
    }
  } else {
    assert(!splitkexists_FildeshKVE(e));
    assert(!splitvalue_FildeshKVE(e));
  }
  return populated;
}

static void edge_split_test() {
  FildeshKVE e;
  const size_t max_ksize = FildeshKVE_splitksize_max;
  const size_t max_splitksize = FildeshKVE_splitksize_max;

  /* First key's size is just too big.*/
  assert(!try_edge_split(&e, high_size_bit(CHAR_BIT-1), 1, 0));
  if (sizeof(size_t) > 2) {
    assert(!try_edge_split(&e, high_size_bit(CHAR_BIT), 1, 0));
  }

  /* Max allowable key sizes for being packed togethher.*/
  assert( try_edge_split(&e, max_ksize,   max_splitksize  , 0));
  assert( try_edge_split(&e, max_ksize,   max_splitksize  , 1));
  assert(!try_edge_split(&e, max_ksize+1, max_splitksize  , 0));
  assert(!try_edge_split(&e, max_ksize,   max_splitksize+1, 0));
  assert(!try_edge_split(&e, max_ksize+1, max_splitksize  , 1));
  assert(!try_edge_split(&e, max_ksize,   max_splitksize+1, 1));
}

static void zero_length_key_test() {
  FildeshKVE e[1] = {DEFAULT_FildeshKVE};
  char dummy[1] = {'d'};
  char value[1] = {'h'};
  char splitvalue[2] = {'w', 'o'};

  assert(e->kv[0] == 0);
  assert(e->kv[1] == 0);
  populate_empty_FildeshKVE(e, 0, dummy, sizeof(value), value, NULL);
  assert(e->kv[0] == 0);
  assert(e->kv[1] != 0);
  assert(0 == ksize_FildeshKVE_size(e->size));
  assert(0 == memcmp(value_FildeshKVE(e), value, sizeof(value)));

  assert(e->split[0] == FildeshKV_NULL_INDEX);
  assert(e->split[1] == FildeshKV_NULL_INDEX);
  populate_splitkv_FildeshKVE(e, 0, dummy, sizeof(splitvalue), splitvalue, NULL);
  assert(e->split[0] == FildeshKV_NULL_INDEX);
  assert(e->split[1] != FildeshKV_NULL_INDEX);
  assert(0 == ksize_FildeshKVE_size(e->size));
  assert(0 == splitksize_FildeshKVE_size(e->size));
  assert(0 == memcmp(value_FildeshKVE(e), value, sizeof(value)));
  assert(0 == memcmp(splitvalue_FildeshKVE(e), splitvalue, sizeof(splitvalue)));
}

int main() {
  trivial_setget_bit_test();
  primary_setget_test();
  split_setget_test();
  edge_split_test();
  zero_length_key_test();
  return 0;
}
