#include <fildesh/fildesh.h>
#include "include/fildesh/fildesh_compat_fd.h"
#include "fildesh_tool.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>

typedef struct PipemCallbackArg PipemCallbackArg;
struct PipemCallbackArg {
  const char* fildesh_exe;
};

FILDESH_TOOL_PIPEM_CALLBACK(run_fildesh, in_fd, out_fd, PipemCallbackArg*, st) {
  fildesh_compat_fd_t fds_to_inherit[] = {-1, -1, -1};
  char in_fd_arg[FILDESH_FD_PATH_SIZE_MAX];
  char out_fd_arg[FILDESH_FD_PATH_SIZE_MAX];
  int istat;

  fds_to_inherit[0] = in_fd;
  fds_to_inherit[1] = out_fd;
  fildesh_encode_fd_path(in_fd_arg, in_fd);
  fildesh_encode_fd_path(out_fd_arg, out_fd);

  istat = fildesh_compat_fd_spawnlp_wait(
      -1, -1, 2,
      fds_to_inherit,
      st->fildesh_exe,
      "-stdin", in_fd_arg,
      "-stdout", out_fd_arg,
      "--",
      "|< stdin",
      "|- zec / \"hello \" / -",
      "|> stdout",
      NULL);
  assert(istat == 0);
}

int main(int argc, char** argv) {
  const char* input_data = "world\n";
  const char* expect_data = "hello world\n";
  const size_t expect_size = strlen(expect_data);
  char* output_data = NULL;
  size_t output_size;
  PipemCallbackArg st[1];

  assert(argc == 2 && "need fildesh executable as arg");

  st->fildesh_exe = argv[1];

  output_size = fildesh_tool_pipem(
      strlen(input_data), input_data,
      run_fildesh, st,
      &output_data);
  assert(output_size == expect_size);
  assert(0 == memcmp(output_data, expect_data, expect_size));

  if (output_data) {
    free(output_data);
  }
  return 0;
}
