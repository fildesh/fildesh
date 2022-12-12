#include <fildesh/ifstream.hh>
#include <cassert>
#include <string>

static void getline_test(const std::string& filename) {
  fildesh::ifstream in(filename);
  std::string line;
  std::getline(in, line, '\n');
  assert(!in.fail());
  assert(line == "hello" || line == "hello\r");
}

static void parse_int_test(const std::string& filename) {
  fildesh::ifstream in(filename);
  int x = -1;
  in >> x;
  assert(!in.fail());
  assert(x == 127);
}

static void putback_test(const std::string& filename) {
  fildesh::ifstream in(filename);
  char c = '\0';

  in.get(c);
  assert(c == 'h');

  in.unget();
  in.get(c);
  assert(c == 'h');

  in.putback('j');
  in.get(c);
  assert(c == 'j');

  in.get(c);
  assert(c == 'e');
  in >> c;
  assert(c == 'l');
}


int main(int argc, char** argv) {
  assert(argc == 3);
  const std::string literal_hello_filename = argv[1];
  const std::string literal_127_filename = argv[2];

  getline_test(literal_hello_filename);
  parse_int_test(literal_127_filename);
  putback_test(literal_hello_filename);

  return 0;
}

