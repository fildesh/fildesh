#define FILDESH_LOG_TRACE_ON
#include "src/sxproto/parse_sxpb.h"

#include <assert.h>
#include <string.h>


#define XPT(expect, text)  do { \
  DECLARE_STRLIT_FildeshX(in, text); \
  FildeshSxpb* sxpb; \
  sxpb = slurp_sxpb_close_FildeshX(in, NULL, oslice); \
  assert(!sxpb); \
  assert(oslice->size > 0); \
  oslice->at[oslice->size-1] = '\0'; \
  fildesh_log_trace(oslice->at); \
  assert(oslice->size >= sizeof(expect)); \
  assert(0 == strcmp(expect, &oslice->at[oslice->size - sizeof(expect)])); \
  truncate_FildeshO(oslice); \
} while (0)

static
  void
parse_failure_test()
{
  FildeshO oslice[1] = {DEFAULT_FildeshO};

  XPT("Unknown escape sequence. Only very basic ones are supported.",
      "(\"a\" (\"b\\.\" 5))");
  XPT("Expected subfield name to be quoted too.",
      "(\"a\" (b 5))");
  XPT("Expected 2 closing parens after manyof name; got 0.",
      "((((");
  XPT("Expected 2 closing parens after manyof name; got 1.",
      "(((u)");
  XPT("Expected closing paren after array name.",
      "((");
  XPT("Denote empty message in array as (), not (()).",
      "((a) (()))");
  XPT("Message expects named fields inside it.",
      "(");
  XPT("Literal field can only hold 1 value.",
      "(k 1 2)");
  XPT("Expected closing double quote.",
      "(k \"");
  XPT("Expected a literal or closing paren.",
      "((x)\0");
  XPT("Expected open paren to start field.",
      ")");
  XPT("Cannot parse exponent.",
      "(x 5e+bad)");
  XPT("Array cannot hold fields.",
      "((a) (x 5))");
  XPT("Manyof cannot hold nameless message values yet.",
      "(((a)) (() (x 5)))");
  XPT("Duplicate field name. Use array syntax for repeated fields.",
      "(x 5) (x 6)");
  XPT("Unexpected message.",
      "((a) 5 (() (a 1)))");
  XPT("Unexpected literal type.",
      "((a) 5 6 7 \"8\")");
  XPT("Unexpected literal type.",
      "((a) \"5\" \"6\" \"7\" 8)");

  close_FildeshO(oslice);
}

#undef XPT


int main() {
  parse_failure_test();
  return 0;
}
