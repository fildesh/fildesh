#include <assert.h>
#include <string.h>

#include <fildesh/sxproto.h>

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
parse_with_schema_test()
{
  static const char content[] =
    "\
    (b true)\n\
    (n 10)\n\
    (f 2.5)\n\
    (s_alternate_name \"Kappa ¬‿¬\")\n\
    (((predicates))\n\
     \"alpha\"\n\
     \"beta\"\n\
     \"gamma\"\n\
     \"delta\"\n\
     (b false)\n\
     (((or))\n\
      (b 1)\n\
      (b_alias 0)\n\
      (u 1)\n\
      (u 0)))\n\
    ((a) 0.5e1 4 30e-1 2.e0 1)\n\
    (cons (car \"first\") (cdr (car \"second\") (cdr (car \"third\") (cdr))))\n\
    ((messages)\n\
     (() (car \"schwam\"))\n\
     (())\n\
     (() (\"car\" \"doo\") (cdr (car \"two and heif\"))))\n\
    ((fruit_as banana) true)\n\
    ";
  DECLARE_STRLIT_FildeshX(in, content);
  FildeshO* err_out = open_FildeshOF("/dev/stderr");
  const FildeshSxprotoField* const schema = (
      (void)sxproto_schema(),
      sxproto_schema());
  FildeshSxpb* const sxpb = slurp_sxpb_close_FildeshX(in, schema, err_out);
  const FildeshSxpbIT top_it = top_of_FildeshSxpb(sxpb);
  FildeshSxpbIT it;
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

  it = lookup_subfield_at_FildeshSxpb(sxpb, top_it, "a");
    assert(0 == strcmp(name_at_FildeshSxpb(sxpb, it), "a"));
  /* Message fields are stored in lexicographic order. This one is first.*/
  assert(it.elem_id == first_at_FildeshSxpb(sxpb, top_it).elem_id);
  {
    it = first_at_FildeshSxpb(sxpb, it);
    assert(!nullish_FildeshSxpbIT(it));
    assert(5.0f == float_value_at_FildeshSxpb(sxpb, it));

    it = next_at_FildeshSxpb(sxpb, it);
    assert(!nullish_FildeshSxpbIT(it));
    assert(4.0f == float_value_at_FildeshSxpb(sxpb, it));

    it = next_at_FildeshSxpb(sxpb, it);
    assert(!nullish_FildeshSxpbIT(it));
    assert(3.0f == float_value_at_FildeshSxpb(sxpb, it));

    it = next_at_FildeshSxpb(sxpb, it);
    assert(!nullish_FildeshSxpbIT(it));
    assert(2.0f == float_value_at_FildeshSxpb(sxpb, it));

    it = next_at_FildeshSxpb(sxpb, it);
    assert(!nullish_FildeshSxpbIT(it));
    assert(1.0f == float_value_at_FildeshSxpb(sxpb, it));

    assert(nullish_FildeshSxpbIT(next_at_FildeshSxpb(sxpb, it)));
  }

  it = lookup_subfield_at_FildeshSxpb(sxpb, top_it, "cons");
  assert(0 == strcmp(name_at_FildeshSxpb(sxpb, it), "cons"));
  {
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
  }

  it = lookup_subfield_at_FildeshSxpb(sxpb, top_it, "messages");
  assert(0 == strcmp(name_at_FildeshSxpb(sxpb, it), "messages"));
  {
    it = first_at_FildeshSxpb(sxpb, it);
    good = lone_subfield_at_FildeshSxpb_to_str(&tmp_s, sxpb, it, "car");
    assert(good);
    assert(0 == strcmp(tmp_s, "schwam"));

    it = next_at_FildeshSxpb(sxpb, it);
    /* Empty message.*/
    assert(nullish_FildeshSxpbIT(first_at_FildeshSxpb(sxpb, it)));

    it = next_at_FildeshSxpb(sxpb, it);
    good = lone_subfield_at_FildeshSxpb_to_str(&tmp_s, sxpb, it, "car");
    assert(good);
    assert(0 == strcmp(tmp_s, "doo"));
    assert(nullish_FildeshSxpbIT(next_at_FildeshSxpb(sxpb, it)));

    it = lookup_subfield_at_FildeshSxpb(sxpb, it, "cdr");
    good = lone_subfield_at_FildeshSxpb_to_str(&tmp_s, sxpb, it, "car");
    assert(good);
    assert(0 == strcmp(tmp_s, "two and heif"));

    it = lookup_subfield_at_FildeshSxpb(sxpb, it, "cdr");
    assert(nullish_FildeshSxpbIT(it));
  }

  it = lookup_subfield_at_FildeshSxpb(sxpb, top_it, "fruit_as");
  assert(0 == strcmp(name_at_FildeshSxpb(sxpb, it), "fruit_as"));
  {
    good = lone_subfield_at_FildeshSxpb_to_bool(&tmp_b, sxpb, it, "banana");
    assert(good);
    assert(tmp_b);
  }

  close_FildeshSxpb(sxpb);
  close_FildeshO(err_out);
}

int main() {
  parse_with_schema_test();
  return 0;
}
