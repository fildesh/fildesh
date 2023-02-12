/** Fuzz test that adds, sets, and removes elements to a byte->byte map.
 *
 * We allow the values to have zero-length, which is interpreted as no value!
 * This approach gives better coverage, but can only be used when testing the
 * `ensure` function, not the `replace` function.
 **/
/* #define FILDESH_LOG_TRACE_ON 1 */
#include "fuzz_common.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include <fildesh/fildesh.h>


#ifdef FILDESH_LOG_TRACE_ON
#include "test/lib/kv_rbtree_validation.h"
static const void* dummy_key(uint8_t kidx) {
  static uint8_t keys[256];
  keys[kidx] = kidx;
  return &keys[kidx];
}
static void maybe_print_debug(FildeshKV* map) {
  FildeshO* out = open_FildeshOF("/dev/stderr");
  print_debug_FildeshKV_RBTREE(map, out);
  putc_FildeshO(out, '\n');
  close_FildeshO(out);
}
#else
static const void* dummy_key(uint8_t kidx) {
  /* Anything really. Shouldn't be referenced.*/
  static const uint8_t key = 7;
  (void)kidx;
  return &key;
}
static void maybe_print_debug(FildeshKV* map) {
  (void)map;  /* No-op.*/
}
#endif


static size_t key_size(uint8_t b) {
  const size_t shift =
    /* hi_lgksize in kve.c */
    sizeof(size_t)*CHAR_BIT/2-2
    /* Number of bits required for numbers 0 to 127.*/
    - 7
    ;
  return (size_t)b << shift;
}

  int
kv_fuzz_common(FildeshKV* map, const uint8_t data[], size_t size)
{
  size_t a[256];
  size_t i;

  memset(a, 0, sizeof(a));

  for (i = 0; i < size; ++i) {
    uint8_t kidx = data[i];
    size_t ksize = key_size(kidx);
    size_t v = i+1;
    FildeshKV_id_t id = lookup_FildeshKV(map, dummy_key(kidx), ksize);
    const size_t* p = (const size_t*) value_at_FildeshKV(map, id);

    if (kidx == 0) {continue;}
    if (i+1 < size && data[i+1] == 0) {
      i += 1;
      v = 0;
    }

    if (a[kidx] == 0) {
      assert(fildesh_nullid(id));
      assert(!p);
    }
    else {
      assert(!fildesh_nullid(id));
      assert(p);
      assert(*p == a[kidx]);
    }

    if (v == 0) {
      if (a[kidx] != 0) {
        remove_at_FildeshKV(map, id);
        assert(fildesh_nullid(lookup_FildeshKV(map, dummy_key(kidx), ksize)));
      }
    }
    else {
      id = ensure_FildeshKV(map, dummy_key(kidx), ksize);
      assign_at_FildeshKV(map, id, &v, sizeof(v));
      assert(id == lookup_FildeshKV(map, dummy_key(kidx), ksize));
    }
    a[kidx] = v;

    maybe_print_debug(map);
  }

  for (i = 1; i < 256; ++i) {
    uint8_t kidx = (uint8_t)i;
    size_t ksize = key_size(kidx);
    FildeshKV_id_t id = lookup_FildeshKV(map, dummy_key(kidx), ksize);
    const size_t* p = (const size_t*) value_at_FildeshKV(map, id);
    if (a[kidx] == 0) {
      assert(fildesh_nullid(id));
      assert(!p);
    }
    else {
      assert(!fildesh_nullid(id));
      assert(p);
      assert(*p == a[kidx]);
    }
  }
  return 0;
}
