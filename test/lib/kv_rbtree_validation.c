#include "kv_rbtree_validation.h"

#include "src/lib/kv_bstree.h"

  void
print_debug_FildeshKV_RBTREE(const FildeshKV* map, FildeshO* out)
{
  FildeshKV_id_t id;
  for (id = first_FildeshKV(map); !fildesh_nullid(id);
       id = next_at_FildeshKV(map, id))
  {
    size_t x = id/2;
    print_int_FildeshO(out, (unsigned)*(char*)key_at_FildeshKV(map, id));
    puts_FildeshO(out, ": ");
    puts_FildeshO(out, (RedColorOf(x) ? "red" : "black"));
    putc_FildeshO(out, ' ');
    print_int_FildeshO(out, (unsigned)x);
    puts_FildeshO(out, " -> ");
    if (IsRoot(x)) {
      puts_FildeshO(out, "NULL");
    }
    else {
      print_int_FildeshO(out, (unsigned)JointOf(x));
    }
    putc_FildeshO(out, '\n');
  }
}

  void
print_graphviz_FildeshKV_RBTREE(const FildeshKV* map, FildeshO* out)
{
  FildeshKV_id_t id;
  puts_FildeshO(out, "digraph tree {\n");

  for (id = first_FildeshKV(map); !fildesh_nullid(id);
       id = next_at_FildeshKV(map, id))
  {
    size_t x = id/2;
    if ((id & 1) != 0) {continue;}
    putc_FildeshO(out, 'q');
    print_int_FildeshO(out, (int)x);
    puts_FildeshO(out, " [label = \"");
    print_int_FildeshO(out, (unsigned)*(char*)key_at_FildeshKV(map, id));
    puts_FildeshO(out, "\", color = \"");
    puts_FildeshO(out, (RedColorOf(x) ? "red" : "black"));
    puts_FildeshO(out, "\"];\n");
  }

  for (id = first_FildeshKV(map); !fildesh_nullid(id);
       id = next_at_FildeshKV(map, id))
  {
    size_t x = id/2;
    if ((id & 1) != 0) {continue;}
    if (IsRoot(x)) {continue;}
    putc_FildeshO(out, 'q');
    print_int_FildeshO(out, (int)x);
    puts_FildeshO(out, " -> q");
    print_int_FildeshO(out, (int)JointOf(x));
    puts_FildeshO(out, ";\n");
  }

  puts_FildeshO(out, "}\n");
}

static
  unsigned
count_up_black(const FildeshKV* map, size_t x) {
  unsigned n = 0;
  do {
    if (!RedColorOf(x)) {
      n += 1;
    }
    x = JointOf(x);
  } while (!Nullish(x));
  return n;
}

  bool
validate_FildeshKV_RBTREE(const FildeshKV* map)
{
  unsigned black_count;
  FildeshKV_id_t id = first_FildeshKV(map);
  if (fildesh_nullid(id)) {
    return true;
  }
  black_count = count_up_black(map, id/2);
  do {
    if ((id & 1) == 0) {
      size_t x = id/2;
      if (IsRoot(x)) {
        assert(!RedColorOf(x));
      }
      else {
        assert(!RedColorOf(x) || !RedColorOf(JointOf(x)));
      }
      if (Nullish(SplitOf(x, 0)) && Nullish(SplitOf(x, 1))) {
        assert(black_count == count_up_black(map, x));
      }
    }
    id = next_at_FildeshKV(map, id);
  } while (!fildesh_nullid(id));
  return (black_count > 0);
}
