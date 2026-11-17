#define FILDESH_LOG_TRACE_ON
#include "src/sxproto/parse_sxpb.h"

#include <assert.h>
#include <string.h>

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

static void parse_field_name_test() {
  FildeshO err_out[1] = {DEFAULT_FildeshO};
#define expectpass(name) do { \
  FildeshX slice = FildeshX_of_strlit("(" name " 1)"); \
  FildeshSxpb* s = slurp_sxpb_close_FildeshX(&slice, NULL, err_out); \
  FildeshSxpbIT it; \
  assert(s); \
  assert(err_out->size == 0); \
  it = lookup_subfield_at_FildeshSxpb(s, top_of_FildeshSxpb(s), name); \
  assert(!nullish_FildeshSxpbIT(it)); \
  close_FildeshSxpb(s); \
} while (0)
  expectpass("abc");
  expectpass("-");
  expectpass(".");
  expectpass("-foo");
  expectpass(".word");
  expectpass("--+");
  expectpass("..-");
  expectpass("_");
#undef expectpass
  close_FildeshO(err_out);
}

static void parse_quoted_field_name_test() {
  FildeshO err_out[1] = {DEFAULT_FildeshO};
  FildeshX in[1];
  FildeshSxpb* sxpb;
  FildeshSxpbIT it;

  *in = FildeshX_of_strlit("(\"50mm\" 1)");
  sxpb = slurp_sxpb_close_FildeshX(in, NULL, err_out);
  assert(sxpb);
  it = lookup_subfield_at_FildeshSxpb(sxpb, top_of_FildeshSxpb(sxpb), "50mm");
  assert(!nullish_FildeshSxpbIT(it));
  close_FildeshSxpb(sxpb);

  close_FildeshO(err_out);
}

static void parse_field_name_failure_test() {
  FildeshO err_out[1] = {DEFAULT_FildeshO};
#define expectfail(name) do { \
  FildeshX slice = FildeshX_of_strlit("(" name " 1)"); \
  FildeshSxpb* s = slurp_sxpb_close_FildeshX(&slice, NULL, err_out); \
  assert(s == NULL); \
  assert(err_out->size > 0); \
  truncate_FildeshO(err_out); \
} while (0)
  expectfail("+.5");
  expectfail("-.5");
  expectfail("-+x");
  expectfail(".-x");
  expectfail(".+x");
  expectfail(".1");
  expectfail("+1");
  expectfail("+true");
  expectfail("50mm");
  expectfail("123");
  expectfail("+truex");
  expectfail("(50mm)");
  expectfail("( ; comment\n +truex)");
#undef expectfail
  close_FildeshO(err_out);
}

static void parse_subnest_name_test() {
  FildeshO err_out[1] = {DEFAULT_FildeshO};
  FildeshX in = FildeshX_of_strlit(
      "(n (\"\") (50mm x) (123 y) (-1 z) (.1 q) (\"+.\" r))");
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
  assert(0 == strcmp("-1", name_at_FildeshSxpb(sxpb, it)));
  it = next_at_FildeshSxpb(sxpb, it);
  assert(0 == strcmp(".1", name_at_FildeshSxpb(sxpb, it)));
  it = next_at_FildeshSxpb(sxpb, it);
  assert(0 == strcmp("+.", name_at_FildeshSxpb(sxpb, it)));
  close_FildeshSxpb(sxpb);
  close_FildeshO(err_out);
}

static void parse_subnest_name_failure_test() {
  FildeshO err_out[1] = {DEFAULT_FildeshO};
#define expectfail(name) do { \
  FildeshX slice = FildeshX_of_strlit("(my_nest (\"\") (" name " content)"); \
  FildeshSxpb* s = slurp_sxpb_close_FildeshX(&slice, NULL, err_out); \
  assert(s == NULL); \
  assert(err_out->size > 0); \
  truncate_FildeshO(err_out); \
} while (0)
  expectfail("+truex");
  expectfail("+true");
  expectfail("-.x");
  expectfail("-+x");
  expectfail(".-x");
  expectfail(".+x");
#undef expectfail
  close_FildeshO(err_out);
}

static void parse_append_name_failure_test() {
  FildeshO err_out[1] = {DEFAULT_FildeshO};
#define expectfail(text) do { \
  FildeshX slice = FildeshX_of_strlit(text); \
  FildeshSxpb* s = slurp_sxpb_close_FildeshX(&slice, NULL, err_out); \
  assert(s == NULL); \
  assert(err_out->size > 0); \
  truncate_FildeshO(err_out); \
} while (0)
  expectfail("(\"50mm\" (()) 1)((+. 50mm) (()) 2)");
  expectfail("(\"+true\" (()) 1)((+. +true) (()) 2)");
#undef expectfail
  close_FildeshO(err_out);
}

int main() {
  parse_name_test();
  parse_field_name_test();
  parse_quoted_field_name_test();
  parse_field_name_failure_test();
  parse_subnest_name_test();
  parse_subnest_name_failure_test();
  parse_append_name_failure_test();
  return 0;
}
