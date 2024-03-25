#include <assert.h>
#include <string.h>

#include <fildesh/sxproto.h>

static
  void
assign_to_subfield_test()
{
  static FildeshSxprotoField thing_loneof[] = {
    {"f", FILL_FildeshSxprotoField_FLOAT(0, 10)},
    {"s", FILL_FildeshSxprotoField_STRING(1, 10)},
  };
  static FildeshSxprotoField m_message[] = {
    {"a", FILL_DEFAULT_FildeshSxprotoField_INTS},
    {"b", FILL_DEFAULT_FildeshSxprotoField_BOOL},
    {"i", FILL_FildeshSxprotoField_INT(0, 10)},
    {"thing", FILL_FildeshSxprotoField_LONEOF(thing_loneof)},
    {"things", FILL_FildeshSxprotoField_MANYOF(thing_loneof)},
  };
  static FildeshSxprotoField toplevel_fields[] = {
    {"m", 1  FILL_FildeshSxprotoField_MESSAGE(m_message)},
  };
  DECLARE_TOPLEVEL_FildeshSxprotoField(toplevel_schema, toplevel_fields);
  FildeshX in[1];
  FildeshO* err_out = open_FildeshOF("/dev/stderr");
  FildeshSxpb* sxpb = open_FildeshSxpb();
  FildeshSxpb* src_sxpb;
  FildeshSxpbIT src_it;
  FildeshSxpbIT dst_it;
  FildeshSxpbIT it;
  const FildeshSxprotoField* message_schema;

  lone_toplevel_initialization_FildeshSxprotoField(toplevel_schema);
  message_schema = subfield_of_FildeshSxprotoField(toplevel_schema, "m");
  assert(1 == tag_id_of_FildeshSxprotoField(message_schema));

  it = ensure_message_subfield_at_FildeshSxpb(
      sxpb, top_of_FildeshSxpb(sxpb), "dst_message");

  /* dst_message.dst_b := false */
  *in = FildeshX_of_strlit("(b false)");
  src_sxpb = slurp_sxpb_close_FildeshX(in, message_schema, err_out);
  src_it = lookup_subfield_at_FildeshSxpb(
      src_sxpb, top_of_FildeshSxpb(src_sxpb), "b");
  assign_at_FildeshSxpb(sxpb, it, "dst_b", src_sxpb, src_it);
  close_FildeshSxpb(src_sxpb);
  dst_it = lookup_subfield_at_FildeshSxpb(sxpb, it, "dst_b");
  assert(!bool_value_at_FildeshSxpb(sxpb, dst_it));

  /* dst_message.dst_b := true */
  *in = FildeshX_of_strlit("(b true)");
  src_sxpb = slurp_sxpb_close_FildeshX(in, message_schema, err_out);
  src_it = lookup_subfield_at_FildeshSxpb(
      src_sxpb, top_of_FildeshSxpb(src_sxpb), "b");
  assign_at_FildeshSxpb(sxpb, dst_it, NULL, src_sxpb, src_it);
  close_FildeshSxpb(src_sxpb);
  assert(bool_value_at_FildeshSxpb(sxpb, dst_it));

  /* dst_message.dst_i := 5 */
  *in = FildeshX_of_strlit("(i 5)");
  src_sxpb = slurp_sxpb_close_FildeshX(in, message_schema, err_out);
  src_it = lookup_subfield_at_FildeshSxpb(
      src_sxpb, top_of_FildeshSxpb(src_sxpb), "i");
  assign_at_FildeshSxpb(sxpb, it, "dst_i", src_sxpb, src_it);
  close_FildeshSxpb(src_sxpb);
  assert(5 == unsigned_value_at_FildeshSxpb(
          sxpb, lookup_subfield_at_FildeshSxpb(sxpb, it, "dst_i")));

  /* dst_message := (() (b true) (i 2)) */
  *in = FildeshX_of_strlit("(m (b true) (i 2))");
  src_sxpb = slurp_sxpb_close_FildeshX(in, toplevel_schema, err_out);
  src_it = lookup_subfield_at_FildeshSxpb(
      src_sxpb, top_of_FildeshSxpb(src_sxpb), "m");
  assign_at_FildeshSxpb(sxpb, it, NULL, src_sxpb, src_it);
  close_FildeshSxpb(src_sxpb);
  assert(bool_value_at_FildeshSxpb(
          sxpb, lookup_subfield_at_FildeshSxpb(sxpb, it, "b")));
  assert(2 == unsigned_value_at_FildeshSxpb(
          sxpb, lookup_subfield_at_FildeshSxpb(sxpb, it, "i")));

  /* dst_message.dst_a := ((() thing) (f 7.7)) */
  *in = FildeshX_of_strlit("((thing f) 7.7)");
  src_sxpb = slurp_sxpb_close_FildeshX(in, message_schema, err_out);
  src_it = lookup_subfield_at_FildeshSxpb(
      src_sxpb, top_of_FildeshSxpb(src_sxpb), "thing");
  assign_at_FildeshSxpb(sxpb, it, "dst_thing", src_sxpb, src_it);
  close_FildeshSxpb(src_sxpb);
  dst_it = lookup_subfield_at_FildeshSxpb(sxpb, it, "dst_thing");
  dst_it = first_at_FildeshSxpb(sxpb, dst_it);
  assert(!nullish_FildeshSxpbIT(dst_it));

  /* dst_message.dst_a := ((()) 1 2 3) */
  *in = FildeshX_of_strlit("((a) 1 2 3)");
  src_sxpb = slurp_sxpb_close_FildeshX(in, message_schema, err_out);
  src_it = lookup_subfield_at_FildeshSxpb(
      src_sxpb, top_of_FildeshSxpb(src_sxpb), "a");
  assign_at_FildeshSxpb(sxpb, it, "dst_a", src_sxpb, src_it);
  close_FildeshSxpb(src_sxpb);
  dst_it = lookup_subfield_at_FildeshSxpb(sxpb, it, "dst_a");
  dst_it = first_at_FildeshSxpb(sxpb, dst_it);
  assert(!nullish_FildeshSxpbIT(dst_it));

  /* dst_message.dst_a := ((()) (f 1.0) (s "2") (f 3.0)) */
  *in = FildeshX_of_strlit("((things) (f 1.0) (s \"2\") (f 3.0))");
  src_sxpb = slurp_sxpb_close_FildeshX(in, message_schema, err_out);
  src_it = lookup_subfield_at_FildeshSxpb(
      src_sxpb, top_of_FildeshSxpb(src_sxpb), "things");
  assign_at_FildeshSxpb(sxpb, it, "dst_things", src_sxpb, src_it);
  close_FildeshSxpb(src_sxpb);
  dst_it = lookup_subfield_at_FildeshSxpb(sxpb, it, "dst_things");
  dst_it = first_at_FildeshSxpb(sxpb, dst_it);
  assert(!nullish_FildeshSxpbIT(dst_it));

  close_FildeshSxpb(sxpb);
  close_FildeshO(err_out);
}

int main() {
  assign_to_subfield_test();
  return 0;
}
