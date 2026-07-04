#include <assert.h>
#include <string.h>

#include <fildesh/sxproto.h>

static const char array_test_content[] = "\
(())\n\
(() (w 5) (x 6))\n\
()\n\
(() (y 7) (z 8))\n\
";

static const char dict_test_content[] = "\
()\n\
(key1 (value val1))\n\
(key2 (subkey subval))\n\
";

static const char nest_test_content[] = "\
(\"\")\n\
\"\"  ; Empty string.\n\
(\"\" \"\")  ; Empty string.\n\
(\"\" (\"\"))  ; Anonymous empty nest.\n\
(nest_nothing)\n\
(nest_empty_string \"\"  ; Unnecessary spacing.\n\
)\n\
(bear grizzly)\n\
(7 8 9)\n\
(\"black bear\"\n\
 (color \"\" cinnamon)\n\
 \"clawing at the bark of a Douglas fir\"\n\
 (\"\" clawing at the bark of a Douglas fir)\n\
)\n\
";

static
  void
array_test()
{
  DECLARE_STRLIT_FildeshX(in, array_test_content);
  FildeshO* err_out = open_FildeshOF("/dev/stderr");
  FildeshSxpb* const sxpb = slurp_sxpb_close_FildeshX(in, NULL, err_out);
  FildeshSxpbIT it;
  FildeshSxpbIT val_it;

  assert(sxpb);

  it = first_at_FildeshSxpb(sxpb, top_of_FildeshSxpb(sxpb));
  val_it = lookup_subfield_at_FildeshSxpb(sxpb, it, "w");
  assert(5 == unsigned_value_at_FildeshSxpb(sxpb, val_it));
  val_it = lookup_subfield_at_FildeshSxpb(sxpb, it, "x");
  assert(6 == unsigned_value_at_FildeshSxpb(sxpb, val_it));

  it = next_at_FildeshSxpb(sxpb, it);
  /* Empty message.*/
  assert(nullish_FildeshSxpbIT(first_at_FildeshSxpb(sxpb, it)));

  it = next_at_FildeshSxpb(sxpb, it);
  val_it = lookup_subfield_at_FildeshSxpb(sxpb, it, "y");
  assert(7 == unsigned_value_at_FildeshSxpb(sxpb, val_it));
  val_it = lookup_subfield_at_FildeshSxpb(sxpb, it, "z");
  assert(8 == unsigned_value_at_FildeshSxpb(sxpb, val_it));

  /* End of array.*/
  assert(nullish_FildeshSxpbIT(next_at_FildeshSxpb(sxpb, it)));

  close_FildeshSxpb(sxpb);
  close_FildeshO(err_out);
}

static
  void
dict_test()
{
  DECLARE_STRLIT_FildeshX(in, dict_test_content);
  FildeshO* err_out = open_FildeshOF("/dev/stderr");
  FildeshSxpb* const sxpb = slurp_sxpb_close_FildeshX(in, NULL, err_out);
  FildeshSxpbIT it;
  FildeshSxpbIT val_it;

  assert(sxpb);

  it = top_of_FildeshSxpb(sxpb);
  assert(it.field_kind == FildeshSxprotoFieldKind_DICT);
  val_it = lookup_subfield_at_FildeshSxpb(sxpb, it, "key1");
  assert(val_it.field_kind == FildeshSxprotoFieldKind_MESSAGE);
  val_it = lookup_subfield_at_FildeshSxpb(sxpb, val_it, "value");
  assert(0 == strcmp("val1", str_value_at_FildeshSxpb(sxpb, val_it)));
  val_it = lookup_subfield_at_FildeshSxpb(sxpb, it, "key2");
  assert(val_it.field_kind == FildeshSxprotoFieldKind_MESSAGE);
  val_it = lookup_subfield_at_FildeshSxpb(sxpb, val_it, "subkey");
  assert(0 == strcmp("subval", str_value_at_FildeshSxpb(sxpb, val_it)));

  close_FildeshSxpb(sxpb);
  close_FildeshO(err_out);
}

static
  void
