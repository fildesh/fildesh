#include <fildesh/istream.hh>

#include <cassert>
#include <string>

static void c_struct_test() {
  FildeshX x[1] = {DEFAULT_FildeshX};
  fildesh::istream in(x);
  assert(in.c_struct() == x);

  const fildesh::istream& const_in = in;
  assert(const_in.c_struct() == x);
}

static void dev_null_test() {
  fildesh::ifstream in;
  assert(!in);

  in.open("/dev/null");
  assert(in);
  std::streambuf::int_type c = in.get();
  assert(c == std::streambuf::traits_type::eof());
  assert(!in);

  // Reopen with implicit in.close().
  in.open("/dev/null");
  assert(in);

  // Reopen with explicit in.close().
  in.close();
  assert(!in);
  in.open("/dev/null");
  assert(in);

  // Close before going out of scope to test that no double deletion occurs.
  in.close();
}

static void getline_test() {
  FildeshX content_in[1] = {FildeshX_of_strlit("hello\nworld")};
  fildesh::istream in(content_in);

  std::string line;
  std::getline(in, line, '\n');
  assert(in);
  assert(line == "hello");
  std::getline(in, line, '\n');
  assert(in);
  assert(line == "world");
  std::getline(in, line, '\n');
  assert(!in);
}

static void parse_int_test() {
  FildeshX content_in[1] = {FildeshX_of_strlit("127")};
  fildesh::istream in(content_in);
  int x = -1;
  in >> x;
  assert(in);
  assert(x == 127);
  in >> x;
  assert(!in);
}

static void putback_test() {
  FildeshX* content_in = open_FildeshXA();
  memcpy(grow_FildeshX(content_in, 5), "hello", 5);
  fildesh::istream in(content_in);

  assert(in.rdbuf()->in_avail() == 5);
  assert(in);
  in.putback('z');
  assert(!in);
  in.clear();
  assert(in.rdbuf()->in_avail() == 5);

  char c = '\0';
  in.get(c);
  assert(c == 'h');

  in.unget();
  assert(in.peek() == 'h');
  in.get(c);

  in.putback('j');
  in.get(c);
  assert(c == 'j');

  in.get(c);
  assert(c == 'e');
  in >> c;
  assert(c == 'l');

  char tmp_s[10];
  in.read(tmp_s, sizeof(tmp_s));
  assert(in.gcount() == 2);
  assert(tmp_s[0] == 'l');
  assert(tmp_s[1] == 'o');
  assert(!in);
}

int main() {
  c_struct_test();
  dev_null_test();
  getline_test();
  parse_int_test();
  putback_test();

  return 0;
}

