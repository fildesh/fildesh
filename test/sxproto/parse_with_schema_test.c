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
static const char manyof_test_content[] = "\
((predicates)\n\
 \"alpha\"\n\
 \"beta\"\n\
 \"gamma\"\n\
 \"delta\"\n\
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
    {"cons", FILL_FildeshSxprotoField_MESSAGE(m_fields)},
    {"many_fruits", FILL_FildeshSxprotoField_MANYOF(fruit_loneof)},
    {"messages", FILL_FildeshSxprotoField_MESSAGES(m_fields)},
    {"predicates", FILL_FildeshSxprotoField_MANYOF(predicates_manyof)},
    {"s", FILL_FildeshSxprotoField_STRING(1, 64)},
    {"s_alternate_name", FILL_DEFAULT_FildeshSxprotoField_ALIAS},
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

int main() {
  literal_test();
  message_test();
  loneof_test();
  array_test();
  manyof_test();
  manyof_coercion_test();
  return 0;
}
