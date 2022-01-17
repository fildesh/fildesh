#include "fildesh.h"
#include "fildesh_compat_fd.h"
#include "fildesh_tool.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>

typedef struct PipemFnArg PipemFnArg;
struct PipemFnArg {
  unsigned tee_index;
  fildesh_compat_fd_t stdin_fd;
  fildesh_compat_fd_t* tee_fds;
  char* elastic_argv[10];
  size_t expect_size;
  const char* expect_string;
};

FILDESH_TOOL_PIPEM_CALLBACK(run_expect_elastic, in_fd, out_fd, PipemFnArg*, st) {
  char tee_arg[FILDESH_FD_PATH_SIZE_MAX];
  if (in_fd >= 0) {
    st->stdin_fd = in_fd;
  }
  if (out_fd >= 0) {
    st->tee_fds[st->tee_index] = out_fd;
    fildesh_encode_fd_path(tee_arg, out_fd);
    st->elastic_argv[1+st->tee_index] = tee_arg;
    st->tee_index += 1;
  }

  if (st->tee_fds[st->tee_index] < 0) {
    int istat;
    st->elastic_argv[st->tee_index+1] = NULL;
    istat = fildesh_compat_fd_spawnvp_wait(
        st->stdin_fd, -1, 2, st->tee_fds, (const char**)st->elastic_argv);
    assert(istat == 0);
  } else {
    size_t output_size;
    char* output_data = NULL;
    const unsigned tee_index = st->tee_index;
    fildesh_log_tracef("Piping tee_index %u", tee_index);
    output_size = fildesh_tool_pipem(
        0, NULL,
        run_expect_elastic, st,
        &output_data);
    fildesh_log_tracef("Checking tee_index %u", tee_index);
    assert(output_size == st->expect_size);
    assert(0 == memcmp(output_data, st->expect_string, st->expect_size));
    free(output_data);
  }
}

int main(int argc, char** argv) {
  PipemFnArg st[1];
  fildesh_compat_fd_t tee_fds[] = {1, 1, 1, -1};

  assert(argc == 2);

  st->tee_index = 0;
  st->tee_fds = tee_fds;
  st->elastic_argv[0] = argv[1];
  st->expect_string =
    "Greetings, I am a test string.\n"
    "My goal is to be long enough to be interesting.\n"
    "But boring enough to be overlooked.\n"
    ;
  st->expect_size = strlen(st->expect_string);

  fildesh_tool_pipem(
      st->expect_size, st->expect_string,
      run_expect_elastic, st,
      NULL);

  return 0;
}
