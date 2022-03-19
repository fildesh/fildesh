#include "fildesh.h"
#include "fildesh_builtin.h"
#include "fildesh_tool.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int
fildesh_builtin_transpose_main(unsigned argc, char** argv,
                            FildeshX** inputs, FildeshO** outputs);

int
main_transpose(unsigned argc, char** argv);

FILDESH_TOOL_PIPEM_NULLARY_CALLBACK(run_transpose, in_fd, out_fd) {
  const char* argv[] = {"transpose", "-d", ",", "-blank", NULL};
  FildeshX* in = open_fd_FildeshX(in_fd);
  FildeshO* out = open_fd_FildeshO(out_fd);
  int istat = fildesh_builtin_transpose_main(4, (char**)argv, &in, &out);
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

  output_size = fildesh_tool_pipem(
      input_size, input_data,
      run_transpose, NULL,
      &output_data);
  fputs("Got:\n", stderr);
  fputs(output_data, stderr);
  assert(output_size == expect_size);
  assert(0 == memcmp(output_data, expect_data, expect_size));
  free(output_data);
  return 0;
}
