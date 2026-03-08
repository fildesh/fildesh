#include <fildesh/sxproto.h>
#include "src/sxproto/value.h"
#include <string.h>

static void build_node_2(FildeshSxpb* sxpb, FildeshSxpbIT parent_it) {
  FildeshSxpbIT it = parent_it;
  it.cons_id = parent_it.elem_id;
  it.elem_id = ~(FildeshSxpb_id)0;
  it = direct_insert_first_FildeshSxpb(sxpb, it, ensure_name_FildeshSxpb(sxpb, "+1", 2), FildeshSxprotoFieldKind_LITERAL_INT);
  it = direct_insert_next_FildeshSxpb(sxpb, it, ensure_name_FildeshSxpb(sxpb, "+2", 2), FildeshSxprotoFieldKind_LITERAL_INT);
  it = direct_insert_next_FildeshSxpb(sxpb, it, ensure_name_FildeshSxpb(sxpb, "+3", 2), FildeshSxprotoFieldKind_LITERAL_INT);
}

static void build_node_6(FildeshSxpb* sxpb, FildeshSxpbIT parent_it) {
  FildeshSxpbIT it = parent_it;
  it.cons_id = parent_it.elem_id;
  it.elem_id = ~(FildeshSxpb_id)0;
  it = direct_insert_first_FildeshSxpb(sxpb, it, ensure_name_FildeshSxpb(sxpb, "+1.e+0", 6), FildeshSxprotoFieldKind_LITERAL_FLOAT);
  it = direct_insert_next_FildeshSxpb(sxpb, it, ensure_name_FildeshSxpb(sxpb, "+1.e+2", 6), FildeshSxprotoFieldKind_LITERAL_FLOAT);
  it = direct_insert_next_FildeshSxpb(sxpb, it, ensure_name_FildeshSxpb(sxpb, "+1.e-3", 6), FildeshSxprotoFieldKind_LITERAL_FLOAT);
}

static void build_node_10(FildeshSxpb* sxpb, FildeshSxpbIT parent_it) {
  FildeshSxpbIT it = parent_it;
  it.cons_id = parent_it.elem_id;
  it.elem_id = ~(FildeshSxpb_id)0;
  it = direct_insert_first_FildeshSxpb(sxpb, it, ensure_name_FildeshSxpb(sxpb, "yo", 2), FildeshSxprotoFieldKind_LITERAL_STRING);
  it = direct_insert_next_FildeshSxpb(sxpb, it, ensure_name_FildeshSxpb(sxpb, "howdy", 5), FildeshSxprotoFieldKind_LITERAL_STRING);
  it = direct_insert_next_FildeshSxpb(sxpb, it, ensure_name_FildeshSxpb(sxpb, "moin", 4), FildeshSxprotoFieldKind_LITERAL_STRING);
  it = direct_insert_next_FildeshSxpb(sxpb, it, ensure_name_FildeshSxpb(sxpb, "ciao", 4), FildeshSxprotoFieldKind_LITERAL_STRING);
  it = direct_insert_next_FildeshSxpb(sxpb, it, ensure_name_FildeshSxpb(sxpb, "adios", 5), FildeshSxprotoFieldKind_LITERAL_STRING);
  it = direct_insert_next_FildeshSxpb(sxpb, it, ensure_name_FildeshSxpb(sxpb, "farewell", 8), FildeshSxprotoFieldKind_LITERAL_STRING);
}

static void build_node_18(FildeshSxpb* sxpb, FildeshSxpbIT parent_it) {
  FildeshSxpbIT it = parent_it;
  it.cons_id = parent_it.elem_id;
  it.elem_id = ~(FildeshSxpb_id)0;
  it = direct_insert_first_FildeshSxpb(sxpb, it, ensure_name_FildeshSxpb(sxpb, "i", 1), FildeshSxprotoFieldKind_LITERAL);
  direct_insert_first_FildeshSxpb(sxpb, it, ensure_name_FildeshSxpb(sxpb, "+5", 2), FildeshSxprotoFieldKind_LITERAL_INT);
}

