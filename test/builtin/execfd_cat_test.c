
#include <fildesh/fildesh.h>
#include "include/fildesh/fildesh_compat_fd.h"
#include "include/fildesh/fildesh_compat_file.h"
#include "fildesh_tool.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int fildesh_builtin_execfd_main(unsigned, char**, FildeshX**, FildeshO**);

FILDESH_TOOL_PIPEM_CALLBACK(run_execfd, in_fd, out_fd, const char**, execfd_argv) {
  int istat;
  char arg_fd_buf[FILDESH_INT_BASE10_SIZE_MAX];
  char stdout_buf[FILDESH_FD_PATH_SIZE_MAX];

  fildesh_encode_int_base10(arg_fd_buf, in_fd);
  execfd_argv[13] = arg_fd_buf;

  fildesh_encode_fd_path(stdout_buf, out_fd);
  execfd_argv[4] = stdout_buf;

  istat = fildesh_builtin_execfd_main(15, (char**)execfd_argv, NULL, NULL);
  assert(istat == 0);
}

int main(int argc, char** argv) {
  const char* execfd_argv[] = {
    "execfd",
    "-stdin", "/dev/null",
    "-stdout", "replace_with_stdout_file",
    "a_a_a+a+a+a_x+a",
    "--",
    "replace_with_shout_exe",
    "-",
    "a", "b", "c", "d",
    "replace_with_fd", "world",
    NULL,
  };
  const char expect_data[] = "abcd helloworld\n";
  const size_t expect_size = strlen(expect_data);
  char* output_data = NULL;
  size_t output_size;

  assert(argc == 2);
  execfd_argv[7] = argv[1];  /* replace_with_shout_exe */

  output_size = fildesh_tool_pipem(
      strlen("hello"), "hello",
      run_execfd, execfd_argv,
      &output_data);

  assert(output_size > 0);
  fildesh_log_trace(output_data);
  assert(output_size == expect_size);
  assert(0 == memcmp(output_data, expect_data, expect_size));

  free(output_data);
  return 0;
}

