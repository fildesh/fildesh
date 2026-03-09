#include <fildesh/sxproto.h>

#include "src/sxproto/value.h"

static void make_node_100(FildeshSxpb* sxpb, FildeshSxpbIT it) {
  it = direct_insert_first_FildeshSxpb(sxpb, it, ensure_name_FildeshSxpb(sxpb, "paper", 5), FildeshSxprotoFieldKind_LITERAL);
  direct_insert_first_FildeshSxpb(sxpb, it, ensure_name_FildeshSxpb(sxpb, "+2", 2), FildeshSxprotoFieldKind_LITERAL_INT);
}

static void make_node_210(FildeshSxpb* sxpb, FildeshSxpbIT it) {
  it = direct_insert_first_FildeshSxpb(sxpb, it, ensure_name_FildeshSxpb(sxpb, "count", 5), FildeshSxprotoFieldKind_LITERAL);
  direct_insert_first_FildeshSxpb(sxpb, it, ensure_name_FildeshSxpb(sxpb, "+5", 2), FildeshSxprotoFieldKind_LITERAL_INT);
  it = direct_insert_next_FildeshSxpb(sxpb, it, ensure_name_FildeshSxpb(sxpb, "ripeness", 8), FildeshSxprotoFieldKind_LITERAL);
  direct_insert_first_FildeshSxpb(sxpb, it, ensure_name_FildeshSxpb(sxpb, "+4.e-1", 6), FildeshSxprotoFieldKind_LITERAL_FLOAT);
}

static void make_node_200(FildeshSxpb* sxpb, FildeshSxpbIT it) {
  it = direct_insert_first_FildeshSxpb(sxpb, it, ensure_name_FildeshSxpb(sxpb, "banana", 6), FildeshSxprotoFieldKind_MESSAGE);
  make_node_210(sxpb, it);
}


FildeshSxpb* make_loneof_test_FildeshSxpb(void) {
  FildeshSxpb* sxpb = open_FildeshSxpb();
  FildeshSxpbIT it = top_of_FildeshSxpb(sxpb);
  it = direct_insert_first_FildeshSxpb(sxpb, it, ensure_name_FildeshSxpb(sxpb, "bag_with", 8), FildeshSxprotoFieldKind_LONEOF);
  make_node_100(sxpb, it);
  it = direct_insert_next_FildeshSxpb(sxpb, it, ensure_name_FildeshSxpb(sxpb, "fruit_as", 8), FildeshSxprotoFieldKind_LONEOF);
  make_node_200(sxpb, it);
  return sxpb;
}
