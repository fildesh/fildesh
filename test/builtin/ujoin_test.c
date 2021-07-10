#include "lace.h"
#include "lace_compat_fd.h"
#include "lace_tool.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct PipemFnArg PipemFnArg;
struct PipemFnArg {
  const lace_compat_fd_t* fds;
  const char* input_large;
  const char* argv[10];
};

LACE_TOOL_PIPEM_CALLBACK(run_query_ujoin, PipemFnArg*, st) {
  if (!st->input_large) {
    int istat;
    istat = lace_compat_fd_spawnvp_wait(st->fds, st->argv);
    assert(istat == 0);
  } else {
    const char* input_large = st->input_large;
    st->input_large = NULL;
    lace_tool_pipem(
        strlen(input_large), input_large, 0,
        run_query_ujoin, st,
        -1, NULL);
  }
}

int main(int argc, char** argv) {
  static const char input_small[] =
    "jwbackus\tBackus, John\n"
    "wbsearp\tEarp, Wyatt\n"
    "jhhollid\tHolliday, John\n"
    "rhui\tHui, Roger\n"
    "keiverso\tIverson, Kenneth\n"
    "apklinkh\tKlinkhamer, Alex\n"
    "wbmaster\tMasterson, William\n"
    "chmoore\tMoore, Charles\n"
    ;
  static const char input_large[] =
    "keiverso\t1\n"
    "rhui\t2\n"
    "chmoore\t3\n"
    "apklinkh\t4\n"
    "jwbackus\t5\n"
    "wbsearp\t6\n"
    "jhhollid\t7\n"
    ;
  static const char expect_data[] =
    "jwbackus\t5\tBackus, John\n"
    "wbsearp\t6\tEarp, Wyatt\n"
    "jhhollid\t7\tHolliday, John\n"
    "rhui\t2\tHui, Roger\n"
    "keiverso\t1\tIverson, Kenneth\n"
    "apklinkh\t4\tKlinkhamer, Alex\n"
    "wbmaster\tx\tMasterson, William\n"
    "chmoore\t3\tMoore, Charles\n"
    ;
  const size_t expect_size = strlen(expect_data);
  size_t output_size;
  char* output_data = NULL;
  char fd_arg[30];
  PipemFnArg st[1];
  lace_compat_fd_t inherit_fds[] = {0, 0, 1, -1};

  assert(argc == 2);

  inherit_fds[0] = lace_compat_fd_reserve();
  assert(inherit_fds[0] >= 0);
  sprintf(fd_arg, "/dev/fd/%u", (unsigned)inherit_fds[0]);
  st->fds = inherit_fds;
  st->input_large = input_large;
  st->argv[0] = argv[1];
  st->argv[1] = fd_arg;
  st->argv[2] = "-";
  st->argv[3] = "-l";
  st->argv[4] = "-p";
  st->argv[5] = "x";
  st->argv[6] = NULL;

  output_size = lace_tool_pipem(
      strlen(input_small), input_small, inherit_fds[0],
      run_query_ujoin, st,
      1, &output_data);

  assert(output_size == expect_size);
  assert(0 == memcmp(output_data, expect_data, expect_size));
  free(output_data);
  return 0;
}
