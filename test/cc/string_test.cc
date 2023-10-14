#include <fildesh/string.hh>

#include <cassert>

static void append_assign_test() {
  FildeshO oslice[1] = {DEFAULT_FildeshO};
  FildeshX in[1] = {LITERAL_FildeshX("alpha beta gamma")};
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
}

int main() {
  append_assign_test();
  return 0;
}

