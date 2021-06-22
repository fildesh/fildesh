
#include "lace.h"
#include <assert.h>
#include <stdlib.h>


typedef struct LaceXA LaceXA;
struct LaceXA { LaceX base; };
static void read_LaceXA(LaceXA* x) {(void)x;}
static void close_LaceXA(LaceXA* x) {(void)x;}
static void free_LaceXA(LaceXA* x) {free(x);}
DEFINE_LaceX_VTable(LaceXA, base);

LaceX* open_LaceXA() {
  LaceXA* x = (LaceXA*) malloc(sizeof(LaceXA));
  x->base = default_LaceX();
  x->base.vt = DEFAULT_LaceXA_LaceX_VTable;
  return &x->base;
}


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

