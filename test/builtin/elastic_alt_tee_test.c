#include "lace.h"
#include "lace_compat_fd.h"
#include "lace_tool.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct PipemFnArg PipemFnArg;
struct PipemFnArg {
  unsigned tee_index;
  const lace_compat_fd_t* tee_fds;
  const char* elastic_argv[10];
  size_t expect_size;
  const char* expect_string;
};

static void run_expect_elastic(PipemFnArg* st) {
  lace_compat_fd_t fd = st->tee_fds[st->tee_index];
  const unsigned tee_index = st->tee_index++;
  if (fd < 0) {
    int istat;
    st->elastic_argv[tee_index] = NULL;
    istat = lace_compat_fd_spawnvp_wait(st->tee_fds, st->elastic_argv);
    assert(istat == 0);
  } else {
    char tee_arg[30];
    size_t output_size;
    char* output_data = NULL;
    sprintf(tee_arg, "/dev/fd/%u", (unsigned)fd);
    st->elastic_argv[tee_index] = tee_arg;
    lace_log_tracef("Piping tee_index %u", tee_index);
    output_size = lace_tool_pipem(
        0, NULL, -1,
        (void (*) (void*))run_expect_elastic, st,
        fd, &output_data);
    lace_log_tracef("Checking tee_index %u", tee_index);
    assert(output_size == st->expect_size);
    assert(0 == memcmp(output_data, st->expect_string, st->expect_size));
    free(output_data);
  }
}

int main(int argc, char** argv) {
  PipemFnArg st[1];
  lace_compat_fd_t tee_fds[] = {0, 1, 1, 1, -1};
  unsigned i;

  assert(argc == 2);

  st->tee_index = 1;
  st->tee_fds = tee_fds;
  st->elastic_argv[0] = argv[1];
  st->expect_string =
    "Greetings, I am a test string.\n"
    "My goal is to be long enough to be interesting.\n"
    "But boring enough to be overlooked.\n"
    ;
  st->expect_size = strlen(st->expect_string);

  for (i = 1; tee_fds[i] >= 0; ++i) {
    tee_fds[i] = lace_compat_fd_reserve();
    assert(tee_fds[i] >= 0);
  }
  lace_tool_pipem(
      st->expect_size, st->expect_string, 0,
      (void (*) (void*))run_expect_elastic, st,
      -1, NULL);

  return 0;
}
