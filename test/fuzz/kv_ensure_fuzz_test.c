/** Fuzz test that adds elements to a string->string map.
 *
 * We allow the keys and values to have zero-length.
 **/
#include "fuzz_common.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

/* #define FILDESH_LOG_TRACE_ON 1 */
#include <fildesh/fildesh.h>

#include "src/lib/kv.h"
#include "test/lib/kv_rbtree_validation.h"

static bool is_rbtree_selection(uint8_t map_selection) {
  return map_selection == 3 || map_selection == 5;
}

static bool is_broadleaf_rbtree_selection(uint8_t map_selection) {
  return map_selection == 5;
}

static inline
  FildeshKV
initial_map(uint8_t map_selection)
{
  const FildeshKV a[] = {
    DEFAULT_FildeshKV,
    DEFAULT_FildeshKV_SINGLE_LIST,
    DEFAULT_FildeshKV_BSTREE,
    DEFAULT_FildeshKV_RBTREE,
    DEFAULT_FildeshKV_BROADLEAF_BSTREE,
    DEFAULT_FildeshKV_BROADLEAF_RBTREE,
  };
  const uint8_t n = (uint8_t)(sizeof(a) / sizeof(a[0]));
  if (map_selection < n) {
    return a[map_selection];
  }
  return a[0];
}

typedef struct Entry Entry;
struct Entry {
  const uint8_t* key; size_t ksize;
  const uint8_t* value; size_t vsize;
};

static
  void
initialize_entries(Entry** entries, const uint8_t data[], size_t size)
{
  size_t k_index = 1;
  size_t k_size = 0;
  size_t v_index = 0;
  size_t v_size = 0;
  size_t i;

  init_FildeshAT(entries);

  for (i = 1; i < size; ++i) {
    const uint8_t b = data[i];
    if (i+1 == size || (v_index > k_index && b == 0)) {
      Entry entry;
      entry.key = &data[k_index];
      entry.ksize = k_size;
      entry.value = &data[v_index];
      entry.vsize = v_size;
      push_FildeshAT(entries, entry);

      k_index = i+1;
      k_size = 0;
      v_index = 0;
      v_size = 0;
    }
    else if (b == 0) {
      v_index = k_index + k_size + 1;
    }
    else if (v_index > k_index) {
      v_size += 1;
    }
    else {
      k_size += 1;
    }
  }
}

  int
LLVMFuzzerTestOneInput(const uint8_t data[], size_t size) {
  const uint8_t map_selection = (size > 0 ? data[0] : 0);
  bool is_rbtree = is_rbtree_selection(map_selection);
  bool is_broadleaf_rbtree = is_broadleaf_rbtree_selection(map_selection);
  FildeshKV map[1];
  size_t i;
  DECLARE_FildeshAT(Entry, entries);

  /* First byte determines what kind of FildeshKV we use.*/
  *map = initial_map(map_selection);
  initialize_entries(entries, data, size);

  for (i = 0; i < count_of_FildeshAT(entries); ++i) {
    const Entry e = (*entries)[i];
    const FildeshKV_id_t id = ensure_FildeshKV(map, e.key, e.ksize);
    assign_at_FildeshKV(map, id, e.value, e.vsize);

    fildesh_log_tracef("%u %u", (unsigned)*e.key, (unsigned)e.ksize);
    {
      const size_t result_size = size_of_key_at_FildeshKV(map, id);
      const void* const result_key = key_at_FildeshKV(map, id);
      const void* const result_value = value_at_FildeshKV(map, id);
      assert(result_size == e.ksize);
      assert(0 == memcmp(result_key, e.key, e.ksize));
      assert(0 == memcmp(result_value, e.value, e.vsize));
    }

    if (is_rbtree) {
      validate_FildeshKV_RBTREE(map);
    }
    if (is_broadleaf_rbtree) {
      validate_growing_FildeshKV_BROADLEAF_RBTREE(map);
    }
  }

  for (i = 0; i < count_of_FildeshAT(entries); ++i) {
    const Entry e = (*entries)[i];
    const FildeshKV_id_t id = lookup_FildeshKV(map, e.key, e.ksize);
    assert(!fildesh_nullid(id));
    assign_memref_at_FildeshKV(map, id, e.value);
  }

  for (i = 0; i < count_of_FildeshAT(entries); ++i) {
    const Entry e = (*entries)[i];
    const FildeshKV_id_t id = lookup_FildeshKV(map, e.key, e.ksize);
    assert(!fildesh_nullid(id));
    if ((const void*)e.value == value_at_FildeshKV(map, id)) {
      const void* const result_value = value_at_FildeshKV(map, id);
      assert(0 == memcmp(result_value, e.value, e.vsize));
      remove_at_FildeshKV(map, id);
    }
  }

  /* Map should be empty at the end.*/
  {
    FildeshKV_id_t id = first_FildeshKV(map);
    assert(fildesh_nullid(id));
  }

  close_FildeshAT(entries);
  close_FildeshKV(map);
  return 0;
}

