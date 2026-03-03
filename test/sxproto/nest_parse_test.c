#include "src/sxproto/parse_sxpb.h"
#include <fildesh/sxproto.h>
#include <assert.h>
#include <string.h>

static void expect_nest_with_string_test() {
  FildeshSxpbInfo info[1] = {DEFAULT_FildeshSxpbInfo};
  FildeshO oslice[1] = {DEFAULT_FildeshO};
  FildeshSxpb* sxpb = open_FildeshSxpb();
  FildeshX slice = FildeshX_of_strlit("(nest_with_string (\"\") the_string)");
  FildeshSxpbIT p_it = top_of_FildeshSxpb(sxpb);
  FildeshSxpbIT it;
  bool good;
  (*sxpb->values)[top_of_FildeshSxpb(sxpb).cons_id].field_kind = FildeshSxprotoFieldKind_NEST;
  p_it.field_kind = FildeshSxprotoFieldKind_NEST;
  good = parse_field_FildeshSxpbInfo(info, NULL, &slice, sxpb, p_it, oslice);
  assert(good);

  it = first_at_FildeshSxpb(sxpb, p_it);
  assert(!nullish_FildeshSxpbIT(it));
  assert(it.field_kind == FildeshSxprotoFieldKind_NEST);
  assert(0 == strcmp(name_at_FildeshSxpb(sxpb, it), "nest_with_string"));

  it = first_at_FildeshSxpb(sxpb, it);
  assert(!nullish_FildeshSxpbIT(it));
  assert(it.field_kind == FildeshSxprotoFieldKind_LITERAL_STRING);
  assert(0 == strcmp(str_value_at_FildeshSxpb(sxpb, it), "the_string"));

  assert(nullish_FildeshSxpbIT(next_at_FildeshSxpb(sxpb, it)));

  close_FildeshO(oslice);
  close_FildeshSxpb(sxpb);
}

static void expect_empty_nest_test() {
  FildeshSxpbInfo info[1] = {DEFAULT_FildeshSxpbInfo};
  FildeshO oslice[1] = {DEFAULT_FildeshO};
  FildeshSxpb* sxpb = open_FildeshSxpb();
  FildeshX slice = FildeshX_of_strlit("(empty_nest (\"\"))");
  FildeshSxpbIT p_it = top_of_FildeshSxpb(sxpb);
  FildeshSxpbIT it;
  bool good;
  (*sxpb->values)[top_of_FildeshSxpb(sxpb).cons_id].field_kind = FildeshSxprotoFieldKind_NEST;
  p_it.field_kind = FildeshSxprotoFieldKind_NEST;
  good = parse_field_FildeshSxpbInfo(info, NULL, &slice, sxpb, p_it, oslice);
  assert(good);

  it = first_at_FildeshSxpb(sxpb, p_it);
  assert(!nullish_FildeshSxpbIT(it));
  assert(it.field_kind == FildeshSxprotoFieldKind_NEST);
  assert(0 == strcmp(name_at_FildeshSxpb(sxpb, it), "empty_nest"));

  /* empty_nest has NO children because `("")` creates an empty nest type. */
  assert(nullish_FildeshSxpbIT(first_at_FildeshSxpb(sxpb, it)));

  close_FildeshO(oslice);
  close_FildeshSxpb(sxpb);
}

static void expect_nest_with_3_empty_strings_test() {
  FildeshSxpbInfo info[1] = {DEFAULT_FildeshSxpbInfo};
  FildeshO oslice[1] = {DEFAULT_FildeshO};
  FildeshSxpb* sxpb = open_FildeshSxpb();
  FildeshX slice = FildeshX_of_strlit("(nest_with_3_empty_strings (\"\") \"\" \"\" \"\")");
  FildeshSxpbIT p_it = top_of_FildeshSxpb(sxpb);
  FildeshSxpbIT it;
  bool good;
  (*sxpb->values)[top_of_FildeshSxpb(sxpb).cons_id].field_kind = FildeshSxprotoFieldKind_NEST;
  p_it.field_kind = FildeshSxprotoFieldKind_NEST;
  good = parse_field_FildeshSxpbInfo(info, NULL, &slice, sxpb, p_it, oslice);
  assert(good);

  it = first_at_FildeshSxpb(sxpb, p_it);
  assert(!nullish_FildeshSxpbIT(it));
  assert(it.field_kind == FildeshSxprotoFieldKind_NEST);
  assert(0 == strcmp(name_at_FildeshSxpb(sxpb, it), "nest_with_3_empty_strings"));

  it = first_at_FildeshSxpb(sxpb, it);
  assert(!nullish_FildeshSxpbIT(it));
  assert(it.field_kind == FildeshSxprotoFieldKind_LITERAL_STRING);
  assert(0 == strcmp(str_value_at_FildeshSxpb(sxpb, it), ""));

  assert(nullish_FildeshSxpbIT(next_at_FildeshSxpb(sxpb, it)));

  close_FildeshO(oslice);
  close_FildeshSxpb(sxpb);
}

