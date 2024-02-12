
#include <fildesh/fildesh.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>


typedef struct FildeshXA FildeshXA;
struct FildeshXA { FildeshX base; };
static void read_FildeshXA(FildeshXA* x) {(void)x;}
static void close_FildeshXA(FildeshXA* x) {(void)x;}
static void free_FildeshXA(FildeshXA* x) {free(x);}
DEFINE_FildeshX_VTable(FildeshXA, base);

FildeshX* open_FildeshXA() {
  FildeshXA* x = (FildeshXA*) malloc(sizeof(FildeshXA));
  if (!x) {return NULL;}
  x->base = default_FildeshX();
  x->base.vt = DEFAULT_FildeshXA_FildeshX_VTable;
  return &x->base;
}


  int
fildesh_compare_bytestring(
    const unsigned char* a, size_t m,
    const unsigned char* b, size_t n)
{
  return (
      m == n
      ? (n > 0 ? memcmp(a, b, n) : 0)
      : (m < n ? -1 : 1));
}

  char*
fildesh_parse_int(int* ret, const char* in)
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
fildesh_parse_unsigned(unsigned* ret, const char* in)
{
  unsigned long v;
  char* out = NULL;

  assert(ret);
  if (!in) {return NULL;}
  v = strtoul(in, &out, 10);

  if (out == in)  out = NULL;
  if (out) {
    *ret = (unsigned) v;
    if (*ret != v)  out = NULL;
  }
  return out;
}

  char*
fildesh_parse_double(double* ret, const char* in)
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
fildesh_encode_int_base10(char* buf, int q)
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

  i = FILDESH_INT_BASE10_SIZE_MAX-1;
  while (q > 0) {
    buf[--i] = '0' + (char)(q % 10);
    q /= 10;
  }

  if (buf[0] == '-') {
    buf[--i] = '-';
  }
  n = FILDESH_INT_BASE10_SIZE_MAX-1-i;
  memmove(buf, &buf[i], n);
  buf[n] = '\0';
  return n;
}

  unsigned
fildesh_encode_fd_path(char* buf, Fildesh_fd fd)
{
  static const char prefix[] = "/dev/fd/";
  unsigned n = strlen(prefix);
  assert(buf);
  assert(fd >= 0);
  memcpy(buf, "/dev/fd/", n);
  return n + fildesh_encode_int_base10(&buf[n], fd);
}
