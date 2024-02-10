#include <fildesh/fildesh.h>
#include "include/fildesh/fildesh_compat_errno.h"
#include "include/fildesh/fildesh_compat_file.h"
#include "include/fildesh/fildesh_compat_string.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>

int main() {
  const char* output_directory = getenv("TEST_TMPDIR");
  char* initial_filename;
  char* sibling_filename;
  FildeshX* in;
  FildeshO* out;
  FildeshX slice;

  assert(output_directory && "need a TEST_TMPDIR environment variable");
  initial_filename =
    fildesh_compat_file_catpath(
        output_directory, "sibling_file_test_initial.txt");
  fildesh_log_tracef("initial file is: %s", initial_filename);
  assert(0 == memcmp(output_directory, initial_filename,
                     strlen(output_directory)));

  /* Write out initial file.*/
  out = open_FildeshOF(initial_filename);
  assert(out);
  putstrlit_FildeshO(out, "i am the initial file\n");
  close_FildeshO(out);
  /* Read back initial file.*/
  in = open_FildeshXF(initial_filename);
  assert(in);
  slice = sliceline_FildeshX(in);
  assert(0 == fildesh_compare_bytestring(
          bytestring_of_FildeshX(&slice),
          fildesh_bytestrlit("i am the initial file")));
  close_FildeshX(in);

  /* Write out sibling file.*/
  out = open_sibling_FildeshOF(initial_filename, "sibling_file_test_sibling.txt");
  assert(out);
  sibling_filename = fildesh_compat_string_duplicate(filename_FildeshOF(out));
  putstrlit_FildeshO(out, "i am the sibling file\n");
  close_FildeshO(out);
  /* Assertions about sibling filename.*/
  assert(sibling_filename);
  fildesh_log_tracef("sibling file is: %s", sibling_filename);
  assert(0 == memcmp(output_directory, sibling_filename,
                     strlen(output_directory)));
  /* Read back sibling file.*/
  in = open_sibling_FildeshXF(initial_filename, "sibling_file_test_sibling.txt");
  assert(in);
  assert(0 == strcmp(sibling_filename, filename_FildeshXF(in)));
  slice = sliceline_FildeshX(in);
  assert(0 == fildesh_compare_bytestring(
          bytestring_of_FildeshX(&slice),
          fildesh_bytestrlit("i am the sibling file")));
  close_FildeshX(in);

  free(initial_filename);
  free(sibling_filename);

  return 0;
}
