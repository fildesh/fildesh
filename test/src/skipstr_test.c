#include "lace.h"

#include <assert.h>
#include <string.h>

static
  void
skipstr_easy_test()
{
  LaceX in[1] = { DEFAULT_LaceX };
  bool skipped;
  char content[] = "please skip me brooo";

  in->at = content;
  in->size = strlen(content);

  skipped = skipstr_LaceX(in, "just");
  assert(!skipped);
  skipped = skipstr_LaceX(in, "please ");
  assert(skipped);

  skipped = skipchrs_LaceX(in, "s");
  assert(skipped);
  skipped = skipchrs_LaceX(in, "hijlmnop");
  assert(!skipped);
  skipped = skipchrs_LaceX(in, "k");
  assert(skipped);
  skipped = skipstr_LaceX(in, "ip");
  assert(skipped);

  skipped = skipstr_LaceX(in, "me");
  assert(!skipped);
  skipped = skipstr_LaceX(in, " me ");
  assert(skipped);

  skipped = skipstr_LaceX(in, "broooo");
  assert(!skipped);
  skipped = skipstr_LaceX(in, "brooo");
  assert(skipped);

  skipped = skipstr_LaceX(in, "any more?");
  assert(!skipped);
  skipped = skipstr_LaceX(in, "");
  assert(skipped);
}

int main() {
  skipstr_easy_test();
  return 0;
}
