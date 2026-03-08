#include <fildesh/sxproto.h>
#include "src/sxproto/value.h"
#include <string.h>

static void build_node_1(FildeshSxpb* sxpb, FildeshSxpbIT parent_it);
static void build_node_2(FildeshSxpb* sxpb, FildeshSxpbIT parent_it);
static void build_node_4(FildeshSxpb* sxpb, FildeshSxpbIT parent_it);
static void build_node_6(FildeshSxpb* sxpb, FildeshSxpbIT parent_it);
static void build_node_8(FildeshSxpb* sxpb, FildeshSxpbIT parent_it);
static void build_node_10(FildeshSxpb* sxpb, FildeshSxpbIT parent_it);
static void build_node_12(FildeshSxpb* sxpb, FildeshSxpbIT parent_it);
static void build_node_14(FildeshSxpb* sxpb, FildeshSxpbIT parent_it);
static void build_node_18(FildeshSxpb* sxpb, FildeshSxpbIT parent_it);
static void build_node_25(FildeshSxpb* sxpb, FildeshSxpbIT parent_it);
static void build_node_27(FildeshSxpb* sxpb, FildeshSxpbIT parent_it);

static void build_node_2(FildeshSxpb* sxpb, FildeshSxpbIT parent_it) {
  FildeshSxpbIT it = parent_it;
  it.cons_id = parent_it.elem_id;
  it.elem_id = ~(FildeshSxpb_id)0;
  it = direct_insert_first_FildeshSxpb(sxpb, it, ensure_name_FildeshSxpb(sxpb, "AA BB CC", 8), FildeshSxprotoFieldKind_LITERAL_STRING);
}

static void build_node_4(FildeshSxpb* sxpb, FildeshSxpbIT parent_it) {
  FildeshSxpbIT it = parent_it;
  it.cons_id = parent_it.elem_id;
  it.elem_id = ~(FildeshSxpb_id)0;
  it = direct_insert_first_FildeshSxpb(sxpb, it, ensure_name_FildeshSxpb(sxpb, "AA BB CC", 8), FildeshSxprotoFieldKind_LITERAL_STRING);
}

static void build_node_6(FildeshSxpb* sxpb, FildeshSxpbIT parent_it) {
  FildeshSxpbIT it = parent_it;
  it.cons_id = parent_it.elem_id;
  it.elem_id = ~(FildeshSxpb_id)0;
  it = direct_insert_first_FildeshSxpb(sxpb, it, ensure_name_FildeshSxpb(sxpb, "1 2 3", 5), FildeshSxprotoFieldKind_LITERAL_STRING);
}

static void build_node_8(FildeshSxpb* sxpb, FildeshSxpbIT parent_it) {
  FildeshSxpbIT it = parent_it;
  it.cons_id = parent_it.elem_id;
  it.elem_id = ~(FildeshSxpb_id)0;
  it = direct_insert_first_FildeshSxpb(sxpb, it, ensure_name_FildeshSxpb(sxpb, "1 2 3", 5), FildeshSxprotoFieldKind_LITERAL_STRING);
}

static void build_node_10(FildeshSxpb* sxpb, FildeshSxpbIT parent_it) {
  FildeshSxpbIT it = parent_it;
  it.cons_id = parent_it.elem_id;
  it.elem_id = ~(FildeshSxpb_id)0;
  it = direct_insert_first_FildeshSxpb(sxpb, it, ensure_name_FildeshSxpb(sxpb, "AA BB CC", 8), FildeshSxprotoFieldKind_LITERAL_STRING);
}

static void build_node_12(FildeshSxpb* sxpb, FildeshSxpbIT parent_it) {
  FildeshSxpbIT it = parent_it;
  it.cons_id = parent_it.elem_id;
  it.elem_id = ~(FildeshSxpb_id)0;
  it = direct_insert_first_FildeshSxpb(sxpb, it, ensure_name_FildeshSxpb(sxpb, "Unquoted string 2.", 18), FildeshSxprotoFieldKind_LITERAL_STRING);
}

static void build_node_14(FildeshSxpb* sxpb, FildeshSxpbIT parent_it) {
  FildeshSxpbIT it = parent_it;
  it.cons_id = parent_it.elem_id;
  it.elem_id = ~(FildeshSxpb_id)0;
  it = direct_insert_first_FildeshSxpb(sxpb, it, ensure_name_FildeshSxpb(sxpb, "unquoted", 8), FildeshSxprotoFieldKind_LITERAL_STRING);
  it = direct_insert_next_FildeshSxpb(sxpb, it, ensure_name_FildeshSxpb(sxpb, "string", 6), FildeshSxprotoFieldKind_LITERAL_STRING);
  it = direct_insert_next_FildeshSxpb(sxpb, it, ensure_name_FildeshSxpb(sxpb, "array", 5), FildeshSxprotoFieldKind_LITERAL_STRING);
}

