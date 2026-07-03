#include <assert.h>
#include <string.h>

#include <fildesh/sxproto.h>

#include "src/sxproto/parse_sxpb.h"

static void parse_basic_nest_test() {
  FildeshSxpbInfo info[1] = {DEFAULT_FildeshSxpbInfo};
  FildeshO oslice[1] = {DEFAULT_FildeshO};
  FildeshSxpb* sxpb = open_FildeshSxpb();
  const FildeshSxpbIT p_it = top_of_FildeshSxpb(sxpb);
  FildeshSxpbIT it;
  FildeshSxpbIT e_it;
  FildeshX slice;
  info->err_out = open_FildeshOF("/dev/stderr");

  slice = FildeshX_of_strlit("(my_nest (\"\") (k1 v1_1 v1_2) (k2 v2_1 (k3 v3_1 v3_2) v2_3))");

  assert(parse_field_FildeshSxpbInfo(info, NULL,  &slice, sxpb, p_it, oslice));
  it = lookup_subfield_at_FildeshSxpb(sxpb, p_it, "my_nest");
  assert(!nullish_FildeshSxpbIT(it));
  assert(it.field_kind == FildeshSxprotoFieldKind_NEST);

  it = first_at_FildeshSxpb(sxpb, it);
  assert(!nullish_FildeshSxpbIT(it));
  assert(name_at_FildeshSxpb(sxpb, it));
  assert(0 == strcmp("k1", name_at_FildeshSxpb(sxpb, it)));
  assert(it.field_kind == FildeshSxprotoFieldKind_NEST);

  e_it = first_at_FildeshSxpb(sxpb, it);
  assert(!nullish_FildeshSxpbIT(e_it));
  assert(str_value_at_FildeshSxpb(sxpb, e_it));
  assert(0 == strcmp("v1_1", str_value_at_FildeshSxpb(sxpb, e_it)));
  assert(e_it.field_kind == FildeshSxprotoFieldKind_LITERAL_STRING);

  e_it = next_at_FildeshSxpb(sxpb, e_it);
  assert(!nullish_FildeshSxpbIT(e_it));
  assert(str_value_at_FildeshSxpb(sxpb, e_it));
  assert(0 == strcmp("v1_2", str_value_at_FildeshSxpb(sxpb, e_it)));
  assert(e_it.field_kind == FildeshSxprotoFieldKind_LITERAL_STRING);

  it = next_at_FildeshSxpb(sxpb, it);
  assert(!nullish_FildeshSxpbIT(it));
  assert(name_at_FildeshSxpb(sxpb, it));
  assert(0 == strcmp("k2", name_at_FildeshSxpb(sxpb, it)));
  assert(it.field_kind == FildeshSxprotoFieldKind_NEST);

  e_it = first_at_FildeshSxpb(sxpb, it);
  assert(!nullish_FildeshSxpbIT(e_it));
  assert(str_value_at_FildeshSxpb(sxpb, e_it));
  assert(0 == strcmp("v2_1", str_value_at_FildeshSxpb(sxpb, e_it)));
  assert(e_it.field_kind == FildeshSxprotoFieldKind_LITERAL_STRING);

  e_it = next_at_FildeshSxpb(sxpb, e_it);
  it = e_it;

  e_it = next_at_FildeshSxpb(sxpb, e_it);
  assert(!nullish_FildeshSxpbIT(e_it));
  assert(str_value_at_FildeshSxpb(sxpb, e_it));
  assert(0 == strcmp("v2_3", str_value_at_FildeshSxpb(sxpb, e_it)));
  assert(e_it.field_kind == FildeshSxprotoFieldKind_LITERAL_STRING);

  assert(name_at_FildeshSxpb(sxpb, it));
  assert(0 == strcmp("k3", name_at_FildeshSxpb(sxpb, it)));
  assert(it.field_kind == FildeshSxprotoFieldKind_NEST);

  e_it = first_at_FildeshSxpb(sxpb, it);
  assert(!nullish_FildeshSxpbIT(e_it));
  assert(str_value_at_FildeshSxpb(sxpb, e_it));
  assert(0 == strcmp("v3_1", str_value_at_FildeshSxpb(sxpb, e_it)));
  assert(e_it.field_kind == FildeshSxprotoFieldKind_LITERAL_STRING);

  e_it = next_at_FildeshSxpb(sxpb, e_it);
  assert(!nullish_FildeshSxpbIT(e_it));
  assert(str_value_at_FildeshSxpb(sxpb, e_it));
  assert(0 == strcmp("v3_2", str_value_at_FildeshSxpb(sxpb, e_it)));
  assert(e_it.field_kind == FildeshSxprotoFieldKind_LITERAL_STRING);

  close_FildeshO(oslice);
  close_FildeshO(info->err_out);
  close_FildeshSxpb(sxpb);
}

