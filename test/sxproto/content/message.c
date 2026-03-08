#include <fildesh/sxproto.h>
#include "src/sxproto/value.h"
#include <string.h>

static void build_node_1(FildeshSxpb* sxpb, FildeshSxpbIT parent_it);
static void build_node_8(FildeshSxpb* sxpb, FildeshSxpbIT parent_it);

static void build_node_8(FildeshSxpb* sxpb, FildeshSxpbIT parent_it) {
  FildeshSxpbIT it = parent_it;
  it.cons_id = parent_it.elem_id;
  it.elem_id = ~(FildeshSxpb_id)0;
  it = direct_insert_first_FildeshSxpb(sxpb, it, ensure_name_FildeshSxpb(sxpb, "i", 1), FildeshSxprotoFieldKind_LITERAL);
  direct_insert_first_FildeshSxpb(sxpb, it, ensure_name_FildeshSxpb(sxpb, "-5", 2), FildeshSxprotoFieldKind_LITERAL_INT);
  it = direct_insert_next_FildeshSxpb(sxpb, it, ensure_name_FildeshSxpb(sxpb, "f", 1), FildeshSxprotoFieldKind_LITERAL);
  direct_insert_first_FildeshSxpb(sxpb, it, ensure_name_FildeshSxpb(sxpb, "-5.5e+0", 7), FildeshSxprotoFieldKind_LITERAL_FLOAT);
  it = direct_insert_next_FildeshSxpb(sxpb, it, ensure_name_FildeshSxpb(sxpb, "s", 1), FildeshSxprotoFieldKind_LITERAL);
  direct_insert_first_FildeshSxpb(sxpb, it, ensure_name_FildeshSxpb(sxpb, "goodbye", 7), FildeshSxprotoFieldKind_LITERAL_STRING);
}

static void build_node_1(FildeshSxpb* sxpb, FildeshSxpbIT parent_it) {
  FildeshSxpbIT it = parent_it;
  it.cons_id = parent_it.elem_id;
  it.elem_id = ~(FildeshSxpb_id)0;
  it = direct_insert_first_FildeshSxpb(sxpb, it, ensure_name_FildeshSxpb(sxpb, "i", 1), FildeshSxprotoFieldKind_LITERAL);
  direct_insert_first_FildeshSxpb(sxpb, it, ensure_name_FildeshSxpb(sxpb, "+5", 2), FildeshSxprotoFieldKind_LITERAL_INT);
  it = direct_insert_next_FildeshSxpb(sxpb, it, ensure_name_FildeshSxpb(sxpb, "f", 1), FildeshSxprotoFieldKind_LITERAL);
  direct_insert_first_FildeshSxpb(sxpb, it, ensure_name_FildeshSxpb(sxpb, "+5.5e+0", 7), FildeshSxprotoFieldKind_LITERAL_FLOAT);
  it = direct_insert_next_FildeshSxpb(sxpb, it, ensure_name_FildeshSxpb(sxpb, "s", 1), FildeshSxprotoFieldKind_LITERAL);
  direct_insert_first_FildeshSxpb(sxpb, it, ensure_name_FildeshSxpb(sxpb, "hello", 5), FildeshSxprotoFieldKind_LITERAL_STRING);
  it = direct_insert_next_FildeshSxpb(sxpb, it, ensure_name_FildeshSxpb(sxpb, "m", 1), FildeshSxprotoFieldKind_MESSAGE);
  build_node_8(sxpb, it);
  it = direct_insert_next_FildeshSxpb(sxpb, it, ensure_name_FildeshSxpb(sxpb, "empty_message", 13), FildeshSxprotoFieldKind_MESSAGE);
}

FildeshSxpb* make_message_test_FildeshSxpb(void) {
  FildeshSxpb* sxpb = open_FildeshSxpb();
  FildeshSxpbIT top = top_of_FildeshSxpb(sxpb);
  top.elem_id = 0;
  build_node_1(sxpb, top);
  return sxpb;
}
