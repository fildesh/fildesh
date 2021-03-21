
#include <assert.h>
#include <stdlib.h>

  char*
lace_parse_int(int* ret, const char* in)
{
  long v;
  char* out = 0;

  assert(ret);
  assert(in);
  v = strtol(in, &out, 10);

  if (out == in)  out = 0;
  if (out) {
    *ret = (int) v;
    if (*ret != v)  out = 0;
  }
  return out;
}

