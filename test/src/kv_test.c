
#include "src/lib/kve.h"
#include <assert.h>
#include <stdio.h>

static void print_whole_thing(const FildeshKV* map) {
  size_t i;
  size_t n = fildesh_size_of_lgcount(1, map->allocated_lgcount);
  for (i = 0; i < n; ++i) {
    const unsigned* p1 = (unsigned*) value_FildeshKVE(&map->at[i]);
    const unsigned* p2 = (unsigned*) splitvalue_FildeshKVE(&map->at[i]);
    fprintf(stderr, "  %u %u", p1 ? *p1 : 0, p2 ? *p2 : 0);
  }
  fputc('\n', stderr);
}

static void add_remove_in_order_test()
{
  FildeshKV map[1] = {DEFAULT_FildeshKV_SINGLE_LIST};
  const unsigned n = 10;
  unsigned i;
  for (i = 0; i < n; ++i) {
    const unsigned k = i;
    unsigned v = i+10;
    const FildeshKV_id_t id = ensure_FildeshKV(map, &k, sizeof(k));
    unsigned* p;
    assert(!fildesh_nullid(id));
    assign_at_FildeshKV(map, id, &v, sizeof(v));
    p = (unsigned*)value_at_FildeshKV(map, id);
    assert(*p == v);
    assert(p != &v);
    print_whole_thing(map);
  }
  for (i = 0; i < n; ++i) {
    const unsigned k = i;
    const unsigned expected_v = i+10;
    const FildeshKV_id_t id = lookup_FildeshKV(map, &k, sizeof(k));
    unsigned* p;
    assert(!fildesh_nullid(id));
    p = (unsigned*) value_at_FildeshKV(map, id);
    assert(*p == expected_v);
    remove_at_FildeshKV(map, id);
    print_whole_thing(map);
  }
  close_FildeshKV(map);
}

int main() {
  add_remove_in_order_test();
  return 0;
}
