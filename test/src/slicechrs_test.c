
#include "lace.h"

#include <assert.h>
#include <string.h>

static
  void
slicechrs_easy_test()
{
  LaceX in[1] = { DEFAULT_LaceX };
  LaceX slice;
  char content[] = "(i.am some|content.)";
  const char delims[] = " ().|";

  in->buf.at = content;
  in->buf.sz = strlen(content);
  in->buf.alloc_lgsz = LACE_LGSIZE_MAX;

  slice = slicechrs_LaceX(in, delims);
  assert(0 == strcmp(slice.buf.at, ""));
  slice = slicechrs_LaceX(in, delims);
  assert(0 == strcmp(slice.buf.at, "i"));
  slice = slicechrs_LaceX(in, delims);
  assert(0 == strcmp(slice.buf.at, "am"));
  slice = slicechrs_LaceX(in, delims);
  assert(0 == strcmp(slice.buf.at, "some"));
  slice = slicechrs_LaceX(in, delims);
  assert(0 == strcmp(slice.buf.at, "content"));
  slice = slicechrs_LaceX(in, delims);
  assert(0 == strcmp(slice.buf.at, ""));
  slice = slicechrs_LaceX(in, delims);
  assert(!slice.buf.at);
}

int main() {
  slicechrs_easy_test();
  return 0;
}
