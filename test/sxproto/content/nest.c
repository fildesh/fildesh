#include <fildesh/sxproto.h>

#include "src/sxproto/value.h"

static void add_node_110(FildeshSxpb* sxpb, FildeshSxpbIT it) {
  it = direct_insert_first_FildeshSxpb(sxpb, it, ensure_name_FildeshSxpb(sxpb, "cinnamon", 8), FildeshSxprotoFieldKind_LITERAL_STRING);
}

static void add_node_100(FildeshSxpb* sxpb, FildeshSxpbIT it) {
  it = direct_insert_first_FildeshSxpb(sxpb, it, ensure_name_FildeshSxpb(sxpb, "color", 5), FildeshSxprotoFieldKind_NEST);
  add_node_110(sxpb, it);
  it = direct_insert_next_FildeshSxpb(sxpb, it, ensure_name_FildeshSxpb(sxpb, "clawing at the bark of a Douglas fir", 36), FildeshSxprotoFieldKind_LITERAL_STRING);
}

static void add_node_200(FildeshSxpb* sxpb, FildeshSxpbIT it) {
  it = direct_insert_first_FildeshSxpb(sxpb, it, ensure_name_FildeshSxpb(sxpb, "foraging for honeyberries", 25), FildeshSxprotoFieldKind_LITERAL_STRING);
}

static void add_node_310(FildeshSxpb* sxpb, FildeshSxpbIT it) {
  it = direct_insert_first_FildeshSxpb(sxpb, it, ensure_name_FildeshSxpb(sxpb, "gently in the breeze", 20), FildeshSxprotoFieldKind_LITERAL_STRING);
}

static void add_node_320(FildeshSxpb* sxpb, FildeshSxpbIT it) {
  it = direct_insert_first_FildeshSxpb(sxpb, it, ensure_name_FildeshSxpb(sxpb, "green", 5), FildeshSxprotoFieldKind_LITERAL_STRING);
  it = direct_insert_next_FildeshSxpb(sxpb, it, ensure_name_FildeshSxpb(sxpb, "verdant", 7), FildeshSxprotoFieldKind_LITERAL_STRING);
  it = direct_insert_next_FildeshSxpb(sxpb, it, ensure_name_FildeshSxpb(sxpb, "swaying", 7), FildeshSxprotoFieldKind_NEST);
  add_node_310(sxpb, it);
}

static void add_node_330(FildeshSxpb* sxpb, FildeshSxpbIT it) {
  it = direct_insert_first_FildeshSxpb(sxpb, it, ensure_name_FildeshSxpb(sxpb, "sweet", 5), FildeshSxprotoFieldKind_LITERAL_STRING);
  it = direct_insert_next_FildeshSxpb(sxpb, it, ensure_name_FildeshSxpb(sxpb, "subtle", 6), FildeshSxprotoFieldKind_LITERAL_STRING);
}

static void add_node_340(FildeshSxpb* sxpb, FildeshSxpbIT it) {
  it = direct_insert_first_FildeshSxpb(sxpb, it, ensure_name_FildeshSxpb(sxpb, "blooming", 8), FildeshSxprotoFieldKind_LITERAL_STRING);
  it = direct_insert_next_FildeshSxpb(sxpb, it, ensure_name_FildeshSxpb(sxpb, "fragrance", 9), FildeshSxprotoFieldKind_NEST);
  add_node_330(sxpb, it);
}

static void add_node_300(FildeshSxpb* sxpb, FildeshSxpbIT it) {
  it = direct_insert_first_FildeshSxpb(sxpb, it, ensure_name_FildeshSxpb(sxpb, "grass", 5), FildeshSxprotoFieldKind_NEST);
  add_node_320(sxpb, it);
  it = direct_insert_next_FildeshSxpb(sxpb, it, ensure_name_FildeshSxpb(sxpb, "wildflowers", 11), FildeshSxprotoFieldKind_NEST);
  add_node_340(sxpb, it);
}

static void add_node_410(FildeshSxpb* sxpb, FildeshSxpbIT it) {
  it = direct_insert_first_FildeshSxpb(sxpb, it, ensure_name_FildeshSxpb(sxpb, "content", 7), FildeshSxprotoFieldKind_LITERAL_STRING);
}

static void add_node_420(FildeshSxpb* sxpb, FildeshSxpbIT it) {
  it = direct_insert_first_FildeshSxpb(sxpb, it, ensure_name_FildeshSxpb(sxpb, "", 0), FildeshSxprotoFieldKind_LITERAL_STRING);
  it = direct_insert_next_FildeshSxpb(sxpb, it, ensure_name_FildeshSxpb(sxpb, "with", 4), FildeshSxprotoFieldKind_LITERAL_STRING);
  it = direct_insert_next_FildeshSxpb(sxpb, it, ensure_name_FildeshSxpb(sxpb, "empty", 5), FildeshSxprotoFieldKind_LITERAL_STRING);
  it = direct_insert_next_FildeshSxpb(sxpb, it, ensure_name_FildeshSxpb(sxpb, "string", 6), FildeshSxprotoFieldKind_LITERAL_STRING);
}

