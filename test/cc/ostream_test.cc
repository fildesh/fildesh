#include <fildesh/ostream.hh>

#include <cassert>

static void c_struct_test() {
  FildeshO o[1] = {DEFAULT_FildeshO};
  fildesh::ostream out(o);
  assert(out.c_struct() == o);

  const fildesh::ostream& const_out = out;
  assert(const_out.c_struct() == o);
}

static void dev_null_test() {
  fildesh::ofstream out;
  assert(!out);

  out.open("/dev/null");
  for (unsigned i = 0; i < 10; ++i) {
    out << i;
  }
  assert(out);
  out.setstate(std::ios::badbit);
  assert(!out);

  // Reopen with implicit out.close().
  out.open("/dev/null");
  assert(out);
  out.setstate(std::ios::badbit);
  assert(!out);

  // Reopen with explicit out.close().
  out.close();
  assert(!out);
  out.open("/dev/null");
  assert(out);

  // Close before going out of scope to test that no double deletion occurs.
  out.close();
}

static void print_ints_test() {
  FildeshO content_oslice[1] = {DEFAULT_FildeshO};
  fildesh::ostream out(content_oslice);

  size_t n = 0;
  for (unsigned i = 0; i < 1000; ++i) {
    out << i << '\n';
    n += (i < 10 ? 1 : (i < 100 ? 2 : 3));  // Digits.
    n += 1;  // Newline.
    assert(content_oslice->size == n);
  }

  out.flush();
  assert(content_oslice->size == n);
}

int main() {
  c_struct_test();
  dev_null_test();
  print_ints_test();
  return 0;
}

