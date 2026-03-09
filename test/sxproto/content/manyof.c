#include <fildesh/sxproto.h>

#include "src/sxproto/value.h"

static void make_node_120(FildeshSxpb* sxpb, FildeshSxpbIT it) {
  it = direct_insert_first_FildeshSxpb(sxpb, it, ensure_name_FildeshSxpb(sxpb, "+1", 2), FildeshSxprotoFieldKind_LITERAL_INT);
  it = direct_insert_next_FildeshSxpb(sxpb, it, ensure_name_FildeshSxpb(sxpb, "+2", 2), FildeshSxprotoFieldKind_LITERAL_INT);
}

static void make_node_130(FildeshSxpb* sxpb, FildeshSxpbIT it) {
  it = direct_insert_first_FildeshSxpb(sxpb, it, ensure_name_FildeshSxpb(sxpb, "+3", 2), FildeshSxprotoFieldKind_LITERAL_INT);
  it = direct_insert_next_FildeshSxpb(sxpb, it, ensure_name_FildeshSxpb(sxpb, "+4", 2), FildeshSxprotoFieldKind_LITERAL_INT);
}

static void make_node_150(FildeshSxpb* sxpb, FildeshSxpbIT it) {
  it = direct_insert_first_FildeshSxpb(sxpb, it, ensure_name_FildeshSxpb(sxpb, "+56", 3), FildeshSxprotoFieldKind_LITERAL_INT);
  it = direct_insert_next_FildeshSxpb(sxpb, it, ensure_name_FildeshSxpb(sxpb, "+7", 2), FildeshSxprotoFieldKind_LITERAL_INT);
}

static void make_node_140(FildeshSxpb* sxpb, FildeshSxpbIT it) {
  it = direct_insert_first_FildeshSxpb(sxpb, it, ensure_name_FildeshSxpb(sxpb, "div", 3), FildeshSxprotoFieldKind_MANYOF);
  make_node_150(sxpb, it);
}

static void make_node_110(FildeshSxpb* sxpb, FildeshSxpbIT it) {
  it = direct_insert_first_FildeshSxpb(sxpb, it, ensure_name_FildeshSxpb(sxpb, "add", 3), FildeshSxprotoFieldKind_MANYOF);
  make_node_120(sxpb, it);
  it = direct_insert_next_FildeshSxpb(sxpb, it, ensure_name_FildeshSxpb(sxpb, "mul", 3), FildeshSxprotoFieldKind_MANYOF);
  make_node_130(sxpb, it);
  it = direct_insert_next_FildeshSxpb(sxpb, it, ensure_name_FildeshSxpb(sxpb, "neg", 3), FildeshSxprotoFieldKind_MESSAGE);
  make_node_140(sxpb, it);
}

static void make_node_170(FildeshSxpb* sxpb, FildeshSxpbIT it) {
  it = direct_insert_first_FildeshSxpb(sxpb, it, ensure_name_FildeshSxpb(sxpb, "value", 5), FildeshSxprotoFieldKind_LITERAL);
  direct_insert_first_FildeshSxpb(sxpb, it, ensure_name_FildeshSxpb(sxpb, "+1", 2), FildeshSxprotoFieldKind_LITERAL_INT);
  it = direct_insert_next_FildeshSxpb(sxpb, it, ensure_name_FildeshSxpb(sxpb, "value", 5), FildeshSxprotoFieldKind_LITERAL);
  direct_insert_first_FildeshSxpb(sxpb, it, ensure_name_FildeshSxpb(sxpb, "+2", 2), FildeshSxprotoFieldKind_LITERAL_INT);
}

static void make_node_180(FildeshSxpb* sxpb, FildeshSxpbIT it) {
  it = direct_insert_first_FildeshSxpb(sxpb, it, ensure_name_FildeshSxpb(sxpb, "value", 5), FildeshSxprotoFieldKind_LITERAL);
  direct_insert_first_FildeshSxpb(sxpb, it, ensure_name_FildeshSxpb(sxpb, "+3", 2), FildeshSxprotoFieldKind_LITERAL_INT);
  it = direct_insert_next_FildeshSxpb(sxpb, it, ensure_name_FildeshSxpb(sxpb, "value", 5), FildeshSxprotoFieldKind_LITERAL);
  direct_insert_first_FildeshSxpb(sxpb, it, ensure_name_FildeshSxpb(sxpb, "+4", 2), FildeshSxprotoFieldKind_LITERAL_INT);
}

static void make_node_200(FildeshSxpb* sxpb, FildeshSxpbIT it) {
  it = direct_insert_first_FildeshSxpb(sxpb, it, ensure_name_FildeshSxpb(sxpb, "value", 5), FildeshSxprotoFieldKind_LITERAL);
  direct_insert_first_FildeshSxpb(sxpb, it, ensure_name_FildeshSxpb(sxpb, "+56", 3), FildeshSxprotoFieldKind_LITERAL_INT);
  it = direct_insert_next_FildeshSxpb(sxpb, it, ensure_name_FildeshSxpb(sxpb, "value", 5), FildeshSxprotoFieldKind_LITERAL);
  direct_insert_first_FildeshSxpb(sxpb, it, ensure_name_FildeshSxpb(sxpb, "+7", 2), FildeshSxprotoFieldKind_LITERAL_INT);
}

