#include <assert.h>
#include <string.h>

#include "src/lib/kv/kv.h"
#include "test/lib/kv/rbtree_validation.h"

/**
 * Rigorously test red-black tree with different combinations of insert
 * and remove operations.
 * Set up and tear down a tree of 26 strings (ascii letters) in many different
 * orders. To ensure many orders, use sequential multiples of coprimes with 26
 * to index the array of keys.
 *
 * Ex: The following sequence is generated from the first 26 multiples of 3.
 * 0 3 6 9 12 15 18 21 24 1 4 7 10 13 16 19 22 25 2 5 8 11 14 17 20 23
 **/
static
  void
combination_test()
{
  static const char* const keys[] = {
    "a", "b", "c", "d", "e", "f", "g",
    "h", "i", "j", "k", "l", "m", "n",
    "o", "p", "q", "r", "s", "t", "u",
    "v", "w", "x", "y", "z"
  };
  static const unsigned muls[] = {
    1, 3, 5, 7, 9, 11, 15, 17, 19, 21
  };
  const unsigned nkeys = sizeof(keys) / sizeof(*keys);
  const unsigned nmuls = sizeof(muls) / sizeof(*muls);
  unsigned mi, mj, i;
  FildeshKV map[1] = {DEFAULT_FildeshKV_RBTREE};

  for (mi = 0; mi < nmuls; ++mi) {
    for (mj = 0; mj < nmuls; ++mj) {
      for (i = 0; i < nkeys; ++i) {
        const unsigned idx = (muls[mi] * i) % nkeys;
        FildeshKV_id_t id = ensure_FildeshKV(map, keys[idx], 1);
        assert(!value_at_FildeshKV(map, id));
        assign_at_FildeshKV(map, id, &idx, sizeof(unsigned));
        validate_FildeshKV_RBTREE(map);
      }
      for (i = 0; i < nkeys; ++i) {
        const unsigned idx = (muls[mj] * i) % nkeys;
        FildeshKV_id_t id = lookup_FildeshKV(map, keys[idx], 1);
        assert(!fildesh_nullid(id));
        remove_at_FildeshKV(map, id);
        validate_FildeshKV_RBTREE(map);
      }
    }
  }

  close_FildeshKV(map);
}

static
  void
print_debug_test()
{
  FildeshO out[1] = {DEFAULT_FildeshO};
  const char expect_content[] =
    "10: red 1 -> 0\n"
    "12: red 2 -> 0\n"
    "11: black 0 -> NULL\n"
    ;
  FildeshKV map[1] = {DEFAULT_FildeshKV_RBTREE};

  ensure_FildeshKV(map, "\x0b", 1);
  ensure_FildeshKV(map, "\x0a", 1);
  ensure_FildeshKV(map, "\x0c", 1);
  print_debug_FildeshKV_RBTREE(map, out);

  assert(strlen(expect_content) == out->size);
  assert(0 == memcmp(expect_content, out->at, out->size));

  close_FildeshKV(map);
  close_FildeshO(out);
}

static
  void
print_graphviz_test()
{
  const char expect_content[] =
    "digraph tree {\n"
    "q1 [label = \"10\", color = \"red\"];\n"
    "q2 [label = \"12\", color = \"red\"];\n"
    "q0 [label = \"11\", color = \"black\"];\n"
    "q1 -> q0;\n"
    "q2 -> q0;\n"
    "}\n"
    ;
  FildeshO out[1] = {DEFAULT_FildeshO};
  FildeshKV map[1] = {DEFAULT_FildeshKV_RBTREE};

  ensure_FildeshKV(map, "\x0b", 1);
  ensure_FildeshKV(map, "\x0a", 1);
  ensure_FildeshKV(map, "\x0c", 1);
  print_graphviz_FildeshKV_RBTREE(map, out);

  assert(strlen(expect_content) == out->size);
  assert(0 == memcmp(expect_content, out->at, out->size));

  close_FildeshKV(map);
  close_FildeshO(out);
}

int main() {
  combination_test();
  print_debug_test();
  print_graphviz_test();
  return 0;
}