static void build_node_22(FildeshSxpb* sxpb, FildeshSxpbIT parent_it) {
  FildeshSxpbIT it = parent_it;
  it.cons_id = parent_it.elem_id;
  it.elem_id = ~(FildeshSxpb_id)0;
  it = direct_insert_first_FildeshSxpb(sxpb, it, ensure_name_FildeshSxpb(sxpb, "i", 1), FildeshSxprotoFieldKind_LITERAL);
  direct_insert_first_FildeshSxpb(sxpb, it, ensure_name_FildeshSxpb(sxpb, "+5", 2), FildeshSxprotoFieldKind_LITERAL_INT);
  it = direct_insert_next_FildeshSxpb(sxpb, it, ensure_name_FildeshSxpb(sxpb, "f", 1), FildeshSxprotoFieldKind_LITERAL);
  direct_insert_first_FildeshSxpb(sxpb, it, ensure_name_FildeshSxpb(sxpb, "+5.5e+0", 7), FildeshSxprotoFieldKind_LITERAL_FLOAT);
  it = direct_insert_next_FildeshSxpb(sxpb, it, ensure_name_FildeshSxpb(sxpb, "s", 1), FildeshSxprotoFieldKind_LITERAL);
  direct_insert_first_FildeshSxpb(sxpb, it, ensure_name_FildeshSxpb(sxpb, "hello", 5), FildeshSxprotoFieldKind_LITERAL_STRING);
}

static void build_node_17(FildeshSxpb* sxpb, FildeshSxpbIT parent_it) {
  FildeshSxpbIT it = parent_it;
  it.cons_id = parent_it.elem_id;
  it.elem_id = ~(FildeshSxpb_id)0;
  it = direct_insert_first_FildeshSxpb(sxpb, it, ensure_name_FildeshSxpb(sxpb, "", 0), FildeshSxprotoFieldKind_MESSAGE);
  build_node_18(sxpb, it);
  it = direct_insert_next_FildeshSxpb(sxpb, it, ensure_name_FildeshSxpb(sxpb, "", 0), FildeshSxprotoFieldKind_MESSAGE);
  it = direct_insert_next_FildeshSxpb(sxpb, it, ensure_name_FildeshSxpb(sxpb, "", 0), FildeshSxprotoFieldKind_MESSAGE);
  build_node_22(sxpb, it);
}

static void build_node_1(FildeshSxpb* sxpb, FildeshSxpbIT parent_it) {
  FildeshSxpbIT it = parent_it;
  it.cons_id = parent_it.elem_id;
  it.elem_id = ~(FildeshSxpb_id)0;
  it = direct_insert_first_FildeshSxpb(sxpb, it, ensure_name_FildeshSxpb(sxpb, "my_integers", 11), FildeshSxprotoFieldKind_ARRAY);
  build_node_2(sxpb, it);
  it = direct_insert_next_FildeshSxpb(sxpb, it, ensure_name_FildeshSxpb(sxpb, "my_floats", 9), FildeshSxprotoFieldKind_ARRAY);
  build_node_6(sxpb, it);
  it = direct_insert_next_FildeshSxpb(sxpb, it, ensure_name_FildeshSxpb(sxpb, "my_strings", 10), FildeshSxprotoFieldKind_ARRAY);
  build_node_10(sxpb, it);
  it = direct_insert_next_FildeshSxpb(sxpb, it, ensure_name_FildeshSxpb(sxpb, "my_messages", 11), FildeshSxprotoFieldKind_ARRAY);
  build_node_17(sxpb, it);
  it = direct_insert_next_FildeshSxpb(sxpb, it, ensure_name_FildeshSxpb(sxpb, "my_empty_array", 14), FildeshSxprotoFieldKind_ARRAY);
}

FildeshSxpb* make_array_test_FildeshSxpb(void) {
  FildeshSxpb* sxpb = open_FildeshSxpb();
  FildeshSxpbIT top = top_of_FildeshSxpb(sxpb);
  top.elem_id = 0;
  build_node_1(sxpb, top);
  return sxpb;
}
