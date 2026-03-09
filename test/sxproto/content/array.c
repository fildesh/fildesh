#include <fildesh/sxproto.h>

#include "src/sxproto/value.h"

static void add_node_100(FildeshSxpb* sxpb, FildeshSxpbIT it) {
  it = direct_insert_first_FildeshSxpb(sxpb, it, ensure_name_FildeshSxpb(sxpb, "+1", 2), FildeshSxprotoFieldKind_LITERAL_INT);
  it = direct_insert_next_FildeshSxpb(sxpb, it, ensure_name_FildeshSxpb(sxpb, "+2", 2), FildeshSxprotoFieldKind_LITERAL_INT);
  it = direct_insert_next_FildeshSxpb(sxpb, it, ensure_name_FildeshSxpb(sxpb, "+3", 2), FildeshSxprotoFieldKind_LITERAL_INT);
}

static void add_node_200(FildeshSxpb* sxpb, FildeshSxpbIT it) {
  it = direct_insert_first_FildeshSxpb(sxpb, it, ensure_name_FildeshSxpb(sxpb, "+1.e+0", 6), FildeshSxprotoFieldKind_LITERAL_FLOAT);
  it = direct_insert_next_FildeshSxpb(sxpb, it, ensure_name_FildeshSxpb(sxpb, "+1.e+2", 6), FildeshSxprotoFieldKind_LITERAL_FLOAT);
  it = direct_insert_next_FildeshSxpb(sxpb, it, ensure_name_FildeshSxpb(sxpb, "+1.e-3", 6), FildeshSxprotoFieldKind_LITERAL_FLOAT);
}

static void add_node_300(FildeshSxpb* sxpb, FildeshSxpbIT it) {
  it = direct_insert_first_FildeshSxpb(sxpb, it, ensure_name_FildeshSxpb(sxpb, "yo", 2), FildeshSxprotoFieldKind_LITERAL_STRING);
  it = direct_insert_next_FildeshSxpb(sxpb, it, ensure_name_FildeshSxpb(sxpb, "howdy", 5), FildeshSxprotoFieldKind_LITERAL_STRING);
  it = direct_insert_next_FildeshSxpb(sxpb, it, ensure_name_FildeshSxpb(sxpb, "moin", 4), FildeshSxprotoFieldKind_LITERAL_STRING);
  it = direct_insert_next_FildeshSxpb(sxpb, it, ensure_name_FildeshSxpb(sxpb, "ciao", 4), FildeshSxprotoFieldKind_LITERAL_STRING);
  it = direct_insert_next_FildeshSxpb(sxpb, it, ensure_name_FildeshSxpb(sxpb, "adios", 5), FildeshSxprotoFieldKind_LITERAL_STRING);
  it = direct_insert_next_FildeshSxpb(sxpb, it, ensure_name_FildeshSxpb(sxpb, "farewell", 8), FildeshSxprotoFieldKind_LITERAL_STRING);
}

static void add_node_410(FildeshSxpb* sxpb, FildeshSxpbIT it) {
  it = direct_insert_first_FildeshSxpb(sxpb, it, ensure_name_FildeshSxpb(sxpb, "i", 1), FildeshSxprotoFieldKind_LITERAL);
  direct_insert_first_FildeshSxpb(sxpb, it, ensure_name_FildeshSxpb(sxpb, "+5", 2), FildeshSxprotoFieldKind_LITERAL_INT);
}

static void add_node_420(FildeshSxpb* sxpb, FildeshSxpbIT it) {
  it = direct_insert_first_FildeshSxpb(sxpb, it, ensure_name_FildeshSxpb(sxpb, "i", 1), FildeshSxprotoFieldKind_LITERAL);
  direct_insert_first_FildeshSxpb(sxpb, it, ensure_name_FildeshSxpb(sxpb, "+5", 2), FildeshSxprotoFieldKind_LITERAL_INT);
  it = direct_insert_next_FildeshSxpb(sxpb, it, ensure_name_FildeshSxpb(sxpb, "f", 1), FildeshSxprotoFieldKind_LITERAL);
  direct_insert_first_FildeshSxpb(sxpb, it, ensure_name_FildeshSxpb(sxpb, "+5.5e+0", 7), FildeshSxprotoFieldKind_LITERAL_FLOAT);
  it = direct_insert_next_FildeshSxpb(sxpb, it, ensure_name_FildeshSxpb(sxpb, "s", 1), FildeshSxprotoFieldKind_LITERAL);
  direct_insert_first_FildeshSxpb(sxpb, it, ensure_name_FildeshSxpb(sxpb, "hello", 5), FildeshSxprotoFieldKind_LITERAL_STRING);
}

static void add_node_400(FildeshSxpb* sxpb, FildeshSxpbIT it) {
  it = direct_insert_first_FildeshSxpb(sxpb, it, ensure_name_FildeshSxpb(sxpb, "", 0), FildeshSxprotoFieldKind_MESSAGE);
  add_node_410(sxpb, it);
  it = direct_insert_next_FildeshSxpb(sxpb, it, ensure_name_FildeshSxpb(sxpb, "", 0), FildeshSxprotoFieldKind_MESSAGE);
  it = direct_insert_next_FildeshSxpb(sxpb, it, ensure_name_FildeshSxpb(sxpb, "", 0), FildeshSxprotoFieldKind_MESSAGE);
  add_node_420(sxpb, it);
}

static void add_node_000(FildeshSxpb* sxpb, FildeshSxpbIT it) {
  it = direct_insert_first_FildeshSxpb(sxpb, it, ensure_name_FildeshSxpb(sxpb, "my_integers", 11), FildeshSxprotoFieldKind_ARRAY);
  add_node_100(sxpb, it);
  it = direct_insert_next_FildeshSxpb(sxpb, it, ensure_name_FildeshSxpb(sxpb, "my_floats", 9), FildeshSxprotoFieldKind_ARRAY);
  add_node_200(sxpb, it);
  it = direct_insert_next_FildeshSxpb(sxpb, it, ensure_name_FildeshSxpb(sxpb, "my_strings", 10), FildeshSxprotoFieldKind_ARRAY);
  add_node_300(sxpb, it);
  it = direct_insert_next_FildeshSxpb(sxpb, it, ensure_name_FildeshSxpb(sxpb, "my_messages", 11), FildeshSxprotoFieldKind_ARRAY);
  add_node_400(sxpb, it);
  it = direct_insert_next_FildeshSxpb(sxpb, it, ensure_name_FildeshSxpb(sxpb, "my_empty_array", 14), FildeshSxprotoFieldKind_ARRAY);
}

FildeshSxpb* make_array_test_FildeshSxpb(void) {
  FildeshSxpb* sxpb = open_FildeshSxpb();
  FildeshSxpbIT top = top_of_FildeshSxpb(sxpb);
  top.elem_id = 0;
  add_node_000(sxpb, top);
  return sxpb;
}
