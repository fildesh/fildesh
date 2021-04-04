
#include "lace.h"
#include <assert.h>
#include <stdlib.h>

  char*
lace_parse_int(int* ret, const char* in)
{
  long v;
  char* out = NULL;

  assert(ret);
  assert(in);
  v = strtol(in, &out, 10);

  if (out == in)  out = NULL;
  if (out) {
    *ret = (int) v;
    if (*ret != v)  out = NULL;
  }
  return out;
}

  char*
lace_parse_double(double* ret, const char* in)
{
  double v;
  char* out = NULL;

  assert(ret);
  assert(in);
  v = strtod(in, &out);

  if (out == in)  out = NULL;
  if (out)  *ret = (double) v;
  return out;
}

