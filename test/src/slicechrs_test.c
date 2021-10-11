
#include "fildesh.h"

#include <assert.h>
#include <string.h>

static
  void
slicechrs_easy_test()
{
  FildeshX in[1] = { DEFAULT_FildeshX };
  FildeshX slice;
  char content[] = "(i.am some|content.)";
  const char delims[] = " ().|";

  in->at = content;
  in->size = strlen(content);

  slice = slicechrs_FildeshX(in, delims);
  assert(0 == strcmp(slice.at, ""));
  slice = slicechrs_FildeshX(in, delims);
  assert(0 == strcmp(slice.at, "i"));
  slice = slicechrs_FildeshX(in, delims);
  assert(0 == strcmp(slice.at, "am"));
  slice = slicechrs_FildeshX(in, delims);
  assert(0 == strcmp(slice.at, "some"));
  slice = slicechrs_FildeshX(in, delims);
  assert(0 == strcmp(slice.at, "content"));
  slice = slicechrs_FildeshX(in, delims);
  assert(0 == strcmp(slice.at, ""));
  slice = slicechrs_FildeshX(in, delims);
  assert(!slice.at);
}

int main() {
  slicechrs_easy_test();
  return 0;
}
