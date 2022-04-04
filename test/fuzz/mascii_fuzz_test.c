#include "fuzz_common.h"
#include <assert.h>
#include <string.h>

#include "fildesh.h"
#include "src/mascii.h"

  int
LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
  FildeshMascii mascii;
  size_t i;
  const char* const needle = (const char*)data;
  const char* haystack = NULL;
  size_t needle_size = 0;
  size_t haystack_size = 0;
  bool spanning = false;
  size_t found_index;

  for (i = 0; i < size; ++i) {
    if (data[i] >= 128) {
      needle_size = i;
      haystack = (const char*) &data[i+1];
      haystack_size = size - i - 1;
      spanning = (data[i] % 2 == 1);
      break;
    }
  }
  if (spanning) {
    mascii = charnot_FildeshMascii(needle, needle_size);
  }
  else {
    mascii = charset_FildeshMascii(needle, needle_size);
  }

  found_index = find_FildeshMascii(&mascii, haystack, haystack_size);
  assert(found_index <= haystack_size);

  for (i = 0; i < found_index; ++i) {
    if ((unsigned char) haystack[i] < 128) {
      void* match = memchr(needle, haystack[i], needle_size);
      assert(!spanning || match);
      assert(spanning || !match);
    }
  }

  if (found_index < haystack_size) {
    void* match = memchr(needle, haystack[found_index], needle_size);
    assert(!spanning || !match);
    assert(spanning || match);
  }
  return 0;
}