static void add_node_430(FildeshSxpb* sxpb, FildeshSxpbIT it) {
  it = direct_insert_first_FildeshSxpb(sxpb, it, ensure_name_FildeshSxpb(sxpb, "", 0), FildeshSxprotoFieldKind_NEST);
  it = direct_insert_next_FildeshSxpb(sxpb, it, ensure_name_FildeshSxpb(sxpb, "with", 4), FildeshSxprotoFieldKind_LITERAL_STRING);
  it = direct_insert_next_FildeshSxpb(sxpb, it, ensure_name_FildeshSxpb(sxpb, "empty", 5), FildeshSxprotoFieldKind_LITERAL_STRING);
  it = direct_insert_next_FildeshSxpb(sxpb, it, ensure_name_FildeshSxpb(sxpb, "anonymous", 9), FildeshSxprotoFieldKind_LITERAL_STRING);
  it = direct_insert_next_FildeshSxpb(sxpb, it, ensure_name_FildeshSxpb(sxpb, "nest", 4), FildeshSxprotoFieldKind_LITERAL_STRING);
}

static void add_node_400(FildeshSxpb* sxpb, FildeshSxpbIT it) {
  it = direct_insert_first_FildeshSxpb(sxpb, it, ensure_name_FildeshSxpb(sxpb, "empty_nest", 10), FildeshSxprotoFieldKind_NEST);
  it = direct_insert_next_FildeshSxpb(sxpb, it, ensure_name_FildeshSxpb(sxpb, "empty_nest", 10), FildeshSxprotoFieldKind_NEST);
  it = direct_insert_next_FildeshSxpb(sxpb, it, ensure_name_FildeshSxpb(sxpb, "", 0), FildeshSxprotoFieldKind_NEST);
  it = direct_insert_next_FildeshSxpb(sxpb, it, ensure_name_FildeshSxpb(sxpb, "normal string", 13), FildeshSxprotoFieldKind_LITERAL_STRING);
  it = direct_insert_next_FildeshSxpb(sxpb, it, ensure_name_FildeshSxpb(sxpb, "an empty string follows this one", 32), FildeshSxprotoFieldKind_LITERAL_STRING);
  it = direct_insert_next_FildeshSxpb(sxpb, it, ensure_name_FildeshSxpb(sxpb, "", 0), FildeshSxprotoFieldKind_LITERAL_STRING);
  it = direct_insert_next_FildeshSxpb(sxpb, it, ensure_name_FildeshSxpb(sxpb, "an anonymous nest follows this string", 37), FildeshSxprotoFieldKind_LITERAL_STRING);
  it = direct_insert_next_FildeshSxpb(sxpb, it, ensure_name_FildeshSxpb(sxpb, "", 0), FildeshSxprotoFieldKind_NEST);
  add_node_410(sxpb, it);
  it = direct_insert_next_FildeshSxpb(sxpb, it, ensure_name_FildeshSxpb(sxpb, "an empty anonymous nest follows this string", 43), FildeshSxprotoFieldKind_LITERAL_STRING);
  it = direct_insert_next_FildeshSxpb(sxpb, it, ensure_name_FildeshSxpb(sxpb, "", 0), FildeshSxprotoFieldKind_NEST);

  it = direct_insert_next_FildeshSxpb(sxpb, it, ensure_name_FildeshSxpb(sxpb, "nest", 4), FildeshSxprotoFieldKind_NEST);
  add_node_420(sxpb, it);

  it = direct_insert_next_FildeshSxpb(sxpb, it, ensure_name_FildeshSxpb(sxpb, "nest", 4), FildeshSxprotoFieldKind_NEST);
  add_node_430(sxpb, it);
}

static void add_node_000(FildeshSxpb* sxpb, FildeshSxpbIT it) {
  it = direct_insert_first_FildeshSxpb(sxpb, it, ensure_name_FildeshSxpb(sxpb, "black bear", 10), FildeshSxprotoFieldKind_NEST);
  add_node_100(sxpb, it);
  it = direct_insert_next_FildeshSxpb(sxpb, it, ensure_name_FildeshSxpb(sxpb, "grizzly bear", 12), FildeshSxprotoFieldKind_NEST);
  add_node_200(sxpb, it);
  it = direct_insert_next_FildeshSxpb(sxpb, it, ensure_name_FildeshSxpb(sxpb, "meadow", 6), FildeshSxprotoFieldKind_NEST);
  add_node_300(sxpb, it);
  it = direct_insert_next_FildeshSxpb(sxpb, it, ensure_name_FildeshSxpb(sxpb, "", 0), FildeshSxprotoFieldKind_NEST);
  add_node_400(sxpb, it);
}

FildeshSxpb* make_nest_test_FildeshSxpb(void) {
  FildeshSxpb* sxpb = open_FildeshSxpb();
  FildeshSxpbIT top = top_of_FildeshSxpb(sxpb);
  (*sxpb->values)[top.cons_id].field_kind = FildeshSxprotoFieldKind_NEST;
  add_node_000(sxpb, top);
  return sxpb;
}
