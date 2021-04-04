
#include "lace.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

static
  void
slicestr_long_delim_no_match_test()
{
  LaceX in[1] = { DEFAULT_LaceX };
  LaceX slice;
  size_t i;
  char content[] = "iamsomecontent";
  const size_t content_length = strlen(content);
  const char delim[] = "iamaverylongdelimiter";

  in->buf.at = content;
  in->buf.sz = strlen(content);
  in->buf.alloc_lgsz = LACE_LGSIZE_MAX;

  /* Run the test with various offsets.*/
  for (i = 0; i < content_length; ++i) {
    in->off = i;
    slice = slicestr_LaceX(in, delim);
    assert(in->off == content_length);
    assert(slice.buf.at);
    assert(slice.buf.at == &in->buf.at[i]);
    assert(slice.buf.sz == content_length - i);
  }
  in->off = content_length;
  slice = slicestr_LaceX(in, delim);
  assert(!slice.buf.at);
  assert(slice.buf.sz == 0);
}


int main() {
  slicestr_long_delim_no_match_test();
  return 0;
}
