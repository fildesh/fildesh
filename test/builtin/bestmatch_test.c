#include "lace.h"
#include "lace_compat_fd.h"
#include "lace_tool.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>

typedef struct PipemFnArg PipemFnArg;
struct PipemFnArg {
  lace_compat_fd_t stdin_fd;
  lace_compat_fd_t stdout_fd;
  const char* input_query;
  char* argv[10];
};

LACE_TOOL_PIPEM_CALLBACK(run_query_bestmatch, in_fd, out_fd, PipemFnArg*, st) {
  if (!st->input_query) {
    int istat;
    lace_compat_fd_t extra_fds[] = {-1, -1};
    extra_fds[0] = in_fd;
    lace_encode_fd_path(st->argv[2], in_fd);
    istat = lace_compat_fd_spawnvp_wait(
        st->stdin_fd, st->stdout_fd, 2, extra_fds,
        (const char**) st->argv);
    assert(istat == 0);
  } else {
    const char* input_query = st->input_query;
    st->input_query = NULL;
    st->stdin_fd = in_fd;
    st->stdout_fd = out_fd;
    lace_tool_pipem(
        strlen(input_query), input_query,
        run_query_bestmatch, st,
        NULL);
  }
}

int main(int argc, char** argv) {
  static const char input_table[] =
    "jwbackus\tJohn Backus\n"
    "wbsearp\tWyatt Earp\n"
    "anwhiteh\tAlfred North Whitehead\n"
    "jhhollid\tJohn Holliday\n"
    "rhui\tRoger Hui\n"
    "keiverso\tKenneth Iverson\n"
    "apklinkh\tAlex Klinkhamer\n"
    "wbmaster\tWilliam Masterson\n"
    "chmoore\tCharles Moore\n"
    ;
  static const char input_query[] =
    "ken iverson\n"
    "Roger Hui\n"
    "chuck moore\n"
    "Alex Klinkhammer\n"
    "john backus\n"
    "wyatt\n"
    "doc holliday\n"
    ;
  static const char expect_data[] =
    "keiverso\tKenneth Iverson\n"
    "rhui\tRoger Hui\n"
    "chmoore\tCharles Moore\n"
    "apklinkh\tAlex Klinkhamer\n"
    "jwbackus\tJohn Backus\n"
    "wbsearp\tWyatt Earp\n"
    "jhhollid\tJohn Holliday\n"
    ;
  const size_t expect_size = strlen(expect_data);
  size_t output_size;
  char* output_data = NULL;
  char fd_arg[LACE_FD_PATH_SIZE_MAX];
  PipemFnArg st[1];

  assert(argc == 2);

  st->input_query = input_query;
  st->argv[0] = argv[1];
  st->argv[1] = "-";
  st->argv[2] = fd_arg;
  st->argv[3] = NULL;

  output_size = lace_tool_pipem(
      strlen(input_table), input_table,
      run_query_bestmatch, st,
      &output_data);

  assert(output_size == expect_size);
  assert(0 == memcmp(output_data, expect_data, expect_size));
  free(output_data);
  return 0;
}
