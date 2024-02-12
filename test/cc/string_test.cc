#include <fildesh/string.hh>

#include <cassert>

static void append_assign_test() {
  FildeshO oslice[1] = {DEFAULT_FildeshO};
  FildeshX in[1] = {FildeshX_of_strlit("alpha beta gamma")};
  std::string s = fildesh::make_string(*in);
  assert(0 == s.compare("alpha beta gamma"));

  skipstr_FildeshX(in, "alpha");
  s.append(fildesh::make_string(*in));
  assert(0 == s.compare("alpha beta gamma beta gamma"));

  skipstr_FildeshX(in, " ");
  s = fildesh::make_string(*in);
  assert(0 == s.compare("beta gamma"));

  *oslice << until_char_FildeshX(in, ' ');
  s.assign(fildesh::make_string(*oslice));
  assert(0 == s.compare("beta"));

#ifdef __cpp_lib_string_view
  skipstr_FildeshX(in, " ");
  *oslice << fildesh::make_string_view(until_char_FildeshX(in, 'm'));
  s.assign(fildesh::make_string_view(*oslice));
  assert(0 == s.compare("betaga"));
#endif  // defined(__cpp_lib_string_view)
  close_FildeshO(oslice);

  s = "text that should be cleared";
  fildesh::slurp_file_to_string(s, "/dev/null");
}

static void ostringstream_test() {
  fildesh::ostringstream oss;
  assert(0 == strcmp(oss.c_str(), ""));

  oss << '1' << "2" << 3;
  assert(0 == strcmp(oss.c_str(), "123"));
  assert(oss.str() == "123");
#ifdef __cpp_lib_string_view
  assert(oss.view() == "123");
#endif  // defined(__cpp_lib_string_view)

  oss.truncate();
  oss << '4' << "5" << 6;
  assert(oss.str() == "456");
}

static void sibling_pathname_test() {
  std::string s;
  s = fildesh::sibling_pathname(NULL, "path/to/b.txt");
  assert(s == "path/to/b.txt");
  s = fildesh::sibling_pathname("path/to/", "b.txt");
  assert(s == "path/to/b.txt");
  s = fildesh::sibling_pathname("path/to/a.txt", "b.txt");
  assert(s == "path/to/b.txt");
  s = fildesh::sibling_pathname("path/to/a.txt", "./b.txt");
  assert(s == "path/to/b.txt");

  s = fildesh::sibling_pathname("path/to", "b.txt");
  assert(s == "path/b.txt");
  s = fildesh::sibling_pathname("a.txt", "b.txt");
  assert(s == "b.txt");
  s = fildesh::sibling_pathname("path/to/a.txt", "/tmp/b.txt");
  assert(s == "/tmp/b.txt");
  s = fildesh::sibling_pathname("path/to/a.txt", "-");
  assert(s == "-");

  fildesh::ostringstream oss;
  oss << "path/to/a.txt";
  oss.c_struct()->off += 2;
  sibling_pathname_bytestring_FildeshO(
      oss.c_struct(), (const unsigned char*)"b.txt", 5);
  assert(oss.str() == "th/to/b.txt");
}

int main() {
  append_assign_test();
  ostringstream_test();
  sibling_pathname_test();
  return 0;
}