static void build_node_18(FildeshSxpb* sxpb, FildeshSxpbIT parent_it) {
  FildeshSxpbIT it = parent_it;
  it.cons_id = parent_it.elem_id;
  it.elem_id = ~(FildeshSxpb_id)0;
  it = direct_insert_first_FildeshSxpb(sxpb, it, ensure_name_FildeshSxpb(sxpb, "1", 1), FildeshSxprotoFieldKind_LITERAL_STRING);
  it = direct_insert_next_FildeshSxpb(sxpb, it, ensure_name_FildeshSxpb(sxpb, "2", 1), FildeshSxprotoFieldKind_LITERAL_STRING);
  it = direct_insert_next_FildeshSxpb(sxpb, it, ensure_name_FildeshSxpb(sxpb, "3", 1), FildeshSxprotoFieldKind_LITERAL_STRING);
  it = direct_insert_next_FildeshSxpb(sxpb, it, ensure_name_FildeshSxpb(sxpb, "4", 1), FildeshSxprotoFieldKind_LITERAL_STRING);
  it = direct_insert_next_FildeshSxpb(sxpb, it, ensure_name_FildeshSxpb(sxpb, "5 6", 3), FildeshSxprotoFieldKind_LITERAL_STRING);
  it = direct_insert_next_FildeshSxpb(sxpb, it, ensure_name_FildeshSxpb(sxpb, "7 8", 3), FildeshSxprotoFieldKind_LITERAL_STRING);
}

static void build_node_25(FildeshSxpb* sxpb, FildeshSxpbIT parent_it) {
  FildeshSxpbIT it = parent_it;
  it.cons_id = parent_it.elem_id;
  it.elem_id = ~(FildeshSxpb_id)0;
  it = direct_insert_first_FildeshSxpb(sxpb, it, ensure_name_FildeshSxpb(sxpb, "1\n\"2\"\n3\n4", 9), FildeshSxprotoFieldKind_LITERAL_STRING);
}

static void build_node_27(FildeshSxpb* sxpb, FildeshSxpbIT parent_it) {
  FildeshSxpbIT it = parent_it;
  it.cons_id = parent_it.elem_id;
  it.elem_id = ~(FildeshSxpb_id)0;
  it = direct_insert_first_FildeshSxpb(sxpb, it, ensure_name_FildeshSxpb(sxpb, "", 0), FildeshSxprotoFieldKind_LITERAL_STRING);
}

static void build_node_1(FildeshSxpb* sxpb, FildeshSxpbIT parent_it) {
  FildeshSxpbIT it = parent_it;
  it.cons_id = parent_it.elem_id;
  it.elem_id = ~(FildeshSxpb_id)0;
  it = direct_insert_first_FildeshSxpb(sxpb, it, ensure_name_FildeshSxpb(sxpb, "quoted_1", 8), FildeshSxprotoFieldKind_LITERAL);
  build_node_2(sxpb, it);
  it = direct_insert_next_FildeshSxpb(sxpb, it, ensure_name_FildeshSxpb(sxpb, "quoted_2", 8), FildeshSxprotoFieldKind_LITERAL);
  build_node_4(sxpb, it);
  it = direct_insert_next_FildeshSxpb(sxpb, it, ensure_name_FildeshSxpb(sxpb, "quotemix_1", 10), FildeshSxprotoFieldKind_LITERAL);
  build_node_6(sxpb, it);
  it = direct_insert_next_FildeshSxpb(sxpb, it, ensure_name_FildeshSxpb(sxpb, "quotemix_2", 10), FildeshSxprotoFieldKind_LITERAL);
  build_node_8(sxpb, it);
  it = direct_insert_next_FildeshSxpb(sxpb, it, ensure_name_FildeshSxpb(sxpb, "unquoted_1", 10), FildeshSxprotoFieldKind_LITERAL);
  build_node_10(sxpb, it);
  it = direct_insert_next_FildeshSxpb(sxpb, it, ensure_name_FildeshSxpb(sxpb, "unquoted_2", 10), FildeshSxprotoFieldKind_LITERAL);
  build_node_12(sxpb, it);
  it = direct_insert_next_FildeshSxpb(sxpb, it, ensure_name_FildeshSxpb(sxpb, "array_mix_1", 11), FildeshSxprotoFieldKind_ARRAY);
  build_node_14(sxpb, it);
  it = direct_insert_next_FildeshSxpb(sxpb, it, ensure_name_FildeshSxpb(sxpb, "array_mix_2", 11), FildeshSxprotoFieldKind_ARRAY);
  build_node_18(sxpb, it);
  it = direct_insert_next_FildeshSxpb(sxpb, it, ensure_name_FildeshSxpb(sxpb, "multiline_1", 11), FildeshSxprotoFieldKind_LITERAL);
  build_node_25(sxpb, it);
  it = direct_insert_next_FildeshSxpb(sxpb, it, ensure_name_FildeshSxpb(sxpb, "empty_string", 12), FildeshSxprotoFieldKind_LITERAL);
  build_node_27(sxpb, it);
}

FildeshSxpb* make_string_test_FildeshSxpb(void) {
  FildeshSxpb* sxpb = open_FildeshSxpb();
  FildeshSxpbIT top = top_of_FildeshSxpb(sxpb);
  top.elem_id = 0;
  build_node_1(sxpb, top);
  return sxpb;
}
