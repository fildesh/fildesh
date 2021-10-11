#include "fildesh.h"
#include "lace_compat_errno.h"
#include "lace_compat_file.h"
#include "lace_compat_string.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>

int main() {
  const char* output_directory = getenv("TEST_TMPDIR");
  char* initial_filename;
  char* sibling_filename;
  FildeshX* in;
  FildeshO* out;
  const char* line;

  assert(output_directory && "need a TEST_TMPDIR environment variable");
  initial_filename =
    lace_compat_file_catpath(output_directory, "sibling_file_test_initial.txt");
  fildesh_log_tracef("initial file is: %s", initial_filename);
  assert(0 == memcmp(output_directory, initial_filename,
                     strlen(output_directory)));

  /* Write out initial file.*/
  out = open_FildeshOF(initial_filename);
  assert(out);
  puts_FildeshO(out, "i am the initial file\n");
  close_FildeshO(out);
  /* Read back initial file.*/
  in = open_FildeshXF(initial_filename);
  assert(in);
  line = getline_FildeshX(in);
  assert(line);
  assert(0 == strcmp(line, "i am the initial file"));
  close_FildeshX(in);

  /* Write out sibling file.*/
  out = open_sibling_FildeshOF(initial_filename, "sibling_file_test_sibling.txt");
  assert(out);
  sibling_filename = lace_compat_string_duplicate(filename_FildeshOF(out));
  puts_FildeshO(out, "i am the sibling file\n");
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
  line = getline_FildeshX(in);
  assert(line);
  assert(0 == strcmp(line, "i am the sibling file"));
  close_FildeshX(in);

  free(initial_filename);
  free(sibling_filename);

  return 0;
}
