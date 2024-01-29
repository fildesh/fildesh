#include <fildesh/sxproto.h>

#include <assert.h>
#include <string.h>

static void create_in_message_test() {
  FildeshSxpb* sxpb = open_FildeshSxpb();
  FildeshSxpbIT m_it, it;

  m_it = ensure_message_subfield_at_FildeshSxpb(
      sxpb, top_of_FildeshSxpb(sxpb), "m");
  assert(!nullish_FildeshSxpbIT(m_it));

  it = ensure_array_subfield_at_FildeshSxpb(sxpb, m_it, "some_array");
  assert(!nullish_FildeshSxpbIT(it));
  it = lookup_subfield_at_FildeshSxpb(sxpb, m_it, "some_array");
  assert(!nullish_FildeshSxpbIT(it));
  assert(nullish_FildeshSxpbIT(first_at_FildeshSxpb(sxpb, it)));

  it = ensure_manyof_subfield_at_FildeshSxpb(sxpb, m_it, "some_manyof");
  assert(!nullish_FildeshSxpbIT(it));
  it = lookup_subfield_at_FildeshSxpb(sxpb, m_it, "some_manyof");
  assert(!nullish_FildeshSxpbIT(it));
  assert(nullish_FildeshSxpbIT(first_at_FildeshSxpb(sxpb, it)));

  it = ensure_bool_subfield_at_FildeshSxpb(sxpb, m_it, "b");
  assert(!bool_value_at_FildeshSxpb(sxpb, it));
  it = assign_bool_subfield_at_FildeshSxpb(sxpb, m_it, "b", true);
  assert(bool_value_at_FildeshSxpb(sxpb, it));
  it = assign_bool_subfield_at_FildeshSxpb(sxpb, m_it, "b", false);
  assert(!bool_value_at_FildeshSxpb(sxpb, it));
  it = assign_bool_subfield_at_FildeshSxpb(sxpb, m_it, "b", true);
  assert(bool_value_at_FildeshSxpb(sxpb, it));
  it = ensure_bool_subfield_at_FildeshSxpb(sxpb, m_it, "b");
  assert(bool_value_at_FildeshSxpb(sxpb, it));

  it = ensure_int_subfield_at_FildeshSxpb(sxpb, m_it, "i");
  assert(0 == unsigned_value_at_FildeshSxpb(sxpb, it));

  it = ensure_float_subfield_at_FildeshSxpb(sxpb, m_it, "f");
  assert(0.0f == float_value_at_FildeshSxpb(sxpb, it));

  it = ensure_string_subfield_at_FildeshSxpb(sxpb, m_it, "s");
  assert(0 == strlen(str_value_at_FildeshSxpb(sxpb, it)));
  it = assign_str_subfield_at_FildeshSxpb(sxpb, m_it, "s", "my string");
  assert(0 == strcmp("my string", str_value_at_FildeshSxpb(sxpb, it)));
  it = ensure_string_subfield_at_FildeshSxpb(sxpb, m_it, "s");
  assert(0 == strcmp("my string", str_value_at_FildeshSxpb(sxpb, it)));

  it = assign_str_subfield_at_FildeshSxpb(
      sxpb, m_it, "t", "my second string");
  assert(0 == strcmp("my second string", str_value_at_FildeshSxpb(sxpb, it)));

  remove_at_FildeshSxpb(sxpb, it);
  it = lookup_subfield_at_FildeshSxpb(sxpb, m_it, "t");
  assert(nullish_FildeshSxpbIT(it));

  close_FildeshSxpb(sxpb);
}

int main() {
  create_in_message_test();
  return 0;
}
