#include "test/sxproto/print_test.h"

#include <assert.h>

  bool
check_same_printed_format(
    FildeshO* err_out,
    const char* name,
    const char* format,
    const unsigned char* expect,
    size_t expect_size,
    const unsigned char* actual,
    size_t actual_size)
{
  if (0 == fildesh_compare_bytestring(
          expect, expect_size, actual, actual_size)) {
    return true;
  }
  putstrlit_FildeshO(err_out, "Mismatch in ");
  putstr_FildeshO(err_out, format);
  putstrlit_FildeshO(err_out, " format for ");
  putstr_FildeshO(err_out, name);
  putstrlit_FildeshO(err_out, ".\nExpect: ");
  put_bytestring_FildeshO(err_out, expect, expect_size);
  putstrlit_FildeshO(err_out, "\nActual: ");
  put_bytestring_FildeshO(err_out, actual, actual_size);
  putc_FildeshO(err_out, '\n');
  return false;
}

  void
check_same_printed_format_test(void)
{
  static const char expect_error[] =
    "Mismatch in test format for case.\n"
    "Expect: expected\n"
    "Actual: actual\n"
    ;
  FildeshO err_out[1] = {DEFAULT_FildeshO};
  assert(check_same_printed_format(
      err_out, "case", "test",
      fildesh_bytestrlit("same"),
      fildesh_bytestrlit("same")));
  assert(err_out->size == 0);
  assert(!check_same_printed_format(
      err_out, "case", "test",
      fildesh_bytestrlit("expected"),
      fildesh_bytestrlit("actual")));
  assert(0 == fildesh_compare_bytestring(
      fildesh_bytestrlit(expect_error),
      bytestring_of_FildeshO(err_out)));
  close_FildeshO(err_out);
}
