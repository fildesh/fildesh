#include <fildesh/fildesh.h>
#include <assert.h>

static void encode_int_test() {
  char buf[FILDESH_INT_BASE10_SIZE_MAX];
  unsigned n;
  int x;
  n = fildesh_encode_int_base10(buf, INT_MIN);
  assert(n < sizeof(buf));
  assert(fildesh_parse_int(&x, buf));
  assert(x == INT_MIN);
  n = fildesh_encode_int_base10(buf, INT_MAX);
  assert(n < sizeof(buf));
  assert(fildesh_parse_int(&x, buf));
  assert(x == INT_MAX);
}

static void encode_size_test() {
  char buf[FILDESH_SIZE_BASE10_SIZE_MAX];
  unsigned u, n;
  n = fildesh_encode_size_base10(buf, SIZE_MAX);
  assert(n < sizeof(buf));
  assert(n > 0);
  n = fildesh_encode_size_base10(buf, 0);
  assert(n == 1);
  assert(fildesh_parse_unsigned(&u, buf));
  assert(u == 0);
  n = fildesh_encode_size_base10(buf, UINT_MAX);
  assert(n < sizeof(buf));
  assert(fildesh_parse_unsigned(&u, buf));
  assert(u == UINT_MAX);
}

int main() {
  encode_int_test();
  encode_size_test();
  return 0;
}
