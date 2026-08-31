#include <assert.h>
#include <string.h>

#include <fildesh/sxproto.h>

#include "src/sxproto/parse_sxpb.h"

static void parse_manyof_anonymous_discriminated_string_test() {
  FildeshSxpbInfo info[1] = {DEFAULT_FildeshSxpbInfo};
  FildeshO oslice[1] = {DEFAULT_FildeshO};
  FildeshSxpb* sxpb = open_FildeshSxpb();
  const FildeshSxpbIT p_it = top_of_FildeshSxpb(sxpb);
  FildeshSxpbIT it;
  FildeshX slice;
  info->err_out = open_FildeshOF("/dev/stderr");

  slice = FildeshX_of_strlit(
      "((my_manyof) (\"\" 1 string) 2 (red \"\" 21) (blue \"4\" 2))");

  assert(parse_field_FildeshSxpbInfo(info, NULL,  &slice, sxpb, p_it, oslice));
  it = lookup_subfield_at_FildeshSxpb(sxpb, p_it, "my_manyof");
  assert(!nullish_FildeshSxpbIT(it));
  assert(it.field_kind == FildeshSxprotoFieldKind_MANYOF);

  it = first_at_FildeshSxpb(sxpb, it);
  assert(!nullish_FildeshSxpbIT(it));
  assert(it.field_kind == FildeshSxprotoFieldKind_LITERAL_STRING);
  assert(!name_at_FildeshSxpb(sxpb, it));
  assert(0 == strcmp("1 string", str_value_at_FildeshSxpb(sxpb, it)));

  it = next_at_FildeshSxpb(sxpb, it);
  assert(!nullish_FildeshSxpbIT(it));
  assert(it.field_kind == FildeshSxprotoFieldKind_LITERAL_STRING);
  assert(!name_at_FildeshSxpb(sxpb, it));
  assert(0 == strcmp("2", str_value_at_FildeshSxpb(sxpb, it)));

  it = next_at_FildeshSxpb(sxpb, it);
  assert(!nullish_FildeshSxpbIT(it));
  assert(it.field_kind == FildeshSxprotoFieldKind_LITERAL);
  assert(name_at_FildeshSxpb(sxpb, it));
  assert(0 == strcmp("red", name_at_FildeshSxpb(sxpb, it)));
  assert(0 == strcmp("21", str_value_at_FildeshSxpb(sxpb, it)));

  it = next_at_FildeshSxpb(sxpb, it);
  assert(!nullish_FildeshSxpbIT(it));
  assert(it.field_kind == FildeshSxprotoFieldKind_LITERAL);
  assert(name_at_FildeshSxpb(sxpb, it));
  assert(0 == strcmp("blue", name_at_FildeshSxpb(sxpb, it)));
  assert(0 == strcmp("42", str_value_at_FildeshSxpb(sxpb, it)));

  it = next_at_FildeshSxpb(sxpb, it);
  assert(nullish_FildeshSxpbIT(it));

  close_FildeshSxpb(sxpb);
  close_FildeshO(oslice);
  close_FildeshO(info->err_out);
}

int main() {
  parse_manyof_anonymous_discriminated_string_test();
  return 0;
}