static void make_node_190(FildeshSxpb* sxpb, FildeshSxpbIT it) {
  it = direct_insert_first_FildeshSxpb(sxpb, it, ensure_name_FildeshSxpb(sxpb, "div", 3), FildeshSxprotoFieldKind_MANYOF);
  make_node_200(sxpb, it);
}

static void make_node_160(FildeshSxpb* sxpb, FildeshSxpbIT it) {
  it = direct_insert_first_FildeshSxpb(sxpb, it, ensure_name_FildeshSxpb(sxpb, "add", 3), FildeshSxprotoFieldKind_MANYOF);
  make_node_170(sxpb, it);
  it = direct_insert_next_FildeshSxpb(sxpb, it, ensure_name_FildeshSxpb(sxpb, "mul", 3), FildeshSxprotoFieldKind_MANYOF);
  make_node_180(sxpb, it);
  it = direct_insert_next_FildeshSxpb(sxpb, it, ensure_name_FildeshSxpb(sxpb, "neg", 3), FildeshSxprotoFieldKind_MESSAGE);
  make_node_190(sxpb, it);
}

static void make_node_240(FildeshSxpb* sxpb, FildeshSxpbIT it) {
  it = direct_insert_first_FildeshSxpb(sxpb, it, ensure_name_FildeshSxpb(sxpb, "value", 5), FildeshSxprotoFieldKind_LITERAL);
  direct_insert_first_FildeshSxpb(sxpb, it, ensure_name_FildeshSxpb(sxpb, "+1", 2), FildeshSxprotoFieldKind_LITERAL_INT);
}

static void make_node_250(FildeshSxpb* sxpb, FildeshSxpbIT it) {
  it = direct_insert_first_FildeshSxpb(sxpb, it, ensure_name_FildeshSxpb(sxpb, "value", 5), FildeshSxprotoFieldKind_LITERAL);
  direct_insert_first_FildeshSxpb(sxpb, it, ensure_name_FildeshSxpb(sxpb, "+2", 2), FildeshSxprotoFieldKind_LITERAL_INT);
}

static void make_node_230(FildeshSxpb* sxpb, FildeshSxpbIT it) {
  it = direct_insert_first_FildeshSxpb(sxpb, it, ensure_name_FildeshSxpb(sxpb, "", 0), FildeshSxprotoFieldKind_MESSAGE);
  make_node_240(sxpb, it);
  it = direct_insert_next_FildeshSxpb(sxpb, it, ensure_name_FildeshSxpb(sxpb, "", 0), FildeshSxprotoFieldKind_MESSAGE);
  make_node_250(sxpb, it);
}

static void make_node_220(FildeshSxpb* sxpb, FildeshSxpbIT it) {
  it = direct_insert_first_FildeshSxpb(sxpb, it, ensure_name_FildeshSxpb(sxpb, "add", 3), FildeshSxprotoFieldKind_ARRAY);
  make_node_230(sxpb, it);
}

static void make_node_280(FildeshSxpb* sxpb, FildeshSxpbIT it) {
  it = direct_insert_first_FildeshSxpb(sxpb, it, ensure_name_FildeshSxpb(sxpb, "value", 5), FildeshSxprotoFieldKind_LITERAL);
  direct_insert_first_FildeshSxpb(sxpb, it, ensure_name_FildeshSxpb(sxpb, "+3", 2), FildeshSxprotoFieldKind_LITERAL_INT);
}

static void make_node_290(FildeshSxpb* sxpb, FildeshSxpbIT it) {
  it = direct_insert_first_FildeshSxpb(sxpb, it, ensure_name_FildeshSxpb(sxpb, "value", 5), FildeshSxprotoFieldKind_LITERAL);
  direct_insert_first_FildeshSxpb(sxpb, it, ensure_name_FildeshSxpb(sxpb, "+4", 2), FildeshSxprotoFieldKind_LITERAL_INT);
}

static void make_node_270(FildeshSxpb* sxpb, FildeshSxpbIT it) {
  it = direct_insert_first_FildeshSxpb(sxpb, it, ensure_name_FildeshSxpb(sxpb, "", 0), FildeshSxprotoFieldKind_MESSAGE);
  make_node_280(sxpb, it);
  it = direct_insert_next_FildeshSxpb(sxpb, it, ensure_name_FildeshSxpb(sxpb, "", 0), FildeshSxprotoFieldKind_MESSAGE);
  make_node_290(sxpb, it);
}

static void make_node_260(FildeshSxpb* sxpb, FildeshSxpbIT it) {
  it = direct_insert_first_FildeshSxpb(sxpb, it, ensure_name_FildeshSxpb(sxpb, "mul", 3), FildeshSxprotoFieldKind_ARRAY);
  make_node_270(sxpb, it);
}

