#include "lace.h"
#include "lace_compat_fd.h"
#include "lace_tool.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>

typedef struct PipemCallbackArg PipemCallbackArg;
struct PipemCallbackArg {
  const char* lace_exe;
  lace_compat_fd_t in_fd;
  lace_compat_fd_t out_fd;
};

LACE_TOOL_PIPEM_CALLBACK(run_lace, PipemCallbackArg*, st) {
  lace_compat_fd_t fds_to_inherit[] = {-1, -1, -1};
  char in_fd_arg[LACE_FD_PATH_SIZE_MAX];
  char out_fd_arg[LACE_FD_PATH_SIZE_MAX];
  int istat;

  fds_to_inherit[0] = st->in_fd;
  fds_to_inherit[1] = st->out_fd;
  lace_encode_fd_path(in_fd_arg, st->in_fd);
  lace_encode_fd_path(out_fd_arg, st->out_fd);

  istat = lace_compat_fd_spawnlp_wait(
      fds_to_inherit,
      st->lace_exe,
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

  assert(argc == 2 && "need lace executable as arg");

  st->lace_exe = argv[1];
  st->in_fd = lace_compat_fd_reserve();
  st->out_fd = lace_compat_fd_reserve();

  output_size = lace_tool_pipem(
      strlen(input_data), input_data, st->in_fd,
      run_lace, st,
      st->out_fd, &output_data);
  assert(output_size == expect_size);
  assert(0 == memcmp(output_data, expect_data, expect_size));

  if (output_data) {
    free(output_data);
  }
  return 0;
}
