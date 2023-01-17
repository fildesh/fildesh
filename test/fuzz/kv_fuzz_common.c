/** Fuzz test that adds, sets, and removes elements to a byte->byte map.
 *
 * We allow the values to have zero-length, which is interpreted as no value!
 * This approach gives better coverage, but can only be used when testing the
 * `ensure` function, not the `replace` function.
 **/
#include "fuzz_common.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include <fildesh/fildesh.h>

static const uint8_t NULL_VALUE = 255;

  int
kv_fuzz_common(FildeshKV* map, const uint8_t data[], size_t size)
{
  uint8_t a[256];
  size_t i;

  memset(a, NULL_VALUE, sizeof(a));

  for (i = 0; i < size; i+=2) {
    uint8_t k = data[i];
    uint8_t v = i+1 < size ? data[i+1] : NULL_VALUE;
    FildeshKV_id_t id = lookup_FildeshKV(map, &k, 1);
    const uint8_t* p = (const uint8_t*) value_at_FildeshKV(map, id);

    if (a[k] == NULL_VALUE) {
      assert(fildesh_nullid(id));
      assert(!p);
    }
    else {
      assert(!fildesh_nullid(id));
      assert(p);
      assert(*p == a[k]);
    }

    if (v == NULL_VALUE) {
      if (a[k] != NULL_VALUE) {
        remove_at_FildeshKV(map, id);
        assert(fildesh_nullid(lookup_FildeshKV(map, &k, 1)));
      }
    }
    else {
      id = ensure_FildeshKV(map, &k, 1);
      assign_at_FildeshKV(map, id, &v, 1);
      assert(id == lookup_FildeshKV(map, &k, 1));
    }
    a[k] = v;
  }

  for (i = 0; i < 256; ++i) {
    const uint8_t k = (uint8_t)i;
    FildeshKV_id_t id = lookup_FildeshKV(map, &k, 1);
    const uint8_t* p = (const uint8_t*) value_at_FildeshKV(map, id);
    if (a[k] == NULL_VALUE) {
      assert(fildesh_nullid(id));
      assert(!p);
    }
    else {
      assert(!fildesh_nullid(id));
      assert(p);
      assert(*p == a[k]);
    }
  }
  close_FildeshKV(map);
  return 0;
}
