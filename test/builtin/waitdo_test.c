#include "lace.h"
#include "lace_compat_fd.h"
#include "lace_tool.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>

typedef struct PipemFnArg PipemFnArg;
struct PipemFnArg {
  const char* waitdo_exe;
  const char* shout_exe;
};

int lace_builtin_time2sec_main(int, char**, LaceX**, LaceO**);

LACE_TOOL_PIPEM_CALLBACK(run_waitdo, in_fd, out_fd, const PipemFnArg*, st) {
  int istat = lace_compat_fd_spawnlp_wait(
      in_fd, out_fd, 2, NULL,
      st->waitdo_exe, "--",
      st->shout_exe, "-", "hello", "there", NULL);
  assert(istat == 0);
}

int main(int argc, char** argv) {
  const char input_data[] = "derp";
  const size_t input_data_size = strlen(input_data);
  const char expect_data[] = "hello there\n";
  const size_t expect_size = strlen(expect_data);
  size_t output_size;
  char* output_data = NULL;
  PipemFnArg st[1];

  /* Expect to be given path to `waitdo` and `shout`.*/
  assert(argc == 3);
  st->waitdo_exe = argv[1];
  st->shout_exe = argv[2];

  output_size = lace_tool_pipem(
      input_data_size, input_data,
      run_waitdo, st,
      &output_data);
  assert(output_size == expect_size);
  assert(0 == memcmp(output_data, expect_data, expect_size));
  free(output_data);
  return 0;
}
