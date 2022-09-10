
#include "src/kve.h"
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
  FildeshKVE e;
  e.joint = 0;
  e.size = 0;

  /* Set bits from 0 to 1.*/
  assert(0 == get_red_bit_FildeshKVE_joint(e.joint));
  set1_red_bit_FildeshKVE(&e);
  assert(0 != get_red_bit_FildeshKVE_joint(e.joint));

  assert(0 == get_vexists_bit_FildeshKVE_joint(e.joint));
  set1_vexists_bit_FildeshKVE(&e);
  assert(0 != get_vexists_bit_FildeshKVE_joint(e.joint));

  assert(0 == get_vdirect_bit_FildeshKVE_joint(e.joint));
  set1_vdirect_bit_FildeshKVE(&e);
  assert(0 != get_vdirect_bit_FildeshKVE_joint(e.joint));

  assert(0 == get_splitkexists_bit_FildeshKVE_size(e.size));
  set1_splitkexists_bit_FildeshKVE(&e);
  assert(0 != get_splitkexists_bit_FildeshKVE_size(e.size));

  assert(0 == get_splitvexists_bit_FildeshKVE_size(e.size));
  set1_splitvexists_bit_FildeshKVE(&e);
  assert(0 != get_splitvexists_bit_FildeshKVE_size(e.size));

  assert(0 == get_splitvdirect_bit_FildeshKVE_size(e.size));
  set1_splitvdirect_bit_FildeshKVE(&e);
  assert(0 != get_splitvdirect_bit_FildeshKVE_size(e.size));

  assert(e.joint == hi3);
  assert(e.size == hi3);
  e.joint = ~(size_t)0;
  e.size = ~(size_t)0;

  /* Set bits from 1 to 0.*/
  assert(0 != get_red_bit_FildeshKVE_joint(e.joint));
  set0_red_bit_FildeshKVE(&e);
  assert(0 == get_red_bit_FildeshKVE_joint(e.joint));

  assert(0 != get_vexists_bit_FildeshKVE_joint(e.joint));
  set0_vexists_bit_FildeshKVE(&e);
  assert(0 == get_vexists_bit_FildeshKVE_joint(e.joint));

  assert(0 != get_vdirect_bit_FildeshKVE_joint(e.joint));
  set0_vdirect_bit_FildeshKVE(&e);
  assert(0 == get_vdirect_bit_FildeshKVE_joint(e.joint));

  assert(0 != get_splitkexists_bit_FildeshKVE_size(e.size));
  set0_splitkexists_bit_FildeshKVE(&e);
  assert(0 == get_splitkexists_bit_FildeshKVE_size(e.size));

  assert(0 != get_splitvexists_bit_FildeshKVE_size(e.size));
  set0_splitvexists_bit_FildeshKVE(&e);
  assert(0 == get_splitvexists_bit_FildeshKVE_size(e.size));

  assert(0 != get_splitvdirect_bit_FildeshKVE_size(e.size));
  set0_splitvdirect_bit_FildeshKVE(&e);
  assert(0 == get_splitvdirect_bit_FildeshKVE_size(e.size));

  assert(e.joint == ~hi3);
  assert(e.size == ~hi3);
}

static void check_setget(size_t ksize, size_t vsize,
                         size_t splitksize, size_t splitvsize)
{
  static const char b64chars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
  char k[sizeof(uintptr_t)*2+1+256];
  char v[sizeof(uintptr_t)+1];
  char splitk[sizeof(uintptr_t)*2+1+256];
  char splitv[sizeof(uintptr_t)+1];
  bool populated_splitkv;
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

  populate_empty_FildeshKVE(&e, ksize, k, vsize, v);
  /* Size is preserved.*/
  assert(ksize == ksize_FildeshKVE_size(e.size));
  /* Key is compared properly.*/
  assert(0 == cmp_FildeshKVE_(ksize, k, e.joint, e.size, e.kv));
  assert(0 != cmp_FildeshKVE_(vsize, v, e.joint, e.size, e.kv));
  /* Get the value.*/
  assert(0 == memcmp(v, value_FildeshKVE(&e), vsize));
  /* No splitkey has been set, so that size should be zero.*/
  assert(0 == splitksize_FildeshKVE_size(e.size));

  if (splitksize == 0) {
    return;
  }

  populated_splitkv =
    populate_splitkv_FildeshKVE(&e, splitksize, splitk, splitvsize, splitv);
  /* It actually populated data.*/
  assert(populated_splitkv);
  /* Size is preserved.*/
  assert(ksize == ksize_FildeshKVE_size(e.size));
  assert(splitksize == splitksize_FildeshKVE_size(e.size));
  /* Key is compared properly.*/
  assert(0 == cmp_split_FildeshKVE_(splitksize, splitk, e.size, e.split));
  assert(0 != cmp_split_FildeshKVE_(splitvsize, splitv, e.size, e.split));
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
  populate_empty_FildeshKVE(e, ksize, data, 1, data);
  populated = populate_splitkv_FildeshKVE(e, splitksize, data, splitvsize, data);

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
    assert(0 == splitksize_FildeshKVE_size(e->size));
    assert(!splitvalue_FildeshKVE(e));
  }
  return populated;
}

static void edge_split_test() {
  FildeshKVE e;

  /* First key's size is just too big.*/
  assert(!try_edge_split(&e, high_size_bit(CHAR_BIT-1), 1, 0));
  if (sizeof(size_t) > 2) {
    assert(!try_edge_split(&e, high_size_bit(CHAR_BIT), 1, 0));
  }

  /* First key size fills all bits exept the high 1 byte + 1 bit.*/
  assert( try_edge_split(&e, high_size_bit(CHAR_BIT)-1, 1,                  0));
  assert( try_edge_split(&e, high_size_bit(CHAR_BIT)-1, high_byte_bit(1)-1, 0));
  assert(!try_edge_split(&e, high_size_bit(CHAR_BIT)-1, high_byte_bit(1),   0));
  assert( try_edge_split(&e, high_size_bit(CHAR_BIT)-1, high_byte_bit(2)-1, 1));
  assert(!try_edge_split(&e, high_size_bit(CHAR_BIT)-1, high_byte_bit(2),   1));

  /* First keys size fills all bits of the low-order byte.*/
  assert( try_edge_split(&e, (1<<CHAR_BIT)-1, high_size_bit(1+sizeof(size_t)-2+CHAR_BIT)-1, 0));
  assert(!try_edge_split(&e, (1<<CHAR_BIT)-1, high_size_bit(1+sizeof(size_t)-2+CHAR_BIT),   0));
  assert( try_edge_split(&e, (1<<CHAR_BIT)-1, high_size_bit(2+sizeof(size_t)-2+CHAR_BIT)-1, 1));
  assert(!try_edge_split(&e, (1<<CHAR_BIT)-1, high_size_bit(2+sizeof(size_t)-2+CHAR_BIT),   1));
}

static void zero_length_key_test() {
  FildeshKVE e[1] = {DEFAULT_FildeshKVE};
  char dummy[1] = {'d'};
  char value[1] = {'h'};
  char splitvalue[2] = {'w', 'o'};

  populate_empty_FildeshKVE(e, 0, dummy, sizeof(value), value);
  assert(e->kv[0] == 0);
  assert(e->kv[1] != 0);
  assert(0 == ksize_FildeshKVE_size(e->size));
  assert(0 == memcmp(value_FildeshKVE(e), value, sizeof(value)));

  populate_splitkv_FildeshKVE(e, 0, dummy, sizeof(splitvalue), splitvalue);
  assert(e->split[0] == 0);
  assert(e->split[1] != 0);
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
