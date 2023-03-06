#include "src/builtin/fildesh_builtin.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "include/fildesh_tool.h"

FILDESH_TOOL_PIPEM_NULLARY_CALLBACK(run_oargz, in_fd, out_fd) {
  const char* argv[] = {"oargz", "--", "first", "second", "third", NULL};
  FildeshX* inputv[] = {NULL, NULL, NULL, NULL, NULL};
  FildeshO* outputv[] = {NULL, NULL, NULL, NULL, NULL};
  int istat;
  inputv[0] = open_fd_FildeshX(in_fd);
  outputv[0] = open_fd_FildeshO(out_fd);
  istat = fildesh_builtin_oargz_main(5, (char**)argv, inputv, outputv);
  assert(istat == 0);
  close_FildeshX(inputv[0]);
}

int main() {
  const char expect_data[] = "first\0second\0third";
  const size_t expect_size = sizeof(expect_data)-1;
  char* output_data = NULL;
  size_t output_size = fildesh_tool_pipem(
      0, NULL,
      run_oargz, NULL,
      &output_data);
  assert(output_size == expect_size);
  assert(0 == memcmp(output_data, expect_data, output_size));
  free(output_data);
  return 0;
}
