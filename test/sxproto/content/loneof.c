#include <fildesh/sxproto.h>
#include "src/sxproto/value.h"
#include <string.h>

static void build_node_1(FildeshSxpb* sxpb, FildeshSxpbIT parent_it);
static void build_node_2(FildeshSxpb* sxpb, FildeshSxpbIT parent_it);
static void build_node_5(FildeshSxpb* sxpb, FildeshSxpbIT parent_it);
static void build_node_6(FildeshSxpb* sxpb, FildeshSxpbIT parent_it);

static void build_node_2(FildeshSxpb* sxpb, FildeshSxpbIT parent_it) {
  FildeshSxpbIT it = parent_it;
  it.cons_id = parent_it.elem_id;
  it.elem_id = ~(FildeshSxpb_id)0;
  it = direct_insert_first_FildeshSxpb(sxpb, it, ensure_name_FildeshSxpb(sxpb, "paper", 5), FildeshSxprotoFieldKind_LITERAL);
  direct_insert_first_FildeshSxpb(sxpb, it, ensure_name_FildeshSxpb(sxpb, "+2", 2), FildeshSxprotoFieldKind_LITERAL_INT);
}

static void build_node_6(FildeshSxpb* sxpb, FildeshSxpbIT parent_it) {
  FildeshSxpbIT it = parent_it;
  it.cons_id = parent_it.elem_id;
  it.elem_id = ~(FildeshSxpb_id)0;
  it = direct_insert_first_FildeshSxpb(sxpb, it, ensure_name_FildeshSxpb(sxpb, "count", 5), FildeshSxprotoFieldKind_LITERAL);
  direct_insert_first_FildeshSxpb(sxpb, it, ensure_name_FildeshSxpb(sxpb, "+5", 2), FildeshSxprotoFieldKind_LITERAL_INT);
  it = direct_insert_next_FildeshSxpb(sxpb, it, ensure_name_FildeshSxpb(sxpb, "ripeness", 8), FildeshSxprotoFieldKind_LITERAL);
  direct_insert_first_FildeshSxpb(sxpb, it, ensure_name_FildeshSxpb(sxpb, "+4.e-1", 6), FildeshSxprotoFieldKind_LITERAL_FLOAT);
}

static void build_node_5(FildeshSxpb* sxpb, FildeshSxpbIT parent_it) {
  FildeshSxpbIT it = parent_it;
  it.cons_id = parent_it.elem_id;
  it.elem_id = ~(FildeshSxpb_id)0;
  it = direct_insert_first_FildeshSxpb(sxpb, it, ensure_name_FildeshSxpb(sxpb, "banana", 6), FildeshSxprotoFieldKind_MESSAGE);
  build_node_6(sxpb, it);
}

static void build_node_1(FildeshSxpb* sxpb, FildeshSxpbIT parent_it) {
  FildeshSxpbIT it = parent_it;
  it.cons_id = parent_it.elem_id;
  it.elem_id = ~(FildeshSxpb_id)0;
  it = direct_insert_first_FildeshSxpb(sxpb, it, ensure_name_FildeshSxpb(sxpb, "bag_with", 8), FildeshSxprotoFieldKind_LONEOF);
  build_node_2(sxpb, it);
  it = direct_insert_next_FildeshSxpb(sxpb, it, ensure_name_FildeshSxpb(sxpb, "fruit_as", 8), FildeshSxprotoFieldKind_LONEOF);
  build_node_5(sxpb, it);
}

FildeshSxpb* make_loneof_test_FildeshSxpb(void) {
  FildeshSxpb* sxpb = open_FildeshSxpb();
  FildeshSxpbIT top = top_of_FildeshSxpb(sxpb);
  top.elem_id = 0;
  build_node_1(sxpb, top);
  return sxpb;
}
