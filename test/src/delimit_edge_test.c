
#include "fildesh.h"

#include <assert.h>
#include <string.h>


static
  void
slicestr_long_delim_no_match_test()
{
  FildeshX in[1] = { DEFAULT_FildeshX };
  FildeshX slice;
  size_t i;
  char content[] = "iamsomecontent";
  const size_t content_length = strlen(content);
  const char delim[] = "iamaverylongdelimiter";

  in->at = content;
  in->size = content_length;

  /* Run the test with various offsets.*/
  for (i = 0; i < content_length; ++i) {
    in->off = i;
    slice = slicestr_FildeshX(in, delim);
    assert(in->off == content_length);
    assert(slice.at);
    assert(slice.at == &in->at[i]);
    assert(slice.size == content_length - i);
  }
  in->off = content_length;
  slice = slicestr_FildeshX(in, delim);
  assert(!slice.at);
  assert(slice.size == 0);
}


int main() {
  slicestr_long_delim_no_match_test();
  return 0;
}
