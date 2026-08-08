#include <assert.h>
#include <string.h>

#include <fildesh/sxproto.h>

static const char literal_test_content[] = "\
(b +true)\n\
(n 10)\n\
(f 2.5)\n\
(s_alternate_name \"Kappa ¬‿¬\")\n\
";
static const char message_test_content[] = "\
(cons\n\
 (car \"first\")\n\
 (cdr\n\
  (car \"second\")\n\
  (cdr (car \"third\") (cdr))\n\
))\n\
";
static const char loneof_test_content[] = "\
((fruit_as banana) +true)\n\
";
static const char array_test_content[] = "\
(messages (())\n\
 (() (car \"schwam\"))\n\
 ()\n\
 (() (\"car\" \"doo\") (cdr (car \"two and heif\")))\n\
)\n\
(a (()) 0.5e1 4 30e-1 2.e0 1)\n\
";
static const char dict_test_content[] = "\
(string_dict () (alpha \"one\") (beta \"two\"))\n\
(int_dict () (alpha 1) (beta 2))\n\
(float_dict () (alpha 1.5) (beta 2.5))\n\
(bool_dict () (alpha +true) (beta +false))\n\
(message_dict ()\n\
 (first (car \"one\") (cdr))\n\
 (second (car \"two\") (cdr))\n\
)\n\
";
static const char manyof_test_content[] = "\
((predicates)\n\
 \"alpha\"\n\
 \"beta\"\n\
 \"gamma\"\n\
 \"delta\"\n\
 (\"\" epsilon zeta)\n\
 (b +false)\n\
 ((or)\n\
  (b 1)\n\
  (b_alias 0)\n\
  (u 1)\n\
  (u 0)\n\
))\n\
((many_fruits) (() (banana +true)) () (() (apple +false)))\n\
";
static const char manyof_coercion_test_content[] = "\
((b) 1)\n\
((n) 2)\n\
((f) 2.5)\n\
((s_alternate_name) seven)\n\
((cons) (car kappa))\n\
(predicates (()) alpha beta gamma)\n\
((a) 1 2 3)\n\
(many_fruits (banana +true) (apple +true) (banana +false))\n\
";
static const char nest_test_content[] = "\
(nest (\"\") (a b (c d)) e)\n\
";

static
  const FildeshSxprotoField*
sxproto_schema()
{
  static FildeshSxprotoField m_fields[] = {
    {"car", FILL_DEFAULT_FildeshSxprotoField_STRING},
    {"cdr", FILL_RECURSIVE_FildeshSxprotoField_MESSAGE},
  };
  static FildeshSxprotoField predicates_manyof[] = {
    {"", FILL_FildeshSxprotoField_STRING(1, INT_MAX)},
    {"b", FILL_DEFAULT_FildeshSxprotoField_BOOL},
    {"b_alias", FILL_DEFAULT_FildeshSxprotoField_ALIAS},
    {"u", FILL_FildeshSxprotoField_INT(0, 1)},
    {"or", FILL_RECURSIVE_FildeshSxprotoField_MANYOF},
  };
  static FildeshSxprotoField fruit_loneof[] = {
    {"apple", FILL_DEFAULT_FildeshSxprotoField_BOOL},
    {"banana", FILL_DEFAULT_FildeshSxprotoField_BOOL},
  };
  static FildeshSxprotoField toplevel_fields[] = {
    {"b", FILL_DEFAULT_FildeshSxprotoField_BOOL},
    {"n", FILL_FildeshSxprotoField_INT(0, INT_MAX)},
    {"f", FILL_FildeshSxprotoField_FLOAT(0, 10)},
    {"a", FILL_DEFAULT_FildeshSxprotoField_FLOATS},
    {"a_alias", FILL_DEFAULT_FildeshSxprotoField_ALIAS},
    {"bool_dict", FILL_DEFAULT_FildeshSxprotoField_BOOL_DICT},
    {"cons", FILL_FildeshSxprotoField_MESSAGE(m_fields)},
    {"float_dict", FILL_DEFAULT_FildeshSxprotoField_FLOAT_DICT},
    {"int_dict", FILL_DEFAULT_FildeshSxprotoField_INT_DICT},
    {"many_fruits", FILL_FildeshSxprotoField_MANYOF(fruit_loneof)},
    {"message_dict", FILL_FildeshSxprotoField_MESSAGE_DICT(m_fields)},
    {"messages", FILL_FildeshSxprotoField_MESSAGES(m_fields)},
    {"nest", FILL_DEFAULT_FildeshSxprotoField_NEST},
    {"predicates", FILL_FildeshSxprotoField_MANYOF(predicates_manyof)},
    {"s", FILL_FildeshSxprotoField_STRING(1, 64)},
    {"s_alternate_name", FILL_DEFAULT_FildeshSxprotoField_ALIAS},
    {"string_dict", FILL_DEFAULT_FildeshSxprotoField_STRING_DICT},
    {"fruit_as", FILL_FildeshSxprotoField_LONEOF(fruit_loneof)},
  };
  DECLARE_TOPLEVEL_FildeshSxprotoField(schema, toplevel_fields);
  DECLARE_TOPLEVEL_FildeshSxprotoField(schema_dupe, toplevel_fields);

  /* We test something that shares the same data.*/
  if (lone_toplevel_initialization_FildeshSxprotoField(schema_dupe)) {
    return NULL;
  }
  lone_toplevel_initialization_FildeshSxprotoField(schema);
  return schema;
}