static void expect_nest_with_anonymous_discriminated_string_test() {
  FildeshSxpbInfo info[1] = {DEFAULT_FildeshSxpbInfo};
  FildeshO oslice[1] = {DEFAULT_FildeshO};
  FildeshSxpb* sxpb = open_FildeshSxpb();
  FildeshX slice = FildeshX_of_strlit("(nest_with_anonymous_discriminated_string (\"\") (\"\" the_string))");
  FildeshSxpbIT p_it = top_of_FildeshSxpb(sxpb);
  FildeshSxpbIT it;
  bool good;
  (*sxpb->values)[top_of_FildeshSxpb(sxpb).cons_id].field_kind = FildeshSxprotoFieldKind_NEST;
  p_it.field_kind = FildeshSxprotoFieldKind_NEST;
  good = parse_field_FildeshSxpbInfo(info, NULL, &slice, sxpb, p_it, oslice);
  assert(good);

  it = first_at_FildeshSxpb(sxpb, p_it);
  assert(!nullish_FildeshSxpbIT(it));
  assert(it.field_kind == FildeshSxprotoFieldKind_NEST);
  assert(0 == strcmp(name_at_FildeshSxpb(sxpb, it), "nest_with_anonymous_discriminated_string"));

  it = first_at_FildeshSxpb(sxpb, it);
  assert(!nullish_FildeshSxpbIT(it));
  assert(it.field_kind == FildeshSxprotoFieldKind_LITERAL_STRING);
  assert(name_at_FildeshSxpb(sxpb, it) == NULL || 0 == strcmp(name_at_FildeshSxpb(sxpb, it), ""));
  assert(0 == strcmp(str_value_at_FildeshSxpb(sxpb, it), "the_string"));

  assert(nullish_FildeshSxpbIT(next_at_FildeshSxpb(sxpb, it)));

  close_FildeshO(oslice);
  close_FildeshSxpb(sxpb);
}

static void expect_nest_with_subnests_test() {
  FildeshSxpbInfo info[1] = {DEFAULT_FildeshSxpbInfo};
  FildeshO oslice[1] = {DEFAULT_FildeshO};
  FildeshSxpb* sxpb = open_FildeshSxpb();
  FildeshX slice = FildeshX_of_strlit("(nest_with_subnests (\"\") (subnest1 k1 k2 k3) (subnest2 k4 k5 k6))");
  FildeshSxpbIT p_it = top_of_FildeshSxpb(sxpb);
  FildeshSxpbIT it;
  FildeshSxpbIT sub_it;
  bool good;
  (*sxpb->values)[top_of_FildeshSxpb(sxpb).cons_id].field_kind = FildeshSxprotoFieldKind_NEST;
  p_it.field_kind = FildeshSxprotoFieldKind_NEST;
  good = parse_field_FildeshSxpbInfo(info, NULL, &slice, sxpb, p_it, oslice);
  assert(good);

  it = first_at_FildeshSxpb(sxpb, p_it);
  assert(!nullish_FildeshSxpbIT(it));
  assert(it.field_kind == FildeshSxprotoFieldKind_NEST);
  assert(0 == strcmp(name_at_FildeshSxpb(sxpb, it), "nest_with_subnests"));

  /* subnest1 */
  it = first_at_FildeshSxpb(sxpb, it);
  assert(!nullish_FildeshSxpbIT(it));
  assert(it.field_kind == FildeshSxprotoFieldKind_NEST);
  assert(0 == strcmp(name_at_FildeshSxpb(sxpb, it), "subnest1"));

  sub_it = first_at_FildeshSxpb(sxpb, it);
  assert(!nullish_FildeshSxpbIT(sub_it));
  assert(sub_it.field_kind == FildeshSxprotoFieldKind_LITERAL_STRING);
  assert(0 == strcmp(str_value_at_FildeshSxpb(sxpb, sub_it), "k1"));
  sub_it = next_at_FildeshSxpb(sxpb, sub_it);
  assert(0 == strcmp(str_value_at_FildeshSxpb(sxpb, sub_it), "k2"));
  sub_it = next_at_FildeshSxpb(sxpb, sub_it);
  assert(0 == strcmp(str_value_at_FildeshSxpb(sxpb, sub_it), "k3"));
  assert(nullish_FildeshSxpbIT(next_at_FildeshSxpb(sxpb, sub_it)));

  /* subnest2 */
  it = next_at_FildeshSxpb(sxpb, it);
  assert(!nullish_FildeshSxpbIT(it));
  assert(it.field_kind == FildeshSxprotoFieldKind_NEST);
  assert(0 == strcmp(name_at_FildeshSxpb(sxpb, it), "subnest2"));

  sub_it = first_at_FildeshSxpb(sxpb, it);
  assert(!nullish_FildeshSxpbIT(sub_it));
  assert(sub_it.field_kind == FildeshSxprotoFieldKind_LITERAL_STRING);
  assert(0 == strcmp(str_value_at_FildeshSxpb(sxpb, sub_it), "k4"));
  sub_it = next_at_FildeshSxpb(sxpb, sub_it);
  assert(0 == strcmp(str_value_at_FildeshSxpb(sxpb, sub_it), "k5"));
  sub_it = next_at_FildeshSxpb(sxpb, sub_it);
  assert(0 == strcmp(str_value_at_FildeshSxpb(sxpb, sub_it), "k6"));
  assert(nullish_FildeshSxpbIT(next_at_FildeshSxpb(sxpb, sub_it)));

  assert(nullish_FildeshSxpbIT(next_at_FildeshSxpb(sxpb, it)));

  close_FildeshO(oslice);
  close_FildeshSxpb(sxpb);
}

