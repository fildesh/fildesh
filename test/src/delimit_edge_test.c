
#include <fildesh/fildesh.h>

#include <assert.h>
#include <string.h>


static
  void
slicestr_long_delim_no_match_test()
{
  DECLARE_STRLIT_FildeshX(in, "iamsomecontent");
  FildeshX slice;
  size_t i;
  const size_t content_length = in->size;
  const char delim[] = "iamaverylongdelimiter";
  const size_t delim_length = strlen(delim);

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
