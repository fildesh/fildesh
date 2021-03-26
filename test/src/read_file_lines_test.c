/* EXPECTED_AS_FIRST_LINE */
#include "lace.h"
#include <assert.h>
#include <string.h>

int main() {
  LaceXF xf[] = {DEFAULT_LaceXF};
  LaceX* in = &xf->base;
  char* line;
  if (!open_LaceXF(xf, "test/src/read_file_lines_test.c")) {
    assert(NULL && "tried to open file");
  }

  line = getline_LaceX(in);
  assert(0 == strcmp("/* EXPECTED_AS_FIRST_LINE */", line));
  do {
    line = getline_LaceX(in);
    assert(line && "have not reached the expected last line yet");
  } while (0 != strcmp("/* EXPECTED_AS_LAST_LINE */", line));
  line = getline_LaceX(in);
  assert(!line && "no more lines expected");

  close_LaceX(in);
  return 0;
}

/* EXPECTED_AS_LAST_LINE */
