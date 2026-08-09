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

static void parse_manyof_element_kind_test() {
#define expectmanyoffail(text) do { \
  FildeshX slice = FildeshX_of_strlit(text); \
  FildeshO eo[1] = {DEFAULT_FildeshO}; \
  FildeshSxpb* s = slurp_sxpb_close_FildeshX(&slice, NULL, eo); \
  assert(NULL == s); \
  assert(eo->size > 0); \
  close_FildeshO(eo); \
} while (0)

  /* Unnamed manyof elements obey the same kind constraints as arrays. */
  expectmanyoffail("((m) 1 ())");
  expectmanyoffail("((m) () 1)");
  expectmanyoffail("((m) 1 (() (x 2)))");
  expectmanyoffail("((m) (() (x 1)) 2)");
  expectmanyoffail("((m) one ())");
  expectmanyoffail("((m) () one)");
  expectmanyoffail("((m) (()))");
#undef expectmanyoffail

#define expectmanyofparse(text) do { \
  FildeshX slice = FildeshX_of_strlit(text); \
  FildeshO eo[1] = {DEFAULT_FildeshO}; \
  FildeshSxpb* s = slurp_sxpb_close_FildeshX(&slice, NULL, eo); \
  assert(s); \
  assert(eo->size == 0); \
  close_FildeshSxpb(s); \
  close_FildeshO(eo); \
} while (0)

  /* Named elements are transparent to the unnamed element kind. */
  expectmanyofparse("((m) 1 (named (x 2)) 3)");
  expectmanyofparse("((m) (named (x 2)) 1 3)");
  expectmanyofparse("((m) () (named 1) (() (x 2)))");
#undef expectmanyofparse
}

int main() {
  parse_manyof_anonymous_discriminated_string_test();
  parse_manyof_basic_append_test();
  parse_manyof_element_kind_test();
  return 0;
}
