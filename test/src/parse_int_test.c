
#include <fildesh/fildesh.h>

#include <assert.h>
#include <string.h>


static
  void
parse_int_easy_test()
{
  int got = 0;
  bool good;
  DECLARE_STRLIT_FildeshX(in, "5 -10\n 0  98654  3216 +200");

  good = parse_int_FildeshX(in, &got);
  assert(good);
  assert(5 == got);

  good = parse_int_FildeshX(in, &got);
  assert(good);
  assert(-10 == got);

  good = parse_int_FildeshX(in, &got);
  assert(good);
  assert(0 == got);

  good = parse_int_FildeshX(in, &got);
  assert(good);
  assert(98654 == got);

  good = parse_int_FildeshX(in, &got);
  assert(good);
  assert(3216 == got);

  good = parse_int_FildeshX(in, &got);
  assert(good);
  assert(200 == got);

  good = parse_int_FildeshX(in, &got);
  assert(!good);
}

static
  void
parse_unsigned_test()
{
  unsigned got = 0;
  bool good;
  DECLARE_STRLIT_FildeshX(in, "0 1 65535 +200 -200");

  good = parse_unsigned_FildeshX(in, &got);
  assert(good);
  assert(0 == got);

  good = parse_unsigned_FildeshX(in, &got);
  assert(good);
  assert(1 == got);

  good = parse_unsigned_FildeshX(in, &got);
  assert(good);
  assert(65535 == got);

  good = parse_unsigned_FildeshX(in, &got);
  assert(good);
  assert(200 == got);

  good = parse_unsigned_FildeshX(in, &got);
  assert(!good);
}

int main() {
  parse_int_easy_test();
  parse_unsigned_test();
  return 0;
}