static
  void
literal_test()
{
  DECLARE_STRLIT_FildeshX(in, literal_test_content);
  FildeshO* err_out = open_FildeshOF("/dev/stderr");
  const FildeshSxprotoField* const schema = (
      (void)sxproto_schema(),
      sxproto_schema());
  FildeshSxpb* const sxpb = slurp_sxpb_close_FildeshX(in, schema, err_out);
  const FildeshSxpbIT top_it = top_of_FildeshSxpb(sxpb);
  bool tmp_b;
  unsigned tmp_u;
  float tmp_f;
  const char* tmp_s;
  bool good;

  assert(sxpb);
  assert(name_at_FildeshSxpb(sxpb, top_it));
  assert(!name_at_FildeshSxpb(sxpb, top_it)[0]);

  good = lone_subfield_at_FildeshSxpb_to_bool(&tmp_b, sxpb, top_it, "b");
  assert(good);
  assert(tmp_b);

  good = lone_subfield_at_FildeshSxpb_to_unsigned(&tmp_u, sxpb, top_it, "n");
  assert(good);
  assert(tmp_u == 10);

  good = lone_subfield_at_FildeshSxpb_to_float(&tmp_f, sxpb, top_it, "f");
  assert(good);
  assert(tmp_f == 2.5f);

  good = lone_subfield_at_FildeshSxpb_to_str(&tmp_s, sxpb, top_it, "s");
  assert(good);
  assert(0 == strcmp(tmp_s, "Kappa ¬‿¬"));

  close_FildeshSxpb(sxpb);
  close_FildeshO(err_out);
}

static
  void
message_test()
{
  DECLARE_STRLIT_FildeshX(in, message_test_content);
  FildeshO* err_out = open_FildeshOF("/dev/stderr");
  const FildeshSxprotoField* const schema = sxproto_schema();
  FildeshSxpb* const sxpb = slurp_sxpb_close_FildeshX(in, schema, err_out);
  const FildeshSxpbIT top_it = top_of_FildeshSxpb(sxpb);
  FildeshSxpbIT it;
  const char* tmp_s;
  bool good;

  assert(sxpb);

  it = lookup_subfield_at_FildeshSxpb(sxpb, top_it, "cons");
  assert(0 == strcmp(name_at_FildeshSxpb(sxpb, it), "cons"));

  good = lone_subfield_at_FildeshSxpb_to_str(&tmp_s, sxpb, it, "car");
  assert(good);
  assert(0 == strcmp(tmp_s, "first"));

  it = lookup_subfield_at_FildeshSxpb(sxpb, it, "cdr");
  assert(0 == strcmp(name_at_FildeshSxpb(sxpb, it), "cdr"));
  good = lone_subfield_at_FildeshSxpb_to_str(&tmp_s, sxpb, it, "car");
  assert(good);
  assert(0 == strcmp(tmp_s, "second"));

  it = lookup_subfield_at_FildeshSxpb(sxpb, it, "cdr");
  good = lone_subfield_at_FildeshSxpb_to_str(&tmp_s, sxpb, it, "car");
  assert(good);
  assert(0 == strcmp(tmp_s, "third"));

  it = lookup_subfield_at_FildeshSxpb(sxpb, it, "cdr");
  assert(0 == strcmp(name_at_FildeshSxpb(sxpb, it), "cdr"));
  it = lookup_subfield_at_FildeshSxpb(sxpb, it, "cdr");
  assert(nullish_FildeshSxpbIT(it));

  close_FildeshSxpb(sxpb);
  close_FildeshO(err_out);
}