static void make_node_330(FildeshSxpb* sxpb, FildeshSxpbIT it) {
  it = direct_insert_first_FildeshSxpb(sxpb, it, ensure_name_FildeshSxpb(sxpb, "value", 5), FildeshSxprotoFieldKind_LITERAL);
  direct_insert_first_FildeshSxpb(sxpb, it, ensure_name_FildeshSxpb(sxpb, "+56", 3), FildeshSxprotoFieldKind_LITERAL_INT);
}

static void make_node_340(FildeshSxpb* sxpb, FildeshSxpbIT it) {
  it = direct_insert_first_FildeshSxpb(sxpb, it, ensure_name_FildeshSxpb(sxpb, "value", 5), FildeshSxprotoFieldKind_LITERAL);
  direct_insert_first_FildeshSxpb(sxpb, it, ensure_name_FildeshSxpb(sxpb, "+7", 2), FildeshSxprotoFieldKind_LITERAL_INT);
}

static void make_node_320(FildeshSxpb* sxpb, FildeshSxpbIT it) {
  it = direct_insert_first_FildeshSxpb(sxpb, it, ensure_name_FildeshSxpb(sxpb, "", 0), FildeshSxprotoFieldKind_MESSAGE);
  make_node_330(sxpb, it);
  it = direct_insert_next_FildeshSxpb(sxpb, it, ensure_name_FildeshSxpb(sxpb, "", 0), FildeshSxprotoFieldKind_MESSAGE);
  make_node_340(sxpb, it);
}

static void make_node_310(FildeshSxpb* sxpb, FildeshSxpbIT it) {
  it = direct_insert_first_FildeshSxpb(sxpb, it, ensure_name_FildeshSxpb(sxpb, "div", 3), FildeshSxprotoFieldKind_ARRAY);
  make_node_320(sxpb, it);
}

static void make_node_300(FildeshSxpb* sxpb, FildeshSxpbIT it) {
  it = direct_insert_first_FildeshSxpb(sxpb, it, ensure_name_FildeshSxpb(sxpb, "neg", 3), FildeshSxprotoFieldKind_MESSAGE);
  make_node_310(sxpb, it);
}

static void make_node_210(FildeshSxpb* sxpb, FildeshSxpbIT it) {
  it = direct_insert_first_FildeshSxpb(sxpb, it, ensure_name_FildeshSxpb(sxpb, "", 0), FildeshSxprotoFieldKind_MESSAGE);
  make_node_220(sxpb, it);
  it = direct_insert_next_FildeshSxpb(sxpb, it, ensure_name_FildeshSxpb(sxpb, "", 0), FildeshSxprotoFieldKind_MESSAGE);
  make_node_260(sxpb, it);
  it = direct_insert_next_FildeshSxpb(sxpb, it, ensure_name_FildeshSxpb(sxpb, "", 0), FildeshSxprotoFieldKind_MESSAGE);
  make_node_300(sxpb, it);
}

static void make_node_100(FildeshSxpb* sxpb, FildeshSxpbIT it) {
  it = direct_insert_first_FildeshSxpb(sxpb, it, ensure_name_FildeshSxpb(sxpb, "add", 3), FildeshSxprotoFieldKind_MANYOF);
  make_node_110(sxpb, it);
  it = direct_insert_next_FildeshSxpb(sxpb, it, ensure_name_FildeshSxpb(sxpb, "add", 3), FildeshSxprotoFieldKind_MANYOF);
  make_node_160(sxpb, it);
  it = direct_insert_next_FildeshSxpb(sxpb, it, ensure_name_FildeshSxpb(sxpb, "add", 3), FildeshSxprotoFieldKind_ARRAY);
  make_node_210(sxpb, it);
}


FildeshSxpb* make_manyof_test_FildeshSxpb(void) {
  FildeshSxpb* sxpb = open_FildeshSxpb();
  FildeshSxpbIT it = top_of_FildeshSxpb(sxpb);
  (*sxpb->values)[0].field_kind = FildeshSxprotoFieldKind_MANYOF;
  it = direct_insert_first_FildeshSxpb(sxpb, it, ensure_name_FildeshSxpb(sxpb, "identical_expressions", 21), FildeshSxprotoFieldKind_MANYOF);
  make_node_100(sxpb, it);
  it = direct_insert_next_FildeshSxpb(sxpb, it, ensure_name_FildeshSxpb(sxpb, "empty_manyof", 12), FildeshSxprotoFieldKind_MANYOF);
  it = direct_insert_next_FildeshSxpb(sxpb, it, ensure_name_FildeshSxpb(sxpb, "another_empty_manyof", 20), FildeshSxprotoFieldKind_ARRAY);
  it = direct_insert_next_FildeshSxpb(sxpb, it, ensure_name_FildeshSxpb(sxpb, "empty_message", 13), FildeshSxprotoFieldKind_MESSAGE);
  return sxpb;
}
