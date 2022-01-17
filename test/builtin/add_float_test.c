#include "fildesh.h"
#include "fildesh_compat_fd.h"
#include "fildesh_tool.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

int fildesh_builtin_add_main(int, char**, FildeshX**, FildeshO**);

FILDESH_TOOL_PIPEM_NULLARY_CALLBACK(run_add, in_fd, out_fd) {
  const char* const argv[] = {"add", NULL};
  FildeshX* in = open_fd_FildeshX(in_fd);
  FildeshO* out = open_fd_FildeshO(out_fd);
  int istat;
  istat = fildesh_builtin_add_main(1, (char**)argv, &in, &out);
  assert(istat == 0);
}

static void add_ints_test() {
  const char input_data[] =
    "1 1\n"
    "33 44\n"
    "1 2 3\n"
    "-1 2 -3\n"
    ;
  const size_t input_data_size = strlen(input_data);
  const char expect_data[] =
    "2\n"
    "77\n"
    "6\n"
    "-2\n"
    ;
  const size_t expect_size = strlen(expect_data);
  size_t output_size;
  char* output_data = NULL;

  output_size = fildesh_tool_pipem(
      input_data_size, input_data,
      run_add, NULL,
      &output_data);
  fprintf(stderr, "Got:\n%s", output_data);
  assert(output_size == expect_size);
  assert(0 == memcmp(output_data, expect_data, expect_size));
  free(output_data);
}

static void add_floats_test() {
  const char input_data[] =
    "1.5 1.5\n"
    "1.25 1.25\n"
    "0.2 0.55\n"
    ;
  const size_t input_data_size = strlen(input_data);
  const char expect_data[] =
    "3\n"
    "2.5\n"
    "0.75\n"
    ;
  const size_t expect_size = strlen(expect_data);
  size_t output_size;
  char* output_data = NULL;

  output_size = fildesh_tool_pipem(
      input_data_size, input_data,
      run_add, NULL,
      &output_data);
  fprintf(stderr, "Got:\n%s", output_data);
  assert(output_size == expect_size);
  assert(0 == memcmp(output_data, expect_data, expect_size));
  free(output_data);
}

int main() {
  add_ints_test();
  add_floats_test();
  return 0;
}
