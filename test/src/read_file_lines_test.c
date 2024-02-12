/* EXPECTED_AS_FIRST_LINE */
#include <fildesh/fildesh.h>
#include <assert.h>
#include <string.h>

int main() {
  FildeshX* in = open_FildeshXF("test/src/read_file_lines_test.c");
  FildeshX slice;

  assert(in);

  slice = sliceline_FildeshX(in);
  assert(0 == fildesh_compare_bytestring(
          bytestring_of_FildeshX(&slice),
          fildesh_bytestrlit("/* EXPECTED_AS_FIRST_LINE */")));
  do {
    slice = sliceline_FildeshX(in);
    assert(slice.at && "have not reached the expected last line yet");
  } while (0 != fildesh_compare_bytestring(
          bytestring_of_FildeshX(&slice),
          fildesh_bytestrlit("/* EXPECTED_AS_LAST_LINE */")));
  slice = sliceline_FildeshX(in);
  assert(!slice.at && "no more lines expected");

  close_FildeshX(in);
  return 0;
}

/* EXPECTED_AS_LAST_LINE */
