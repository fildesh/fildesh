#define FILDESH_LOG_TRACE_ON
#include "src/sxproto/parse_sxpb.h"

#include <assert.h>
#include <string.h>

static void parse_string_test() {
  FildeshSxpbInfo info[1] = {DEFAULT_FildeshSxpbInfo};
  FildeshO oslice[1] = {DEFAULT_FildeshO};

#define expectparse(expect, text) do { \
  FildeshX slice = FildeshX_of_strlit(text); \
  bool good = parse_concat_string_FildeshSxpbInfo(info, &slice, oslice); \
  assert(good); \
  putc_FildeshO(oslice, '\0'); \
  fildesh_log_trace(oslice->at); \
  oslice->size -= 1; \
  assert(strlen(expect) == oslice->size); \
  assert(0 == memcmp(expect, oslice->at, oslice->size)); \
  truncate_FildeshO(oslice); \
} while (0)

  /* Quoted.*/
  expectparse("abcdef", "\"abcdef\"");
  expectparse("abcdef", "\"abcdef\"");
  expectparse("ab\"cd", "\"ab\\\"cd\"");
  expectparse("ab\\", "\"ab\\\\\"");
  expectparse("a  \n b", "\"a  \\n b\"");
  expectparse("a\nb\nc", "\"a\nb\r\nc\"");

  /* Unquoted.*/
  expectparse("abcdef", "abcdef");
  expectparse("ab'cd", "ab'cd");
  expectparse("ab\\cd", "ab\\cd");
  expectparse("-", "-");
  expectparse(".", ".");
  expectparse("--", "--");
  expectparse("..", "..");
  expectparse("-bare", "-bare");
  expectparse(".bare", ".bare");

  /* Empty.*/
  expectparse("", "\"\"");
  expectparse("", "\"\"\"\"\"\"");

#undef expectparse
  close_FildeshO(oslice);
}

static void parse_number_test() {
  FildeshSxpbInfo info[1] = {DEFAULT_FildeshSxpbInfo};
  FildeshO oslice[1] = {DEFAULT_FildeshO};
  info->err_out = open_FildeshOF("/dev/stderr");

#define expectparse(expect, text) do { \
  FildeshX slice = FildeshX_of_strlit(text); \
  bool good = parse_number_FildeshSxpbInfo(info, &slice, oslice); \
  assert(good); \
  putc_FildeshO(oslice, '\0'); \
  fildesh_log_trace(oslice->at); \
  oslice->size -= 1; \
  assert(strlen(expect) == oslice->size); \
  assert(0 == memcmp(expect, oslice->at, oslice->size)); \
} while (0)

  expectparse("+12345", "12345");
  expectparse("-12345", "-12345");
  expectparse("+1", "1");
  expectparse("+1.e+5", "100000.");
  expectparse("+1.e+5", "100000.0");
  expectparse("+1.023e+2", "102.3");
  expectparse("+2.e-3", ".0020");
  expectparse("+5.e+0", "5.");
  expectparse("+5.e-1", ".5");
  expectparse("+5.e-1", "+.5");
  expectparse("-5.e-1", "-.5");

#undef expectparse
  close_FildeshO(info->err_out);
  close_FildeshO(oslice);
}

static void parse_number_failure_test() {
  FildeshSxpbInfo info[1] = {DEFAULT_FildeshSxpbInfo};
  FildeshO oslice[1] = {DEFAULT_FildeshO};
  info->err_out = open_FildeshOF("/dev/stderr");

#define expectfail(text) do { \
  FildeshX slice = FildeshX_of_strlit(text); \
  bool good = parse_number_FildeshSxpbInfo(info, &slice, oslice); \
  assert(!good); \
} while (0)

  expectfail("-.");
  expectfail("+.");
  expectfail(".");

#undef expectfail
  close_FildeshO(info->err_out);
  close_FildeshO(oslice);
}