static void parse_anonymous_discriminated_nest_test() {
  FildeshSxpbInfo info[1] = {DEFAULT_FildeshSxpbInfo};
  FildeshO oslice[1] = {DEFAULT_FildeshO};
  FildeshSxpb* sxpb = open_FildeshSxpb();
  const FildeshSxpbIT p_it = top_of_FildeshSxpb(sxpb);
  FildeshSxpbIT it;
  FildeshX slice;
  info->err_out = open_FildeshOF("/dev/stderr");

  slice = FildeshX_of_strlit("(my_nest (\"\") (\"\" (\"\") a b))");

  assert(parse_field_FildeshSxpbInfo(info, NULL,  &slice, sxpb, p_it, oslice));
  it = lookup_subfield_at_FildeshSxpb(sxpb, p_it, "my_nest");
  assert(!nullish_FildeshSxpbIT(it));
  assert(it.field_kind == FildeshSxprotoFieldKind_NEST);

  it = first_at_FildeshSxpb(sxpb, it);
  assert(!nullish_FildeshSxpbIT(it));
  assert(name_at_FildeshSxpb(sxpb, it));
  assert(0 == strcmp("", name_at_FildeshSxpb(sxpb, it)));
  assert(it.field_kind == FildeshSxprotoFieldKind_NEST);

  close_FildeshO(oslice);
  close_FildeshO(info->err_out);
  close_FildeshSxpb(sxpb);
}

static void parse_last_in_nest_field_test() {
  FildeshSxpbInfo info[1] = {DEFAULT_FildeshSxpbInfo};
  FildeshO oslice[1] = {DEFAULT_FildeshO};
  FildeshSxpb* sxpb = open_FildeshSxpb();
  const FildeshSxpbIT p_it = top_of_FildeshSxpb(sxpb);
  info->err_out = open_FildeshOF("/dev/stderr");

#define expectparse(expect, text) do { \
  FildeshX slice = FildeshX_of_strlit("(my_nest (\"\") " text ")"); \
  const char* result = NULL; \
  FildeshSxpbIT it; \
  assert(parse_field_FildeshSxpbInfo(info, NULL,  &slice, sxpb, p_it, oslice)); \
  it = lookup_subfield_at_FildeshSxpb(sxpb, p_it, "my_nest"); \
  assert(!nullish_FildeshSxpbIT(it)); \
  it = first_at_FildeshSxpb(sxpb, it); \
  while (!nullish_FildeshSxpbIT(next_at_FildeshSxpb(sxpb, it))) { \
    it = next_at_FildeshSxpb(sxpb, it); \
  } \
  result = str_value_at_FildeshSxpb(sxpb, it); \
  fildesh_log_trace(result); \
  assert(strlen(expect) == strlen(result)); \
  assert(0 == memcmp(expect, result, strlen(expect))); \
  remove_at_FildeshSxpb(sxpb, first_at_FildeshSxpb(sxpb, p_it)); \
  assert(nullish_FildeshSxpbIT(first_at_FildeshSxpb(sxpb, p_it))); \
  info->column_count = 0; \
} while (0)

  expectparse("hello", "hello");
  expectparse("hello", "hi \"hello\"");
  expectparse("z", "(x y) z");
  expectparse("z", "\"\" (x y) z");

#undef expectparse
  close_FildeshO(oslice);
  close_FildeshO(info->err_out);
  close_FildeshSxpb(sxpb);
}

int main() {
  parse_basic_nest_test();
  parse_anonymous_discriminated_nest_test();
  parse_last_in_nest_field_test();
  return 0;
}