static
  void
loneof_test()
{
  DECLARE_STRLIT_FildeshX(in, loneof_test_content);
  FildeshO* err_out = open_FildeshOF("/dev/stderr");
  const FildeshSxprotoField* const schema = sxproto_schema();
  FildeshSxpb* const sxpb = slurp_sxpb_close_FildeshX(in, schema, err_out);
  const FildeshSxpbIT top_it = top_of_FildeshSxpb(sxpb);
  FildeshSxpbIT it;
  bool tmp_b;

  assert(sxpb);

  it = lookup_subfield_at_FildeshSxpb(sxpb, top_it, "fruit_as");
  assert(0 == strcmp(name_at_FildeshSxpb(sxpb, it), "fruit_as"));
  assert(lone_subfield_at_FildeshSxpb_to_bool(&tmp_b, sxpb, it, "banana"));
  assert(tmp_b);
  it = first_at_FildeshSxpb(sxpb, it);
  assert(!nullish_FildeshSxpbIT(it));
  assert(0 == strcmp(name_at_FildeshSxpb(sxpb, it), "banana"));

  close_FildeshSxpb(sxpb);
  close_FildeshO(err_out);
}

static
  void
array_test()
{
  DECLARE_STRLIT_FildeshX(in, array_test_content);
  FildeshO* err_out = open_FildeshOF("/dev/stderr");
  const FildeshSxprotoField* const schema = sxproto_schema();
  FildeshSxpb* const sxpb = slurp_sxpb_close_FildeshX(in, schema, err_out);
  const FildeshSxpbIT top_it = top_of_FildeshSxpb(sxpb);
  FildeshSxpbIT it;
  FildeshSxpbIT val_it;

  assert(sxpb);

  it = lookup_subfield_at_FildeshSxpb(sxpb, top_it, "a");
  assert(0 == strcmp(name_at_FildeshSxpb(sxpb, it), "a"));
  /* Message fields are stored in input order. This one is last.*/
  assert(it.elem_id != first_at_FildeshSxpb(sxpb, top_it).elem_id);
  /* Check all elements in the array.*/
  it = first_at_FildeshSxpb(sxpb, it);
  assert(!nullish_FildeshSxpbIT(it));
  assert(5.0f == float_value_at_FildeshSxpb(sxpb, it));
  it = next_at_FildeshSxpb(sxpb, it);
  assert(!nullish_FildeshSxpbIT(it));
  assert(4.0f == float_value_at_FildeshSxpb(sxpb, it));
  it = next_at_FildeshSxpb(sxpb, it);
  assert(3.0f == float_value_at_FildeshSxpb(sxpb, it));
  it = next_at_FildeshSxpb(sxpb, it);
  assert(2.0f == float_value_at_FildeshSxpb(sxpb, it));
  it = next_at_FildeshSxpb(sxpb, it);
  assert(1.0f == float_value_at_FildeshSxpb(sxpb, it));
  assert(nullish_FildeshSxpbIT(next_at_FildeshSxpb(sxpb, it)));


  it = lookup_subfield_at_FildeshSxpb(sxpb, top_it, "messages");
  assert(0 == strcmp(name_at_FildeshSxpb(sxpb, it), "messages"));

  it = first_at_FildeshSxpb(sxpb, it);
  val_it = lookup_subfield_at_FildeshSxpb(sxpb, it, "car");
  assert(0 == strcmp("schwam", str_value_at_FildeshSxpb(sxpb, val_it)));

  it = next_at_FildeshSxpb(sxpb, it);
  /* Empty message.*/
  assert(nullish_FildeshSxpbIT(first_at_FildeshSxpb(sxpb, it)));

  it = next_at_FildeshSxpb(sxpb, it);
  val_it = lookup_subfield_at_FildeshSxpb(sxpb, it, "car");
  assert(0 == strcmp("doo", str_value_at_FildeshSxpb(sxpb, val_it)));
  assert(nullish_FildeshSxpbIT(next_at_FildeshSxpb(sxpb, it)));

  it = lookup_subfield_at_FildeshSxpb(sxpb, it, "cdr");
  val_it = lookup_subfield_at_FildeshSxpb(sxpb, it, "car");
  assert(0 == strcmp("two and heif", str_value_at_FildeshSxpb(sxpb, val_it)));

  it = lookup_subfield_at_FildeshSxpb(sxpb, it, "cdr");
  assert(nullish_FildeshSxpbIT(it));

  close_FildeshSxpb(sxpb);
  close_FildeshO(err_out);
}


