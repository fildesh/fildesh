#include <fildesh/fildesh.h>

#include <assert.h>
#include <string.h>

static
  void
skipstr_easy_test()
{
  DECLARE_STRLIT_FildeshX(in, "please skip me brooo");
  bool skipped;

  skipped = skipstr_FildeshX(in, "just");
  assert(!skipped);
  skipped = skipstr_FildeshX(in, "please ");
  assert(skipped);

  skipped = skipchrs_FildeshX(in, "s");
  assert(skipped);
  skipped = skipchrs_FildeshX(in, "hijlmnop");
  assert(!skipped);
  skipped = skipchrs_FildeshX(in, "k");
  assert(skipped);
  skipped = skipstr_FildeshX(in, "ip");
  assert(skipped);

  skipped = skipstr_FildeshX(in, "me");
  assert(!skipped);
  skipped = skipstr_FildeshX(in, " me ");
  assert(skipped);

  skipped = skipstr_FildeshX(in, "broooo");
  assert(!skipped);
  skipped = skipstr_FildeshX(in, "brooo");
  assert(skipped);

  skipped = skipstr_FildeshX(in, "any more?");
  assert(!skipped);
  skipped = skipstr_FildeshX(in, "");
  assert(skipped);
}

int main() {
  skipstr_easy_test();
  return 0;
}
