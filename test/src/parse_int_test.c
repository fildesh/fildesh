
#include <fildesh/fildesh.h>

#include <assert.h>
#include <string.h>


static
  void
parse_int_easy_test()
{
  int got = 0;
  bool good;
  FildeshX in[1] = {LITERAL_FildeshX("5 -10\n 0  98654  3216 +200")};

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

int main() {
  parse_int_easy_test();
  return 0;
}