static
  void
dict_test()
{
  DECLARE_STRLIT_FildeshX(in, dict_test_content);
  FildeshO* err_out = open_FildeshOF("/dev/stderr");
  const FildeshSxprotoField* const schema = sxproto_schema();
  FildeshSxpb* const sxpb = slurp_sxpb_close_FildeshX(in, schema, err_out);
  const FildeshSxpbIT top_it = top_of_FildeshSxpb(sxpb);
  FildeshSxpbIT it;
  FildeshSxpbIT val_it;

  assert(sxpb);

  it = lookup_subfield_at_FildeshSxpb(sxpb, top_it, "string_dict");
  assert(0 == strcmp(name_at_FildeshSxpb(sxpb, it), "string_dict"));
  val_it = lookup_subfield_at_FildeshSxpb(sxpb, it, "alpha");
  assert(0 == strcmp("one", str_value_at_FildeshSxpb(sxpb, val_it)));
  val_it = lookup_subfield_at_FildeshSxpb(sxpb, it, "beta");
  assert(0 == strcmp("two", str_value_at_FildeshSxpb(sxpb, val_it)));

  it = lookup_subfield_at_FildeshSxpb(sxpb, top_it, "int_dict");
  assert(0 == strcmp(name_at_FildeshSxpb(sxpb, it), "int_dict"));
  val_it = lookup_subfield_at_FildeshSxpb(sxpb, it, "alpha");
  assert(1 == unsigned_value_at_FildeshSxpb(sxpb, val_it));
  val_it = lookup_subfield_at_FildeshSxpb(sxpb, it, "beta");
  assert(2 == unsigned_value_at_FildeshSxpb(sxpb, val_it));

  it = lookup_subfield_at_FildeshSxpb(sxpb, top_it, "float_dict");
  assert(0 == strcmp(name_at_FildeshSxpb(sxpb, it), "float_dict"));
  val_it = lookup_subfield_at_FildeshSxpb(sxpb, it, "alpha");
  assert(1.5f == float_value_at_FildeshSxpb(sxpb, val_it));
  val_it = lookup_subfield_at_FildeshSxpb(sxpb, it, "beta");
  assert(2.5f == float_value_at_FildeshSxpb(sxpb, val_it));

  it = lookup_subfield_at_FildeshSxpb(sxpb, top_it, "bool_dict");
  assert(0 == strcmp(name_at_FildeshSxpb(sxpb, it), "bool_dict"));
  val_it = lookup_subfield_at_FildeshSxpb(sxpb, it, "alpha");
  assert(bool_value_at_FildeshSxpb(sxpb, val_it));
  val_it = lookup_subfield_at_FildeshSxpb(sxpb, it, "beta");
  assert(!bool_value_at_FildeshSxpb(sxpb, val_it));

  it = lookup_subfield_at_FildeshSxpb(sxpb, top_it, "message_dict");
  assert(0 == strcmp(name_at_FildeshSxpb(sxpb, it), "message_dict"));
  val_it = lookup_subfield_at_FildeshSxpb(sxpb, it, "first");
  assert(val_it.field_kind == FildeshSxprotoFieldKind_MESSAGE);
  assert(0 == strcmp("one", str_value_at_FildeshSxpb(
          sxpb, lookup_subfield_at_FildeshSxpb(sxpb, val_it, "car"))));
  val_it = lookup_subfield_at_FildeshSxpb(sxpb, it, "second");
  assert(val_it.field_kind == FildeshSxprotoFieldKind_MESSAGE);
  assert(0 == strcmp("two", str_value_at_FildeshSxpb(
          sxpb, lookup_subfield_at_FildeshSxpb(sxpb, val_it, "car"))));

  close_FildeshSxpb(sxpb);
  close_FildeshO(err_out);
}

