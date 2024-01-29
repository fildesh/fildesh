#include <assert.h>
#include <string.h>

#include <fildesh/sxproto.h>

static
  void
append_to_array_test()
{
  static FildeshSxprotoField toplevel_fields[] = {
    {"a", FILL_DEFAULT_FildeshSxprotoField_INTS},
    {"u", FILL_FildeshSxprotoField_INT(0, 100)},
  };
  DECLARE_TOPLEVEL_FildeshSxprotoField(toplevel_schema, toplevel_fields);
  FildeshX in[1];
  FildeshO* err_out = open_FildeshOF("/dev/stderr");
  FildeshSxpb* sxpb = open_FildeshSxpb();
  FildeshSxpb* src_sxpb;
  FildeshSxpbIT src_it;
  FildeshSxpbIT dst_it;
  FildeshSxpbIT it;

  lone_toplevel_initialization_FildeshSxprotoField(toplevel_schema);
  it = ensure_message_subfield_at_FildeshSxpb(
      sxpb, top_of_FildeshSxpb(sxpb), "dst_message");

  /* dst_message.dst_a := ((()) 1) */
  *in = FildeshX_of_strlit("(u 1)");
  src_sxpb = slurp_sxpb_close_FildeshX(in, toplevel_schema, err_out);
  src_it = lookup_subfield_at_FildeshSxpb(
      src_sxpb, top_of_FildeshSxpb(src_sxpb), "u");
  append_at_FildeshSxpb(sxpb, it, "dst_a", src_sxpb, src_it);
  close_FildeshSxpb(src_sxpb);
  dst_it = lookup_subfield_at_FildeshSxpb(sxpb, it, "dst_a");
  dst_it = first_at_FildeshSxpb(sxpb, dst_it);
  assert(1 == unsigned_value_at_FildeshSxpb(sxpb, dst_it));

  /* dst_message.dst_a := ((()) 1 2) */
  *in = FildeshX_of_strlit("(u 2)");
  src_sxpb = slurp_sxpb_close_FildeshX(in, toplevel_schema, err_out);
  src_it = lookup_subfield_at_FildeshSxpb(
      src_sxpb, top_of_FildeshSxpb(src_sxpb), "u");
  append_at_FildeshSxpb(sxpb, it, "dst_a", src_sxpb, src_it);
  close_FildeshSxpb(src_sxpb);
  dst_it = next_at_FildeshSxpb(sxpb, dst_it);
  assert(2 == unsigned_value_at_FildeshSxpb(sxpb, dst_it));

  close_FildeshSxpb(sxpb);
  close_FildeshO(err_out);
}

static
  void
append_to_manyof_from_array_test()
{
  static FildeshSxprotoField toplevel_fields[] = {
    {"a", FILL_DEFAULT_FildeshSxprotoField_INTS},
    {"u", FILL_FildeshSxprotoField_INT(0, 10)},
    {"s", FILL_FildeshSxprotoField_STRING(1, 64)},
  };
  DECLARE_TOPLEVEL_FildeshSxprotoField(toplevel_schema, toplevel_fields);
  FildeshX in[1];
  FildeshO* err_out = open_FildeshOF("/dev/stderr");
  FildeshSxpb* sxpb = open_FildeshSxpb();
  FildeshSxpb* src_sxpb;
  FildeshSxpbIT src_it;
  FildeshSxpbIT dst_it;
  FildeshSxpbIT it;

  lone_toplevel_initialization_FildeshSxprotoField(toplevel_schema);
  it = top_of_FildeshSxpb(sxpb);
  it = ensure_message_subfield_at_FildeshSxpb(sxpb, it, "dst_message");
  it = ensure_manyof_subfield_at_FildeshSxpb(sxpb, it, "dst_manyof");

  /* dst_message.dst_manyof := (((())) (dst_u 1)) */
  *in = FildeshX_of_strlit("((a) 1 2 3)");
  src_sxpb = slurp_sxpb_close_FildeshX(in, toplevel_schema, err_out);
  src_it = top_of_FildeshSxpb(src_sxpb);
  src_it = lookup_subfield_at_FildeshSxpb(src_sxpb, src_it, "a");

  src_it = first_at_FildeshSxpb(src_sxpb, src_it);
  append_at_FildeshSxpb(sxpb, it, "dst_u", src_sxpb, src_it);
  dst_it = first_at_FildeshSxpb(sxpb, it);
  assert(1 == unsigned_value_at_FildeshSxpb(sxpb, dst_it));

  src_it = next_at_FildeshSxpb(src_sxpb, src_it);
  append_at_FildeshSxpb(sxpb, it, "dst_u", src_sxpb, src_it);
  dst_it = next_at_FildeshSxpb(sxpb, dst_it);
  assert(2 == unsigned_value_at_FildeshSxpb(sxpb, dst_it));

  src_it = next_at_FildeshSxpb(src_sxpb, src_it);
  append_at_FildeshSxpb(sxpb, it, "dst_u", src_sxpb, src_it);
  dst_it = next_at_FildeshSxpb(sxpb, dst_it);
  assert(3 == unsigned_value_at_FildeshSxpb(sxpb, dst_it));

  close_FildeshSxpb(src_sxpb);
  close_FildeshSxpb(sxpb);
  close_FildeshO(err_out);
}

