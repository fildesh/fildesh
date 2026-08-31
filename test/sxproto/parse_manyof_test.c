#include <assert.h>
#include <string.h>

#include <fildesh/sxproto.h>

#include "src/sxproto/parse_sxpb.h"

static void parse_manyof_anonymous_discriminated_string_test() {

  FildeshX content_in = FildeshX_of_strlit(
      "((my_manyof) (\"\" 1 string) 2 (red \"\" 21) (blue \"4\" 2))");
  FildeshO* err_out = open_FildeshOF("/dev/stderr");
  FildeshSxpb* sxpb = slurp_sxpb_close_FildeshX(&content_in, NULL, err_out);
  FildeshSxpbIT it;

  assert(sxpb);
  it = lookup_subfield_at_FildeshSxpb(
      sxpb, top_of_FildeshSxpb(sxpb), "my_manyof");
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
  close_FildeshO(err_out);
}

static void parse_manyof_basic_append_test() {
  FildeshX content_in = FildeshX_of_strlit(
      "(my_manyof (()) (x 1))\n(my_manyof (()) (x 2))");
  FildeshO* err_out = open_FildeshOF("/dev/stderr");
  FildeshSxpb* sxpb = slurp_sxpb_close_FildeshX(&content_in, NULL, err_out);
  FildeshSxpbIT it;

  assert(sxpb);
  it = lookup_subfield_at_FildeshSxpb(
      sxpb, top_of_FildeshSxpb(sxpb), "my_manyof");
  assert(!nullish_FildeshSxpbIT(it));
  assert(it.field_kind == FildeshSxprotoFieldKind_MANYOF);

  it = first_at_FildeshSxpb(sxpb, it);
  assert(!nullish_FildeshSxpbIT(it));
  assert(it.field_kind == FildeshSxprotoFieldKind_LITERAL);
  assert(name_at_FildeshSxpb(sxpb, it));
  assert(0 == strcmp("x", name_at_FildeshSxpb(sxpb, it)));
  assert(1 == unsigned_value_at_FildeshSxpb(sxpb, it));

  it = next_at_FildeshSxpb(sxpb, it);
  assert(!nullish_FildeshSxpbIT(it));
  assert(it.field_kind == FildeshSxprotoFieldKind_LITERAL);
  assert(name_at_FildeshSxpb(sxpb, it));
  assert(0 == strcmp("x", name_at_FildeshSxpb(sxpb, it)));
  assert(2 == unsigned_value_at_FildeshSxpb(sxpb, it));

  it = next_at_FildeshSxpb(sxpb, it);
  assert(nullish_FildeshSxpbIT(it));

  close_FildeshSxpb(sxpb);
  close_FildeshO(err_out);
}

int main() {
  parse_manyof_anonymous_discriminated_string_test();
  parse_manyof_basic_append_test();
  return 0;
}