static
  void
manyof_test()
{
  DECLARE_STRLIT_FildeshX(in, manyof_test_content);
  FildeshO* err_out = open_FildeshOF("/dev/stderr");
  const FildeshSxprotoField* const schema = sxproto_schema();
  FildeshSxpb* const sxpb = slurp_sxpb_close_FildeshX(in, schema, err_out);
  const FildeshSxpbIT top_it = top_of_FildeshSxpb(sxpb);
  FildeshSxpbIT it;

  assert(sxpb);

  it = lookup_subfield_at_FildeshSxpb(sxpb, top_it, "predicates");
  assert(0 == strcmp(name_at_FildeshSxpb(sxpb, it), "predicates"));
  {
    it = first_at_FildeshSxpb(sxpb, it);
    assert(!name_at_FildeshSxpb(sxpb, it));
    assert(0 == strcmp(str_value_at_FildeshSxpb(sxpb, it), "alpha"));

    it = next_at_FildeshSxpb(sxpb, it);
    assert(!name_at_FildeshSxpb(sxpb, it));
    assert(0 == strcmp(str_value_at_FildeshSxpb(sxpb, it), "beta"));

    it = next_at_FildeshSxpb(sxpb, it);
    assert(!name_at_FildeshSxpb(sxpb, it));
    assert(0 == strcmp(str_value_at_FildeshSxpb(sxpb, it), "gamma"));

    it = next_at_FildeshSxpb(sxpb, it);
    assert(!name_at_FildeshSxpb(sxpb, it));
    assert(0 == strcmp(str_value_at_FildeshSxpb(sxpb, it), "delta"));

    it = next_at_FildeshSxpb(sxpb, it);
    assert(!name_at_FildeshSxpb(sxpb, it));
    assert(0 == strcmp(str_value_at_FildeshSxpb(sxpb, it), "epsilon zeta"));

    it = next_at_FildeshSxpb(sxpb, it);
    assert(0 == strcmp(name_at_FildeshSxpb(sxpb, it), "b"));
    assert(!bool_value_at_FildeshSxpb(sxpb, it));

    it = next_at_FildeshSxpb(sxpb, it);
    assert(0 == strcmp(name_at_FildeshSxpb(sxpb, it), "or"));
    assert(nullish_FildeshSxpbIT(next_at_FildeshSxpb(sxpb, it)));

    it = first_at_FildeshSxpb(sxpb, it);
    assert(0 == strcmp(name_at_FildeshSxpb(sxpb, it), "b"));
    assert(bool_value_at_FildeshSxpb(sxpb, it));

    it = next_at_FildeshSxpb(sxpb, it);
    assert(0 == strcmp(name_at_FildeshSxpb(sxpb, it), "b"));
    assert(!bool_value_at_FildeshSxpb(sxpb, it));

    it = next_at_FildeshSxpb(sxpb, it);
    assert(0 == strcmp(name_at_FildeshSxpb(sxpb, it), "u"));
    assert(bool_value_at_FildeshSxpb(sxpb, it));

    it = next_at_FildeshSxpb(sxpb, it);
    assert(0 == strcmp(name_at_FildeshSxpb(sxpb, it), "u"));
    assert(!bool_value_at_FildeshSxpb(sxpb, it));
    assert(nullish_FildeshSxpbIT(next_at_FildeshSxpb(sxpb, it)));
  }

  it = lookup_subfield_at_FildeshSxpb(sxpb, top_it, "many_fruits");
  assert(0 == strcmp(name_at_FildeshSxpb(sxpb, it), "many_fruits"));
  {
    FildeshSxpbIT val_it;
    it = first_at_FildeshSxpb(sxpb, it);
    val_it = lookup_subfield_at_FildeshSxpb(sxpb, it, "banana");
    assert(bool_value_at_FildeshSxpb(sxpb, val_it));

    it = next_at_FildeshSxpb(sxpb, it);

    it = next_at_FildeshSxpb(sxpb, it);
    val_it = lookup_subfield_at_FildeshSxpb(sxpb, it, "apple");
    assert(!bool_value_at_FildeshSxpb(sxpb, val_it));

    assert(nullish_FildeshSxpbIT(next_at_FildeshSxpb(sxpb, it)));
  }

  close_FildeshSxpb(sxpb);
  close_FildeshO(err_out);
}