static
  void
append_to_manyof_from_manyof_test()
{
  static FildeshSxprotoField m_message[] = {
    {"x", FILL_FildeshSxprotoField_INT(0, 10)},
    {"y", FILL_FildeshSxprotoField_INT(0, 10)},
  };
  static FildeshSxprotoField things_manyof[] = {
    {"", FILL_FildeshSxprotoField_INT(0, 10)},
    {"b", FILL_DEFAULT_FildeshSxprotoField_BOOL},
    {"u", FILL_FildeshSxprotoField_INT(0, 100)},
    {"f", FILL_FildeshSxprotoField_FLOAT(0, 10)},
    {"s", FILL_FildeshSxprotoField_STRING(1, 64)},
    {"m", FILL_FildeshSxprotoField_MESSAGE(m_message)},
  };
  static FildeshSxprotoField toplevel_fields[] = {
    {"things", FILL_FildeshSxprotoField_MANYOF(things_manyof)},
  };
  DECLARE_TOPLEVEL_FildeshSxprotoField(toplevel_schema, toplevel_fields);
  static const char content[] =
    "(((things))\n\
    1\n\
    0\n\
    (b true)\n\
    (b false)\n\
    (u 10)\n\
    (u 11)\n\
    (f 2.0)\n\
    (f 4.0)\n\
    (s \"hello\")\n\
    (s \"world\")\n\
    (m (x 1) (y 2))\n\
    (m (x 3) (y 4))\n\
    )";
  DECLARE_STRLIT_FildeshX(in, content);
  FildeshO* err_out = open_FildeshOF("/dev/stderr");
  FildeshSxpb* sxpb = open_FildeshSxpb();
  FildeshSxpb* src_sxpb;
  FildeshSxpbIT src_it;
  FildeshSxpbIT it;

  lone_toplevel_initialization_FildeshSxprotoField(toplevel_schema);
  src_sxpb = slurp_sxpb_close_FildeshX(in, toplevel_schema, err_out);
  assert(src_sxpb);
  src_it = lookup_subfield_at_FildeshSxpb(
      src_sxpb, top_of_FildeshSxpb(src_sxpb), "things");
  assert(!nullish_FildeshSxpbIT(src_it));

  it = ensure_manyof_subfield_at_FildeshSxpb(
      sxpb, top_of_FildeshSxpb(sxpb), "dst_manyof");
  for (src_it = first_at_FildeshSxpb(src_sxpb, src_it);
       !nullish_FildeshSxpbIT(src_it);
       src_it = next_at_FildeshSxpb(src_sxpb, src_it))
  {
    FildeshSxpbIT result_it = append_at_FildeshSxpb(
        sxpb, it,
        name_at_FildeshSxpb(src_sxpb, src_it),
        src_sxpb, src_it);
    assert(!nullish_FildeshSxpbIT(result_it));
  }
  close_FildeshSxpb(src_sxpb);

  it = top_of_FildeshSxpb(sxpb);
  it = lookup_subfield_at_FildeshSxpb(sxpb, it, "dst_manyof");
  it = first_at_FildeshSxpb(sxpb, it);
  assert(1 == unsigned_value_at_FildeshSxpb(sxpb, it));
  it = next_at_FildeshSxpb(sxpb, it);
  assert(0 == unsigned_value_at_FildeshSxpb(sxpb, it));
  it = next_at_FildeshSxpb(sxpb, it);
  assert(bool_value_at_FildeshSxpb(sxpb, it));
  it = next_at_FildeshSxpb(sxpb, it);
  assert(!bool_value_at_FildeshSxpb(sxpb, it));
  it = next_at_FildeshSxpb(sxpb, it);
  assert(10 == unsigned_value_at_FildeshSxpb(sxpb, it));
  it = next_at_FildeshSxpb(sxpb, it);
  assert(11 == unsigned_value_at_FildeshSxpb(sxpb, it));
  it = next_at_FildeshSxpb(sxpb, it);
  assert(2.0f == float_value_at_FildeshSxpb(sxpb, it));
  it = next_at_FildeshSxpb(sxpb, it);
  assert(4.0f == float_value_at_FildeshSxpb(sxpb, it));
  it = next_at_FildeshSxpb(sxpb, it);
  assert(0 == strcmp("hello", str_value_at_FildeshSxpb(sxpb, it)));
  it = next_at_FildeshSxpb(sxpb, it);
  assert(0 == strcmp("world", str_value_at_FildeshSxpb(sxpb, it)));
  it = next_at_FildeshSxpb(sxpb, it);
  it = next_at_FildeshSxpb(sxpb, it);
  it = next_at_FildeshSxpb(sxpb, it);
  assert(nullish_FildeshSxpbIT(it));

  close_FildeshSxpb(sxpb);
  close_FildeshO(err_out);
}

int main() {
  append_to_array_test();
  append_to_manyof_from_array_test();
  append_to_manyof_from_manyof_test();
  return 0;
}