static void parse_name_test() {
  FildeshSxpbInfo info[1] = {DEFAULT_FildeshSxpbInfo};
  FildeshO oslice[1] = {DEFAULT_FildeshO};

#define expectparse(expect, expect_depth, text) do { \
  FildeshX slice = FildeshX_of_strlit(text); \
  unsigned nesting_depth = 0; \
  bool good = parse_name_FildeshSxpbInfo(info, &slice, oslice, &nesting_depth, \
                                         FildeshSxprotoFieldKind_MESSAGE); \
  assert(good); \
  putc_FildeshO(oslice, '\0'); \
  fildesh_log_trace(oslice->at); \
  oslice->size -= 1; \
  assert(strlen(expect) == oslice->size); \
  assert(0 == memcmp(expect, oslice->at, oslice->size)); \
  assert(expect_depth == nesting_depth); \
} while (0)

  expectparse("x", 0, "x");
  expectparse("x", 5, "x () (x 5)");
  expectparse("x", 5, "x ()");
  expectparse("y", 1, "y (())");
  expectparse("y", 1, "y (()) (() (x 5))");
  expectparse("y", 2, "y (()) (x 5)");
  expectparse("", 0, "()");
  expectparse("", 0, "() (x 5)");
  expectparse("", 0, "() (() (x 5))");
  expectparse("", 0, "\"\" anonymous discriminated string");
  /* Quoted names.*/
  expectparse("abc", 0, "\"abc\"");
  expectparse("(a\"bc", 5, "\"(a\\\"bc\" ()");

#undef expectparse
  close_FildeshO(oslice);
}

static void parse_field_test() {
  FildeshSxpbInfo info[1] = {DEFAULT_FildeshSxpbInfo};
  FildeshO oslice[1] = {DEFAULT_FildeshO};
  FildeshSxpb* sxpb = open_FildeshSxpb();
  const FildeshSxpbIT p_it = top_of_FildeshSxpb(sxpb);
  info->err_out = open_FildeshOF("/dev/stderr");

#define tryparse(text) do { \
  FildeshX slice = FildeshX_of_strlit(text); \
  bool good = parse_field_FildeshSxpbInfo(info, NULL,  &slice, sxpb, p_it, oslice); \
  assert(good); \
  assert(info->line_count == 0); \
  assert(info->column_count == strlen(text)); \
  remove_at_FildeshSxpb(sxpb, first_at_FildeshSxpb(sxpb, p_it)); \
  assert(nullish_FildeshSxpbIT(first_at_FildeshSxpb(sxpb, p_it))); \
  info->column_count = 0; \
} while (0)

  tryparse("(x 5)");
  tryparse("(y \"hello\")");
  tryparse("(z 5.254)");
  tryparse("(m (x 5) (y 7) (z 12))");
  tryparse("(m (x 5) (\"y\" 7) (z 12))");
  tryparse("(d () (x 5) (y 7))");
  tryparse("(d () (x 5) (y 7.5))");
  tryparse("(d () (x +true) (y 0))");
  tryparse("(d () (x (a 1)) (y (b 2)))");
  tryparse("(d () ((key option) (()) 1 2 3))");
  tryparse("((my_loneof option) () (key1 1) (key2 2))");
  tryparse("(empty_dict ())");
  tryparse("((do_with x) 5)");
  tryparse("((do_with m) (x 1) (y 2))");
  tryparse("((do_with empty_m))");
  tryparse("(a (()) 1 2 3 4 5)");
  tryparse("(a (()) 1 2.2 3)");
  tryparse("(\"a\" (()) 1 2 3 4 5)");
  tryparse("(a (()) (() (x 5)) (() (y 7)) () (() (z 12)))");
  tryparse("(a (()) x y z)");
  tryparse("(a (()) (\"\" x) (\"\" y) (\"\" z))");
  tryparse("(\"a\" (()) \"x\" \"y\" \"z\")");
  tryparse("(\"a\" (()) (\"\" \"x\") (\"\" \"y\") (\"\" \"z\"))");
  tryparse("((a) (() (x 5)) (() (y 7)) () (() (z 12)))");
  tryparse("((b) 1 2 3 4 5)");

#undef tryparse
  close_FildeshO(oslice);
  close_FildeshO(info->err_out);
  close_FildeshSxpb(sxpb);
}

