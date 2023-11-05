#define FILDESH_LOG_TRACE_ON
#include "src/sxproto/parse_sxpb.h"

#include <assert.h>
#include <string.h>

static void parse_string_test() {
  FildeshSxpbInfo info[1] = {DEFAULT_FildeshSxpbInfo};
  FildeshO oslice[1] = {DEFAULT_FildeshO};

#define expectparse(expect, text) do { \
  FildeshX slice = FildeshX_of_strlit(text); \
  bool good = parse_string_FildeshSxpbInfo(info, &slice, oslice); \
  assert(good); \
  putc_FildeshO(oslice, '\0'); \
  fildesh_log_trace(oslice->at); \
  oslice->size -= 1; \
  assert(strlen(expect) == oslice->size); \
  assert(0 == memcmp(expect, oslice->at, oslice->size)); \
} while (0)

  expectparse("abcdef", "\"abcdef\"");
  expectparse("ab\"cd", "\"ab\\\"cd\"");
  expectparse("ab\\", "\"ab\\\\\"");
  expectparse("a  \n b", "\"a  \\n b\"");

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

#undef expectparse
  close_FildeshO(info->err_out);
  close_FildeshO(oslice);
}

static void parse_name_test() {
  FildeshSxpbInfo info[1] = {DEFAULT_FildeshSxpbInfo};
  FildeshO oslice[1] = {DEFAULT_FildeshO};

#define expectparse(expect, expect_depth, text) do { \
  FildeshX slice = FildeshX_of_strlit(text); \
  unsigned nesting_depth = 0; \
  bool good = parse_name_FildeshSxpbInfo(info, &slice, oslice, &nesting_depth); \
  assert(good); \
  putc_FildeshO(oslice, '\0'); \
  fildesh_log_trace(oslice->at); \
  oslice->size -= 1; \
  assert(strlen(expect) == oslice->size); \
  assert(0 == memcmp(expect, oslice->at, oslice->size)); \
  assert(expect_depth == nesting_depth); \
} while (0)

  expectparse("x", 0, "x");
  expectparse("y", 1, "(y)");
  expectparse("z", 2, "((z))");
  expectparse("", 1, "()");
  expectparse("", 2, "(())");
  /* Quoted names.*/
  expectparse("abc", 0, "\"abc\"");
  expectparse("(a\"bc", 1, "( \"(a\\\"bc\")");

#undef expectparse
  close_FildeshO(oslice);
}

static void parse_field_test() {
  FildeshSxpbInfo info[1] = {DEFAULT_FildeshSxpbInfo};
  FildeshO oslice[1] = {DEFAULT_FildeshO};
  FildeshSxpb* sxpb = open_FildeshSxpb();
  FildeshSxpbIT p_it = top_of_FildeshSxpb(sxpb);
  info->err_out = open_FildeshOF("/dev/stderr");

#define tryparse(text) do { \
  FildeshX slice = FildeshX_of_strlit(text); \
  bool good = parse_field_FildeshSxpbInfo(info, NULL,  &slice, sxpb, p_it, oslice); \
  assert(good); \
  assert(info->line_count == 0); \
  assert(info->column_count == strlen(text)); \
  remove_at_FildeshSxpb(sxpb, first_at_FildeshSxpb(sxpb, top_of_FildeshSxpb(sxpb))); \
  info->column_count = 0; \
} while (0)

  tryparse("(x 5)");
  tryparse("(y \"hello\")");
  tryparse("(z 5.254)");
  tryparse("(m (x 5) (y 7) (z 12))");
  tryparse("(m (x 5) (\"y\" 7) (z 12))");
  tryparse("((a) 1 2 3 4 5)");
  tryparse("(((b)) 1 2 3 4 5)");
  tryparse("((a) (() (x 5)) (() (y 7)) () (() (z 12)))");

#undef tryparse
  close_FildeshO(oslice);
  close_FildeshO(info->err_out);
  close_FildeshSxpb(sxpb);
}

int main() {
  parse_string_test();
  parse_number_test();
  parse_name_test();
  parse_field_test();
  return 0;
}