nest_test()
{
  DECLARE_STRLIT_FildeshX(in, nest_test_content);
  FildeshO* err_out = open_FildeshOF("/dev/stderr");
  FildeshSxpb* const sxpb = slurp_sxpb_close_FildeshX(in, NULL, err_out);
  FildeshSxpbIT it;
  FildeshSxpbIT val_it;

  assert(sxpb);

  it = top_of_FildeshSxpb(sxpb);
  assert(it.field_kind == FildeshSxprotoFieldKind_NEST);

  it = first_at_FildeshSxpb(sxpb, it);
  assert(0 == strcmp("", str_value_at_FildeshSxpb(sxpb, it)));

  it = next_at_FildeshSxpb(sxpb, it);
  assert(0 == strcmp("", str_value_at_FildeshSxpb(sxpb, it)));

  it = next_at_FildeshSxpb(sxpb, it);
  assert(0 == strcmp("", name_at_FildeshSxpb(sxpb, it)));
  assert(it.field_kind == FildeshSxprotoFieldKind_NEST);
  assert(nullish_FildeshSxpbIT(first_at_FildeshSxpb(sxpb, it)));

  it = next_at_FildeshSxpb(sxpb, it);
  assert(0 == strcmp("nest_nothing", name_at_FildeshSxpb(sxpb, it)));
  assert(it.field_kind == FildeshSxprotoFieldKind_NEST);
  assert(nullish_FildeshSxpbIT(first_at_FildeshSxpb(sxpb, it)));

  it = next_at_FildeshSxpb(sxpb, it);
  assert(0 == strcmp("nest_empty_string", name_at_FildeshSxpb(sxpb, it)));
  assert(it.field_kind == FildeshSxprotoFieldKind_NEST);
  val_it = first_at_FildeshSxpb(sxpb, it);
  assert(0 == strcmp("", str_value_at_FildeshSxpb(sxpb, val_it)));
  val_it = next_at_FildeshSxpb(sxpb, val_it);
  assert(nullish_FildeshSxpbIT(val_it));

  it = next_at_FildeshSxpb(sxpb, it);
  assert(0 == strcmp("bear", name_at_FildeshSxpb(sxpb, it)));
  assert(it.field_kind == FildeshSxprotoFieldKind_NEST);
  val_it = first_at_FildeshSxpb(sxpb, it);
  assert(0 == strcmp("grizzly", str_value_at_FildeshSxpb(sxpb, val_it)));

  it = next_at_FildeshSxpb(sxpb, it);
  assert(0 == strcmp("7", name_at_FildeshSxpb(sxpb, it)));
  assert(it.field_kind == FildeshSxprotoFieldKind_NEST);

  val_it = first_at_FildeshSxpb(sxpb, it);
  assert(0 == strcmp("8", str_value_at_FildeshSxpb(sxpb, val_it)));
  val_it = next_at_FildeshSxpb(sxpb, val_it);
  assert(0 == strcmp("9", str_value_at_FildeshSxpb(sxpb, val_it)));

  it = next_at_FildeshSxpb(sxpb, it);
  assert(0 == strcmp("black bear", name_at_FildeshSxpb(sxpb, it)));
  assert(it.field_kind == FildeshSxprotoFieldKind_NEST);

  val_it = first_at_FildeshSxpb(sxpb, it);
  assert(0 == strcmp("color", name_at_FildeshSxpb(sxpb, val_it)));
  assert(val_it.field_kind == FildeshSxprotoFieldKind_NEST);
  {
    FildeshSxpbIT color_val_it = first_at_FildeshSxpb(sxpb, val_it);
    assert(0 == strcmp("cinnamon", str_value_at_FildeshSxpb(sxpb, color_val_it)));
  }

  val_it = next_at_FildeshSxpb(sxpb, val_it);
  assert(!nullish_FildeshSxpbIT(val_it));
  assert(val_it.field_kind == FildeshSxprotoFieldKind_LITERAL_STRING);
  assert(0 == strcmp("clawing at the bark of a Douglas fir",
                     str_value_at_FildeshSxpb(sxpb, val_it)));

  val_it = next_at_FildeshSxpb(sxpb, val_it);
  assert(!nullish_FildeshSxpbIT(val_it));
  assert(val_it.field_kind == FildeshSxprotoFieldKind_LITERAL_STRING);
  assert(0 == strcmp("clawing at the bark of a Douglas fir",
                     str_value_at_FildeshSxpb(sxpb, val_it)));

  /* End of nest.*/
  assert(nullish_FildeshSxpbIT(next_at_FildeshSxpb(sxpb, it)));

  close_FildeshSxpb(sxpb);
  close_FildeshO(err_out);
}

int main() {
  array_test();
  dict_test();
  nest_test();
  return 0;
}