static void expect_nest_with_anonymous_subnests_test() {
  FildeshSxpbInfo info[1] = {DEFAULT_FildeshSxpbInfo};
  FildeshO oslice[1] = {DEFAULT_FildeshO};
  FildeshSxpb* sxpb = open_FildeshSxpb();
  FildeshX slice = FildeshX_of_strlit("(nest_with_anonymous_subnests (\"\") (\"\" (\"\") k1 k2 k3) (\"\" (\"\") k4 k5 k6))");
  FildeshSxpbIT p_it = top_of_FildeshSxpb(sxpb);
  FildeshSxpbIT it;
  FildeshSxpbIT sub_it;
  bool good;
  (*sxpb->values)[top_of_FildeshSxpb(sxpb).cons_id].field_kind = FildeshSxprotoFieldKind_NEST;
  p_it.field_kind = FildeshSxprotoFieldKind_NEST;
  good = parse_field_FildeshSxpbInfo(info, NULL, &slice, sxpb, p_it, oslice);
  assert(good);

  it = first_at_FildeshSxpb(sxpb, p_it);
  assert(!nullish_FildeshSxpbIT(it));
  assert(it.field_kind == FildeshSxprotoFieldKind_NEST);
  assert(0 == strcmp(name_at_FildeshSxpb(sxpb, it), "nest_with_anonymous_subnests"));

  /* anon subnest 1 */
  it = first_at_FildeshSxpb(sxpb, it);
  assert(!nullish_FildeshSxpbIT(it));
  assert(it.field_kind == FildeshSxprotoFieldKind_NEST);
  assert(name_at_FildeshSxpb(sxpb, it) == NULL || 0 == strcmp(name_at_FildeshSxpb(sxpb, it), ""));

  sub_it = first_at_FildeshSxpb(sxpb, it);
  assert(!nullish_FildeshSxpbIT(sub_it));
  assert(sub_it.field_kind == FildeshSxprotoFieldKind_LITERAL_STRING);
  assert(0 == strcmp(str_value_at_FildeshSxpb(sxpb, sub_it), "k1"));
  sub_it = next_at_FildeshSxpb(sxpb, sub_it);
  assert(sub_it.field_kind == FildeshSxprotoFieldKind_LITERAL_STRING);
  assert(0 == strcmp(str_value_at_FildeshSxpb(sxpb, sub_it), "k2"));
  sub_it = next_at_FildeshSxpb(sxpb, sub_it);
  assert(sub_it.field_kind == FildeshSxprotoFieldKind_LITERAL_STRING);
  assert(0 == strcmp(str_value_at_FildeshSxpb(sxpb, sub_it), "k3"));
  assert(nullish_FildeshSxpbIT(next_at_FildeshSxpb(sxpb, sub_it)));

  /* anon subnest 2 */
  it = next_at_FildeshSxpb(sxpb, it);
  assert(!nullish_FildeshSxpbIT(it));
  assert(it.field_kind == FildeshSxprotoFieldKind_NEST);
  assert(name_at_FildeshSxpb(sxpb, it) == NULL || 0 == strcmp(name_at_FildeshSxpb(sxpb, it), ""));

  sub_it = first_at_FildeshSxpb(sxpb, it);
  assert(!nullish_FildeshSxpbIT(sub_it));
  assert(sub_it.field_kind == FildeshSxprotoFieldKind_LITERAL_STRING);
  assert(0 == strcmp(str_value_at_FildeshSxpb(sxpb, sub_it), "k4"));
  sub_it = next_at_FildeshSxpb(sxpb, sub_it);
  assert(sub_it.field_kind == FildeshSxprotoFieldKind_LITERAL_STRING);
  assert(0 == strcmp(str_value_at_FildeshSxpb(sxpb, sub_it), "k5"));
  sub_it = next_at_FildeshSxpb(sxpb, sub_it);
  assert(sub_it.field_kind == FildeshSxprotoFieldKind_LITERAL_STRING);
  assert(0 == strcmp(str_value_at_FildeshSxpb(sxpb, sub_it), "k6"));
  assert(nullish_FildeshSxpbIT(next_at_FildeshSxpb(sxpb, sub_it)));

  assert(nullish_FildeshSxpbIT(next_at_FildeshSxpb(sxpb, it)));

  close_FildeshO(oslice);
  close_FildeshSxpb(sxpb);
}

