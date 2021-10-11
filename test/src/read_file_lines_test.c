/* EXPECTED_AS_FIRST_LINE */
#include "fildesh.h"
#include <assert.h>
#include <string.h>

int main() {
  char* line;
  FildeshX* in = open_FildeshXF("test/src/read_file_lines_test.c");

  assert(in);

  line = getline_FildeshX(in);
  assert(0 == strcmp("/* EXPECTED_AS_FIRST_LINE */", line));
  do {
    line = getline_FildeshX(in);
    assert(line && "have not reached the expected last line yet");
  } while (0 != strcmp("/* EXPECTED_AS_LAST_LINE */", line));
  line = getline_FildeshX(in);
  assert(!line && "no more lines expected");

  close_FildeshX(in);
  return 0;
}

/* EXPECTED_AS_LAST_LINE */