static void parse_field_name_validity_test() {
  /* Message field names must be valid bare strings; numbers, booleans, and
   * digit-leading PLAIN words are rejected (parity with sxpb-py). */
#define expectbadname(text) do { \
  FildeshX slice = FildeshX_of_strlit(text); \
  FildeshO eo[1] = {DEFAULT_FildeshO}; \
  FildeshSxpb* s = slurp_sxpb_close_FildeshX(&slice, NULL, eo); \
  assert(NULL == s); \
  assert(eo->size > 0); \
  close_FildeshO(eo); \
} while (0)
  expectbadname("(+.5 1)");
  expectbadname("(-.5 1)");
  expectbadname("(+1 1)");
  expectbadname("(+true 1)");
  expectbadname("(50mm 1)");
  expectbadname("(123 1)");
  expectbadname("(+truex 1)");
  expectbadname("((50mm) 1)");
  expectbadname("(( ; comment\n +truex) 1)");
#undef expectbadname

#define expectgoodname(name) do { \
  FildeshX slice = FildeshX_of_strlit("(" name " 1)"); \
  FildeshO eo[1] = {DEFAULT_FildeshO}; \
  FildeshSxpb* s = slurp_sxpb_close_FildeshX(&slice, NULL, eo); \
  FildeshSxpbIT it; \
  assert(s); \
  it = lookup_subfield_at_FildeshSxpb(s, top_of_FildeshSxpb(s), name); \
  assert(!nullish_FildeshSxpbIT(it)); \
  close_FildeshSxpb(s); \
  close_FildeshO(eo); \
} while (0)
  expectgoodname("abc");
  expectgoodname("-foo");
  expectgoodname("--x");
  expectgoodname("_");
#undef expectgoodname

  /* Quoting leaves otherwise reserved or non-bare names unambiguous. */
#define expectgoodquotedname(name) do { \
  FildeshX slice = FildeshX_of_strlit("(\"" name "\" 1)"); \
  FildeshO eo[1] = {DEFAULT_FildeshO}; \
  FildeshSxpb* s = slurp_sxpb_close_FildeshX(&slice, NULL, eo); \
  FildeshSxpbIT it; \
  assert(s); \
  it = lookup_subfield_at_FildeshSxpb(s, top_of_FildeshSxpb(s), name); \
  assert(!nullish_FildeshSxpbIT(it)); \
  close_FildeshSxpb(s); \
  close_FildeshO(eo); \
} while (0)
  expectgoodquotedname("50mm");
  expectgoodquotedname("+.");
#undef expectgoodquotedname

  {
    FildeshX in = FildeshX_of_strlit("((\"50mm\") 1)");
    FildeshO err_out[1] = {DEFAULT_FildeshO};
    FildeshSxpb* sxpb = slurp_sxpb_close_FildeshX(&in, NULL, err_out);
    FildeshSxpbIT it;
    assert(sxpb);
    it = lookup_subfield_at_FildeshSxpb(
        sxpb, top_of_FildeshSxpb(sxpb), "50mm");
    assert(!nullish_FildeshSxpbIT(it));
    close_FildeshSxpb(sxpb);
    close_FildeshO(err_out);
  }

  /* Nest keys accept PLAIN words (numeric keys remain valid), but a
   * reserved-prefix starter is rejected. */
  {
    FildeshX in = FildeshX_of_strlit(
        "(n (\"\") (50mm x) (123 y) (\"+.\" z))");
    FildeshO err_out[1] = {DEFAULT_FildeshO};
    FildeshSxpb* sxpb = slurp_sxpb_close_FildeshX(&in, NULL, err_out);
    FildeshSxpbIT it;
    assert(sxpb);
    it = lookup_subfield_at_FildeshSxpb(sxpb, top_of_FildeshSxpb(sxpb), "n");
    assert(!nullish_FildeshSxpbIT(it));
    it = first_at_FildeshSxpb(sxpb, it);
    assert(0 == strcmp("50mm", name_at_FildeshSxpb(sxpb, it)));
    it = next_at_FildeshSxpb(sxpb, it);
    assert(0 == strcmp("123", name_at_FildeshSxpb(sxpb, it)));
    it = next_at_FildeshSxpb(sxpb, it);
    assert(0 == strcmp("+.", name_at_FildeshSxpb(sxpb, it)));
    close_FildeshSxpb(sxpb);
    close_FildeshO(err_out);
  }