static
  void
manyof_coercion_test()
{
  DECLARE_STRLIT_FildeshX(in, manyof_coercion_test_content);
  FildeshO* err_out = open_FildeshOF("/dev/stderr");
  const FildeshSxprotoField* const schema = sxproto_schema();
  FildeshSxpb* const sxpb = slurp_sxpb_close_FildeshX(in, schema, err_out);
  const FildeshSxpbIT top_it = top_of_FildeshSxpb(sxpb);
  FildeshSxpbIT it;
  FildeshSxpbIT val_it;

  assert(sxpb);

  val_it = lookup_subfield_at_FildeshSxpb(sxpb, top_it, "b");
  assert(bool_value_at_FildeshSxpb(sxpb, val_it));
  val_it = lookup_subfield_at_FildeshSxpb(sxpb, top_it, "n");
  assert(2 == unsigned_value_at_FildeshSxpb(sxpb, val_it));
  val_it = lookup_subfield_at_FildeshSxpb(sxpb, top_it, "f");
  assert(2.5f == float_value_at_FildeshSxpb(sxpb, val_it));
  val_it = lookup_subfield_at_FildeshSxpb(sxpb, top_it, "s");
  assert(0 == strcmp("seven", str_value_at_FildeshSxpb(sxpb, val_it)));

  it = lookup_subfield_at_FildeshSxpb(sxpb, top_it, "cons");
  assert(0 == strcmp(name_at_FildeshSxpb(sxpb, it), "cons"));
  val_it = lookup_subfield_at_FildeshSxpb(sxpb, it, "car");
  assert(0 == strcmp("kappa", str_value_at_FildeshSxpb(sxpb, val_it)));

  it = lookup_subfield_at_FildeshSxpb(sxpb, top_it, "predicates");
  assert(0 == strcmp(name_at_FildeshSxpb(sxpb, it), "predicates"));
  it = first_at_FildeshSxpb(sxpb, it);
  assert(0 == strcmp(str_value_at_FildeshSxpb(sxpb, it), "alpha"));
  it = next_at_FildeshSxpb(sxpb, it);
  assert(0 == strcmp(str_value_at_FildeshSxpb(sxpb, it), "beta"));
  it = next_at_FildeshSxpb(sxpb, it);
  assert(0 == strcmp(str_value_at_FildeshSxpb(sxpb, it), "gamma"));
  assert(nullish_FildeshSxpbIT(next_at_FildeshSxpb(sxpb, it)));

  it = lookup_subfield_at_FildeshSxpb(sxpb, top_it, "a");
  assert(0 == strcmp(name_at_FildeshSxpb(sxpb, it), "a"));
  it = first_at_FildeshSxpb(sxpb, it);
  assert(1.0f == float_value_at_FildeshSxpb(sxpb, it));
  it = next_at_FildeshSxpb(sxpb, it);
  assert(2.0f == float_value_at_FildeshSxpb(sxpb, it));
  it = next_at_FildeshSxpb(sxpb, it);
  assert(3.0f == float_value_at_FildeshSxpb(sxpb, it));
  assert(nullish_FildeshSxpbIT(next_at_FildeshSxpb(sxpb, it)));

  it = lookup_subfield_at_FildeshSxpb(sxpb, top_it, "many_fruits");
  assert(0 == strcmp(name_at_FildeshSxpb(sxpb, it), "many_fruits"));
  it = first_at_FildeshSxpb(sxpb, it);
  assert(0 == strcmp(name_at_FildeshSxpb(sxpb, it), "banana"));
  assert(bool_value_at_FildeshSxpb(sxpb, it));
  it = next_at_FildeshSxpb(sxpb, it);
  assert(0 == strcmp(name_at_FildeshSxpb(sxpb, it), "apple"));
  assert(bool_value_at_FildeshSxpb(sxpb, it));
  it = next_at_FildeshSxpb(sxpb, it);
  assert(0 == strcmp(name_at_FildeshSxpb(sxpb, it), "banana"));
  assert(!bool_value_at_FildeshSxpb(sxpb, it));
  assert(nullish_FildeshSxpbIT(next_at_FildeshSxpb(sxpb, it)));

  close_FildeshSxpb(sxpb);
  close_FildeshO(err_out);
}

