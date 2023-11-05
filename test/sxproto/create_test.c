#include <fildesh/sxproto.h>

#include <assert.h>
#include <string.h>

static void create_test() {
  FildeshSxpb* sxpb = open_FildeshSxpb();
  FildeshSxpbIT m_it, it;

  m_it = ensure_message_subfield_at_FildeshSxpb(
      sxpb, top_of_FildeshSxpb(sxpb), "m");
  assert(!nullish_FildeshSxpbIT(m_it));

  it = assign_bool_subfield_at_FildeshSxpb(
      sxpb, m_it, "b", true);
  assert(bool_value_at_FildeshSxpb(sxpb, it));
  it = assign_bool_subfield_at_FildeshSxpb(
      sxpb, m_it, "b", false);
  assert(!bool_value_at_FildeshSxpb(sxpb, it));

  it = assign_str_subfield_at_FildeshSxpb(
      sxpb, m_it, "s", "my string");
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
  create_test();
  return 0;
}