#define expectbadnestkey(text) do { \
  FildeshX slice = FildeshX_of_strlit(text); \
  FildeshO eo[1] = {DEFAULT_FildeshO}; \
  FildeshSxpb* s = slurp_sxpb_close_FildeshX(&slice, NULL, eo); \
  assert(NULL == s); \
  assert(eo->size > 0); \
  close_FildeshO(eo); \
} while (0)
  expectbadnestkey("(n (\"\") (+truex x))");
  expectbadnestkey("(n (\"\") (-.x x))");
#undef expectbadnestkey
}

static void parse_string_field_test() {
  FildeshSxpbInfo info[1] = {DEFAULT_FildeshSxpbInfo};
  FildeshO oslice[1] = {DEFAULT_FildeshO};
  FildeshSxpb* sxpb = open_FildeshSxpb();
  const FildeshSxpbIT p_it = top_of_FildeshSxpb(sxpb);
  info->err_out = open_FildeshOF("/dev/stderr");
#define expectparse(expect, text) do { \
  FildeshX slice = FildeshX_of_strlit("(s " text ")"); \
  const char* result = NULL; \
  assert(parse_field_FildeshSxpbInfo(info, NULL,  &slice, sxpb, p_it, oslice)); \
  assert(lone_subfield_at_FildeshSxpb_to_str(&result, sxpb, p_it, "s")); \
  fildesh_log_trace(result); \
  assert(strlen(expect) == strlen(result)); \
  assert(0 == memcmp(expect, result, strlen(expect))); \
  remove_at_FildeshSxpb(sxpb, first_at_FildeshSxpb(sxpb, p_it)); \
  assert(nullish_FildeshSxpbIT(first_at_FildeshSxpb(sxpb, p_it))); \
  info->column_count = 0; \
} while (0)

  expectparse("AA BB CC", "\"AA BB CC\"");
  expectparse("AA BB CC", "\"AA B\" \"B CC\"");
  expectparse("AA BB CC", "AA BB CC");
  expectparse("AA BB CC", "\"\" AA BB CC");
  expectparse("AA BB CC", "\"AA \" BB CC");
  expectparse("AA BB CC", "AA \" BB \" CC");
  expectparse("AA BB CC", "AA BB \" CC\"");

  expectparse("1 2 3", "\"\" 1 2 3");

#undef expectparse
  close_FildeshO(oslice);
  close_FildeshO(info->err_out);
  close_FildeshSxpb(sxpb);
}

static void parse_last_in_string_array_field_test() {
  FildeshSxpbInfo info[1] = {DEFAULT_FildeshSxpbInfo};
  FildeshO oslice[1] = {DEFAULT_FildeshO};
  FildeshSxpb* sxpb = open_FildeshSxpb();
  const FildeshSxpbIT p_it = top_of_FildeshSxpb(sxpb);
  info->err_out = open_FildeshOF("/dev/stderr");

#define expectparse(expect, text) do { \
  FildeshX slice = FildeshX_of_strlit("(my_array (()) " text ")"); \
  const char* result = NULL; \
  FildeshSxpbIT it; \
  assert(parse_field_FildeshSxpbInfo(info, NULL,  &slice, sxpb, p_it, oslice)); \
  it = lookup_subfield_at_FildeshSxpb(sxpb, p_it, "my_array"); \
  assert(!nullish_FildeshSxpbIT(it)); \
  it = first_at_FildeshSxpb(sxpb, it); \
  while (!nullish_FildeshSxpbIT(next_at_FildeshSxpb(sxpb, it))) { \
    it = next_at_FildeshSxpb(sxpb, it); \
  } \
  result = str_value_at_FildeshSxpb(sxpb, it); \
  fildesh_log_trace(result); \
  assert(strlen(expect) == strlen(result)); \
  assert(0 == memcmp(expect, result, strlen(expect))); \
  remove_at_FildeshSxpb(sxpb, first_at_FildeshSxpb(sxpb, p_it)); \
  assert(nullish_FildeshSxpbIT(first_at_FildeshSxpb(sxpb, p_it))); \
  info->column_count = 0; \
} while (0)

  expectparse("hello", "hello");
  expectparse("hello", "hi \"hello\"");
  expectparse("", "(\"\" \"\")");
  expectparse("fourty five", "(\"\" fourty five)");
  expectparse("45", "(\"\" 44) (\"\" 45)");
  expectparse("quoted", "(\"\" something) (\"\" \"quo\" ted)");