static
  void
append_to_empty_array_test()
{
  DECLARE_STRLIT_FildeshX(
      in, "(a_alias (()))((+. a_alias) (()) 1 2 3)");
  FildeshO err_out[1] = {DEFAULT_FildeshO};
  const FildeshSxprotoField* const schema = sxproto_schema();
  FildeshSxpb* const sxpb = slurp_sxpb_close_FildeshX(in, schema, err_out);
  FildeshSxpbIT it;
  unsigned i;

  assert(sxpb);
  assert(err_out->size == 0);
  it = lookup_subfield_at_FildeshSxpb(
      sxpb, top_of_FildeshSxpb(sxpb), "a");
  assert(it.field_kind == FildeshSxprotoFieldKind_ARRAY);
  it = first_at_FildeshSxpb(sxpb, it);
  for (i = 1; i <= 3; ++i) {
    assert(it.field_kind == FildeshSxprotoFieldKind_LITERAL_FLOAT);
    assert((float)i == float_value_at_FildeshSxpb(sxpb, it));
    it = next_at_FildeshSxpb(sxpb, it);
  }
  assert(nullish_FildeshSxpbIT(it));

  close_FildeshSxpb(sxpb);
  close_FildeshO(err_out);
}

static
  void
append_manyof_elements_to_empty_manyof_test()
{
  DECLARE_STRLIT_FildeshX(
      in,
      "((predicates))"
      "((+. predicates) (()) (b +true) \"alpha\" (u 1) \"omega\")");
  FildeshO err_out[1] = {DEFAULT_FildeshO};
  const FildeshSxprotoField* const schema = sxproto_schema();
  FildeshSxpb* const sxpb = slurp_sxpb_close_FildeshX(in, schema, err_out);
  FildeshSxpbIT it;

  assert(sxpb);
  assert(err_out->size == 0);
  it = lookup_subfield_at_FildeshSxpb(
      sxpb, top_of_FildeshSxpb(sxpb), "predicates");
  assert(it.field_kind == FildeshSxprotoFieldKind_MANYOF);
  it = first_at_FildeshSxpb(sxpb, it);
  assert(0 == strcmp("b", name_at_FildeshSxpb(sxpb, it)));
  assert(bool_value_at_FildeshSxpb(sxpb, it));
  it = next_at_FildeshSxpb(sxpb, it);
  assert(NULL == name_at_FildeshSxpb(sxpb, it));
  assert(0 == strcmp("alpha", str_value_at_FildeshSxpb(sxpb, it)));
  it = next_at_FildeshSxpb(sxpb, it);
  assert(0 == strcmp("u", name_at_FildeshSxpb(sxpb, it)));
  assert(1 == unsigned_value_at_FildeshSxpb(sxpb, it));
  it = next_at_FildeshSxpb(sxpb, it);
  assert(NULL == name_at_FildeshSxpb(sxpb, it));
  assert(0 == strcmp("omega", str_value_at_FildeshSxpb(sxpb, it)));
  assert(nullish_FildeshSxpbIT(next_at_FildeshSxpb(sxpb, it)));

  close_FildeshSxpb(sxpb);
  close_FildeshO(err_out);

  {
    DECLARE_STRLIT_FildeshX(
        bad_in, "((predicates))((+. predicates) (()) (unknown 1))");
    FildeshO bad_err_out[1] = {DEFAULT_FildeshO};
    FildeshSxpb* const bad_sxpb = slurp_sxpb_close_FildeshX(
        bad_in, schema, bad_err_out);
    assert(!bad_sxpb);
    assert(bad_err_out->size > 0);
    close_FildeshO(bad_err_out);
  }

  {
    DECLARE_STRLIT_FildeshX(
        bad_in, "((many_fruits))((+. many_fruits) (()) 1)");
    FildeshO bad_err_out[1] = {DEFAULT_FildeshO};
    FildeshSxpb* const bad_sxpb = slurp_sxpb_close_FildeshX(
        bad_in, schema, bad_err_out);
    assert(!bad_sxpb);
    assert(bad_err_out->size > 0);
    close_FildeshO(bad_err_out);
  }
}

