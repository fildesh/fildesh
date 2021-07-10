#include "lace.h"
#include "lace_tool.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int
main_transpose(unsigned argc, char** argv);

LACE_TOOL_PIPEM_NULLARY_CALLBACK(run_transpose) {
  const char* argv[] = {"transpose", ",", NULL};
  int istat = main_transpose(2, (char**)argv);
  assert(istat == 0);
}

int main() {
  const char input_data[] =
    "This, is, the, first, line.\n"
    "This, is, the, next, line.\n"
    "This, is, the, third, line.\n"
    "This, is, completely, out, of, line.\n"
    "Whose, line, is, it?\n"
    ",   ,  \n"
    "Transpose, is, offline.\n";
  const size_t input_size = sizeof(input_data)-1;
  const char expect_data[] =
    " This,  This,  This,       This, Whose, , Transpose\n"
    "   is,    is,    is,         is,  line, ,        is\n"
    "  the,   the,   the, completely,    is, ,  offline.\n"
    "first,  next, third,        out,   it?, ,\n"
    "line., line., line.,         of,      , ,\n"
    "     ,      ,      ,      line.,      , ,\n";
  const size_t expect_size = sizeof(expect_data)-1;
  size_t output_size;
  char* output_data = NULL;

  output_size = lace_tool_pipem(
      input_size, input_data, 0,
      run_transpose, NULL,
      1, &output_data);
  fputs("Got:\n", stderr);
  fputs(output_data, stderr);
  assert(output_size == expect_size);
  assert(0 == memcmp(output_data, expect_data, expect_size));
  free(output_data);
  return 0;
}
