
#include "lace.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>


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
  if (!in) {return NULL;}
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
  if (!in) {return NULL;}
  v = strtod(in, &out);

  if (out == in)  out = NULL;
  if (out)  *ret = (double) v;
  return out;
}

  unsigned
lace_encode_int_base10(char* buf, int q)
{
  unsigned i, n;

  if (q == 0) {
    buf[0] = '0';
    buf[1] = '\0';
    return 1;
  } else if (q < 0) {
    q = -q;
    buf[0] = '-';
  } else {
    buf[0] = '\0';
  }

  i = LACE_INT_BASE10_SIZE_MAX-1;
  while (q > 0) {
    buf[--i] = '0' + (char)(q % 10);
    q /= 10;
  }

  if (buf[0] == '-') {
    buf[--i] = '-';
  }
  n = LACE_INT_BASE10_SIZE_MAX-1-i;
  memmove(buf, &buf[i], n);
  buf[n] = '\0';
  return n;
}

  unsigned
lace_encode_fd_path(char* buf, lace_fd_t fd)
{
  static const char prefix[] = "/dev/fd/";
  unsigned n = strlen(prefix);
  assert(buf);
  assert(fd >= 0);
  memcpy(buf, "/dev/fd/", n);
  return n + lace_encode_int_base10(&buf[n], fd);
}