static
  void
nest_test()
{
  DECLARE_STRLIT_FildeshX(in, nest_test_content);
  FildeshO* err_out = open_FildeshOF("/dev/stderr");
  const FildeshSxprotoField* const schema = sxproto_schema();
  FildeshSxpb* const sxpb = slurp_sxpb_close_FildeshX(in, schema, err_out);
  const FildeshSxpbIT top_it = top_of_FildeshSxpb(sxpb);
  FildeshSxpbIT it;
  FildeshSxpbIT val_it;

  assert(sxpb);
  it = lookup_subfield_at_FildeshSxpb(sxpb, top_it, "nest");
  assert(0 == strcmp(name_at_FildeshSxpb(sxpb, it), "nest"));
  assert(it.field_kind == FildeshSxprotoFieldKind_NEST);

  val_it = first_at_FildeshSxpb(sxpb, it);
  assert(val_it.field_kind == FildeshSxprotoFieldKind_NEST);
  assert(0 == strcmp(name_at_FildeshSxpb(sxpb, val_it), "a"));
  assert(0 == strcmp(str_value_at_FildeshSxpb(
          sxpb, first_at_FildeshSxpb(sxpb, val_it)), "b"));

  val_it = next_at_FildeshSxpb(sxpb, val_it);
  assert(val_it.field_kind == FildeshSxprotoFieldKind_LITERAL_STRING);
  assert(0 == strcmp(str_value_at_FildeshSxpb(sxpb, val_it), "e"));
  assert(nullish_FildeshSxpbIT(next_at_FildeshSxpb(sxpb, val_it)));

  close_FildeshSxpb(sxpb);
  close_FildeshO(err_out);
}

static
  void
dict_schema_error_test()
{
  FildeshX in[1];
  FildeshO err_out[1] = {DEFAULT_FildeshO};
  const FildeshSxprotoField* const schema = sxproto_schema();
  FildeshSxpb* sxpb;

  *in = FildeshX_of_strlit("(message_dict () (first not_a_message))");
  sxpb = slurp_sxpb_close_FildeshX(in, schema, err_out);
  assert(!sxpb);
  putc_FildeshO(err_out, '\0');
  assert(strstr(err_out->at, "Expected dict value to be a message."));

  truncate_FildeshO(err_out);
  *in = FildeshX_of_strlit("(int_dict () (alpha (value 1)))");
  sxpb = slurp_sxpb_close_FildeshX(in, schema, err_out);
  assert(!sxpb);
  putc_FildeshO(err_out, '\0');
  assert(strstr(err_out->at, "Expected dict value to be a literal."));

  truncate_FildeshO(err_out);
  *in = FildeshX_of_strlit("(string_dict not_a_dict)");
  sxpb = slurp_sxpb_close_FildeshX(in, schema, err_out);
  assert(!sxpb);
  putc_FildeshO(err_out, '\0');
  assert(strstr(err_out->at, "Expected field to be a dict."));

  truncate_FildeshO(err_out);
  *in = FildeshX_of_strlit("(unknown x)");
  sxpb = slurp_sxpb_close_FildeshX(in, schema, err_out);
  assert(!sxpb);
  putc_FildeshO(err_out, '\0');
  assert(strstr(err_out->at, "Unrecognized field name."));

  close_FildeshO(err_out);
}

static
  void
nest_schema_error_test()
{
  FildeshX in[1];
  FildeshO err_out[1] = {DEFAULT_FildeshO};
  const FildeshSxprotoField* const schema = sxproto_schema();
  FildeshSxpb* sxpb;

  *in = FildeshX_of_strlit("(nest not_a_nest)");
  sxpb = slurp_sxpb_close_FildeshX(in, schema, err_out);
  assert(!sxpb);
  putc_FildeshO(err_out, '\0');
  assert(strstr(err_out->at, "Expected field to be a nest."));
  close_FildeshO(err_out);
}

int main() {
  literal_test();
  message_test();
  loneof_test();
  array_test();
  dict_test();
  manyof_test();
  manyof_coercion_test();
  append_to_empty_array_test();
  append_manyof_elements_to_empty_manyof_test();
  nest_test();
  dict_schema_error_test();
  nest_schema_error_test();
  return 0;
}