#undef expectparse
  close_FildeshO(oslice);
  close_FildeshO(info->err_out);
  close_FildeshSxpb(sxpb);
}

static void parse_array_of_empty_messages_test() {
  FildeshX in = FildeshX_of_strlit("(a (()) () ())");
  FildeshO err_out[1] = {DEFAULT_FildeshO};
  FildeshSxpb* sxpb = slurp_sxpb_close_FildeshX(&in, NULL, err_out);
  FildeshSxpbIT it;

  assert(sxpb);
  assert(err_out->size == 0);
  it = lookup_subfield_at_FildeshSxpb(
      sxpb, top_of_FildeshSxpb(sxpb), "a");
  assert(it.field_kind == FildeshSxprotoFieldKind_ARRAY);

  it = first_at_FildeshSxpb(sxpb, it);
  assert(it.field_kind == FildeshSxprotoFieldKind_MESSAGE);
  assert(nullish_FildeshSxpbIT(first_at_FildeshSxpb(sxpb, it)));

  it = next_at_FildeshSxpb(sxpb, it);
  assert(it.field_kind == FildeshSxprotoFieldKind_MESSAGE);
  assert(nullish_FildeshSxpbIT(first_at_FildeshSxpb(sxpb, it)));
  assert(nullish_FildeshSxpbIT(next_at_FildeshSxpb(sxpb, it)));

  close_FildeshSxpb(sxpb);
  close_FildeshO(err_out);
}

static void parse_repeated_list_field_test() {
  /* Repeating a manyof field is rejected (reject-all duplicate names). */
  {
    FildeshX in = FildeshX_of_strlit(
        "(m (()) (a 1))(m (()) (a 2))");
    FildeshO err_out[1] = {DEFAULT_FildeshO};
    FildeshSxpb* sxpb = slurp_sxpb_close_FildeshX(&in, NULL, err_out);
    assert(NULL == sxpb);
    assert(err_out->size > 0);
    close_FildeshO(err_out);
  }

  /* Repeating an array field is rejected as well. These message-element cases
   * used to append; they are now duplicate-field errors.
   */
  {
    FildeshX in = FildeshX_of_strlit(
        "(a (()) (() (x 1)))(a (()) (() (x 2)))");
    FildeshO err_out[1] = {DEFAULT_FildeshO};
    FildeshSxpb* sxpb = slurp_sxpb_close_FildeshX(&in, NULL, err_out);
    assert(NULL == sxpb);
    assert(err_out->size > 0);
    close_FildeshO(err_out);
  }
}

