
#include "fildesh.h"

#include <assert.h>
#include <string.h>


static
  void
slicestr_long_delim_no_match_test()
{
  FildeshX in[1] = {DEFAULT_FildeshX};
  FildeshX slice;
  size_t i;
  char content[] = "iamsomecontent";
  const size_t content_length = strlen(content);
  const char delim[] = "iamaverylongdelimiter";
  const size_t delim_length = strlen(delim);

  in->at = content;
  in->size = content_length;

  /* Run the test with various offsets.*/
  for (i = 0; i < content_length; ++i) {
    in->off = i;
    slice = until_bytestring_FildeshX(in, (unsigned char*)delim, delim_length);
    assert(in->off == content_length);
    assert(slice.at);
    assert(slice.at == &in->at[i]);
    assert(slice.size == content_length - i);
    skipstr_FildeshX(in, delim);
  }
  in->off = content_length;
  slice = until_bytestring_FildeshX(in, (unsigned char*)delim, delim_length);
  assert(!slice.at);
  assert(slice.size == 0);
}


int main() {
  slicestr_long_delim_no_match_test();
  return 0;
}