static void expect_nest_with_empty_subnest_test() {
  FildeshSxpbInfo info[1] = {DEFAULT_FildeshSxpbInfo};
  FildeshO oslice[1] = {DEFAULT_FildeshO};
  FildeshSxpb* sxpb = open_FildeshSxpb();
  FildeshX slice = FildeshX_of_strlit("(nest_with_empty_subnest (\"\") (empty_subnest (\"\")))");
  FildeshSxpbIT p_it = top_of_FildeshSxpb(sxpb);
  FildeshSxpbIT it;
  bool good;
  (*sxpb->values)[top_of_FildeshSxpb(sxpb).cons_id].field_kind = FildeshSxprotoFieldKind_NEST;
  p_it.field_kind = FildeshSxprotoFieldKind_NEST;
  good = parse_field_FildeshSxpbInfo(info, NULL, &slice, sxpb, p_it, oslice);
  assert(good);

  it = first_at_FildeshSxpb(sxpb, p_it);
  assert(!nullish_FildeshSxpbIT(it));
  assert(it.field_kind == FildeshSxprotoFieldKind_NEST);
  assert(0 == strcmp(name_at_FildeshSxpb(sxpb, it), "nest_with_empty_subnest"));

  it = first_at_FildeshSxpb(sxpb, it);
  assert(!nullish_FildeshSxpbIT(it));
  assert(it.field_kind == FildeshSxprotoFieldKind_NEST);
  assert(0 == strcmp(name_at_FildeshSxpb(sxpb, it), "empty_subnest"));
  assert(nullish_FildeshSxpbIT(first_at_FildeshSxpb(sxpb, it)));

  assert(nullish_FildeshSxpbIT(next_at_FildeshSxpb(sxpb, it)));

  close_FildeshO(oslice);
  close_FildeshSxpb(sxpb);
}

static void expect_nest_with_anonymous_empty_subnest_test() {
  FildeshSxpbInfo info[1] = {DEFAULT_FildeshSxpbInfo};
  FildeshO oslice[1] = {DEFAULT_FildeshO};
  FildeshSxpb* sxpb = open_FildeshSxpb();
  FildeshX slice = FildeshX_of_strlit("(nest_with_anonymous_empty_subnest (\"\") (\"\" (\"\")))");
  FildeshSxpbIT p_it = top_of_FildeshSxpb(sxpb);
  FildeshSxpbIT it;
  bool good;
  (*sxpb->values)[top_of_FildeshSxpb(sxpb).cons_id].field_kind = FildeshSxprotoFieldKind_NEST;
  p_it.field_kind = FildeshSxprotoFieldKind_NEST;
  good = parse_field_FildeshSxpbInfo(info, NULL, &slice, sxpb, p_it, oslice);
  assert(good);

  it = first_at_FildeshSxpb(sxpb, p_it);
  assert(!nullish_FildeshSxpbIT(it));
  assert(it.field_kind == FildeshSxprotoFieldKind_NEST);
  assert(0 == strcmp(name_at_FildeshSxpb(sxpb, it), "nest_with_anonymous_empty_subnest"));

  it = first_at_FildeshSxpb(sxpb, it);
  assert(!nullish_FildeshSxpbIT(it));
  assert(it.field_kind == FildeshSxprotoFieldKind_NEST);
  assert(name_at_FildeshSxpb(sxpb, it) == NULL || 0 == strcmp(name_at_FildeshSxpb(sxpb, it), ""));

  assert(nullish_FildeshSxpbIT(first_at_FildeshSxpb(sxpb, it)));

  assert(nullish_FildeshSxpbIT(next_at_FildeshSxpb(sxpb, it)));

  close_FildeshO(oslice);
  close_FildeshSxpb(sxpb);
}


int main() {
  expect_empty_nest_test();
  expect_nest_with_string_test();
  expect_nest_with_3_empty_strings_test();
  expect_nest_with_anonymous_discriminated_string_test();
  expect_nest_with_subnests_test();
  expect_nest_with_anonymous_subnests_test();
  expect_nest_with_empty_subnest_test();
  expect_nest_with_anonymous_empty_subnest_test();
  return 0;
}