static void parse_append_operator_test() {
  /* Appending to an existing array via a top-level path. */
  {
    FildeshX in = FildeshX_of_strlit(
        "(m (a (()) 1 2 3))((+. m a) (()) 4 5 6)");
    FildeshO err_out[1] = {DEFAULT_FildeshO};
    FildeshSxpb* sxpb = slurp_sxpb_close_FildeshX(&in, NULL, err_out);
    FildeshSxpbIT it;
    unsigned i;
    assert(sxpb);
    assert(err_out->size == 0);
    it = lookup_subfield_at_FildeshSxpb(sxpb, top_of_FildeshSxpb(sxpb), "m");
    it = lookup_subfield_at_FildeshSxpb(sxpb, it, "a");
    assert(it.field_kind == FildeshSxprotoFieldKind_ARRAY);
    it = first_at_FildeshSxpb(sxpb, it);
    for (i = 1; i <= 6; ++i) {
      assert(i == unsigned_value_at_FildeshSxpb(sxpb, it));
      it = next_at_FildeshSxpb(sxpb, it);
    }
    assert(nullish_FildeshSxpbIT(it));
    close_FildeshSxpb(sxpb);
    close_FildeshO(err_out);
  }

  /* Quoted field names are valid append keypath segments. */
  {
    FildeshX in = FildeshX_of_strlit(
        "(\"m m\" (\"a a\" (()) 1))((+. \"m m\" \"a a\") (()) 2)");
    FildeshO err_out[1] = {DEFAULT_FildeshO};
    FildeshSxpb* sxpb = slurp_sxpb_close_FildeshX(&in, NULL, err_out);
    FildeshSxpbIT it;
    assert(sxpb);
    assert(err_out->size == 0);
    it = lookup_subfield_at_FildeshSxpb(
        sxpb, top_of_FildeshSxpb(sxpb), "m m");
    it = lookup_subfield_at_FildeshSxpb(sxpb, it, "a a");
    it = first_at_FildeshSxpb(sxpb, it);
    assert(1 == unsigned_value_at_FildeshSxpb(sxpb, it));
    it = next_at_FildeshSxpb(sxpb, it);
    assert(2 == unsigned_value_at_FildeshSxpb(sxpb, it));
    assert(nullish_FildeshSxpbIT(next_at_FildeshSxpb(sxpb, it)));
    close_FildeshSxpb(sxpb);
    close_FildeshO(err_out);
  }

  /* Appending no elements is a successful no-op. */
  {
    FildeshX in = FildeshX_of_strlit(
        "(a (()) 1 2)((+. a) (()))");
    FildeshO err_out[1] = {DEFAULT_FildeshO};
    FildeshSxpb* sxpb = slurp_sxpb_close_FildeshX(&in, NULL, err_out);
    FildeshSxpbIT it;
    assert(sxpb);
    it = lookup_subfield_at_FildeshSxpb(
        sxpb, top_of_FildeshSxpb(sxpb), "a");
    it = first_at_FildeshSxpb(sxpb, it);
    assert(1 == unsigned_value_at_FildeshSxpb(sxpb, it));
    it = next_at_FildeshSxpb(sxpb, it);
    assert(2 == unsigned_value_at_FildeshSxpb(sxpb, it));
    assert(nullish_FildeshSxpbIT(next_at_FildeshSxpb(sxpb, it)));
    close_FildeshSxpb(sxpb);
    close_FildeshO(err_out);
  }

  /* Reconstruct the widened type from every existing array element. */
  {
    FildeshX in = FildeshX_of_strlit(
        "(a (()) 1 2.0)((+. a) (()) 3 4.0)");
    FildeshO err_out[1] = {DEFAULT_FildeshO};
    FildeshSxpb* sxpb = slurp_sxpb_close_FildeshX(&in, NULL, err_out);
    FildeshSxpbIT it;
    assert(sxpb);
    it = lookup_subfield_at_FildeshSxpb(
        sxpb, top_of_FildeshSxpb(sxpb), "a");
    it = first_at_FildeshSxpb(sxpb, it);
    assert(it.field_kind == FildeshSxprotoFieldKind_LITERAL_INT);
    assert(1.0f == float_value_at_FildeshSxpb(sxpb, it));
    it = next_at_FildeshSxpb(sxpb, it);
    assert(it.field_kind == FildeshSxprotoFieldKind_LITERAL_FLOAT);
    assert(2.0f == float_value_at_FildeshSxpb(sxpb, it));
    it = next_at_FildeshSxpb(sxpb, it);
    assert(it.field_kind == FildeshSxprotoFieldKind_LITERAL_FLOAT);
    assert(3.0f == float_value_at_FildeshSxpb(sxpb, it));
    it = next_at_FildeshSxpb(sxpb, it);
    assert(it.field_kind == FildeshSxprotoFieldKind_LITERAL_FLOAT);
    assert(4.0f == float_value_at_FildeshSxpb(sxpb, it));
    assert(nullish_FildeshSxpbIT(next_at_FildeshSxpb(sxpb, it)));
    close_FildeshSxpb(sxpb);
    close_FildeshO(err_out);
  }

  /* A multi-segment path descends through nested messages. */
  {
    FildeshX in = FildeshX_of_strlit(
        "(o (m (a (()) 1)))((+. o m a) (()) 2)");
    FildeshO err_out[1] = {DEFAULT_FildeshO};
    FildeshSxpb* sxpb = slurp_sxpb_close_FildeshX(&in, NULL, err_out);
    FildeshSxpbIT it;
    assert(sxpb);
    it = lookup_subfield_at_FildeshSxpb(sxpb, top_of_FildeshSxpb(sxpb), "o");
    it = lookup_subfield_at_FildeshSxpb(sxpb, it, "m");
    it = lookup_subfield_at_FildeshSxpb(sxpb, it, "a");
    it = first_at_FildeshSxpb(sxpb, it);
    assert(1 == unsigned_value_at_FildeshSxpb(sxpb, it));
    it = next_at_FildeshSxpb(sxpb, it);
    assert(2 == unsigned_value_at_FildeshSxpb(sxpb, it));
    assert(nullish_FildeshSxpbIT(next_at_FildeshSxpb(sxpb, it)));
    close_FildeshSxpb(sxpb);
    close_FildeshO(err_out);
  }

  /* Named manyof entries do not constrain later anonymous elements. */
  {
    FildeshX in = FildeshX_of_strlit(
        "((m) (a 1) (b 2))((+. m) (()) (c 3) 4 5)");
    FildeshO err_out[1] = {DEFAULT_FildeshO};
    FildeshSxpb* sxpb = slurp_sxpb_close_FildeshX(&in, NULL, err_out);
    FildeshSxpbIT it;
    assert(sxpb);
    it = lookup_subfield_at_FildeshSxpb(sxpb, top_of_FildeshSxpb(sxpb), "m");
    assert(it.field_kind == FildeshSxprotoFieldKind_MANYOF);
    it = first_at_FildeshSxpb(sxpb, it);
    assert(0 == strcmp("a", name_at_FildeshSxpb(sxpb, it)));
    assert(1 == unsigned_value_at_FildeshSxpb(sxpb, it));
    it = next_at_FildeshSxpb(sxpb, it);
    assert(0 == strcmp("b", name_at_FildeshSxpb(sxpb, it)));
    assert(2 == unsigned_value_at_FildeshSxpb(sxpb, it));
    it = next_at_FildeshSxpb(sxpb, it);
    assert(0 == strcmp("c", name_at_FildeshSxpb(sxpb, it)));
    assert(3 == unsigned_value_at_FildeshSxpb(sxpb, it));
    it = next_at_FildeshSxpb(sxpb, it);
    assert(NULL == name_at_FildeshSxpb(sxpb, it));
    assert(4 == unsigned_value_at_FildeshSxpb(sxpb, it));
    it = next_at_FildeshSxpb(sxpb, it);
    assert(NULL == name_at_FildeshSxpb(sxpb, it));
    assert(5 == unsigned_value_at_FildeshSxpb(sxpb, it));
    assert(nullish_FildeshSxpbIT(next_at_FildeshSxpb(sxpb, it)));
    close_FildeshSxpb(sxpb);
    close_FildeshO(err_out);
  }

  /* Failures: unknown target, non-list target, missing (()) discriminator. */
#define expectappendfail(text) do { \
  FildeshX slice = FildeshX_of_strlit(text); \
  FildeshO eo[1] = {DEFAULT_FildeshO}; \
  FildeshSxpb* s = slurp_sxpb_close_FildeshX(&slice, NULL, eo); \
  assert(NULL == s); \
  assert(eo->size > 0); \
  close_FildeshO(eo); \
} while (0)
  expectappendfail("(m (a (()) 1))((+. m zzz) (()) 2)");
  expectappendfail("(m (x 5))((+. m x) (()) 1)");
  expectappendfail("(m (a (()) 1))((+. m a) 2)");
  expectappendfail("((m) 1)((+. m) (()) ())");
  expectappendfail("((m) ())((+. m) (()) 1)");
#undef expectappendfail
}

int main() {
  parse_string_test();
  parse_number_test();
  parse_number_failure_test();
  parse_name_test();
  parse_field_test();
  parse_field_name_validity_test();
  parse_string_field_test();
  parse_last_in_string_array_field_test();
  parse_array_of_empty_messages_test();
  parse_repeated_list_field_test();
  parse_append_operator_test();
  return 0;
}
