#include <fildesh/fildesh.h>
#include "include/fildesh/fildesh_compat_fd.h"
#include "fildesh_tool.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>

typedef struct PipemFnArg PipemFnArg;
struct PipemFnArg {
  fildesh_compat_fd_t stdin_fd;
  fildesh_compat_fd_t stdout_fd;
  const char* input_query;
  const char* argv[20];
  unsigned argc;
  char fd_arg[FILDESH_FD_PATH_SIZE_MAX];
};

FILDESH_TOOL_PIPEM_CALLBACK(run_query_bestmatch, in_fd, out_fd, PipemFnArg*, st) {
  if (st->input_query) {
    const char* input_query = st->input_query;
    st->input_query = NULL;
    st->stdin_fd = in_fd;
    st->stdout_fd = out_fd;
    fildesh_tool_pipem(
        strlen(input_query), input_query,
        run_query_bestmatch, st,
        NULL);
  }
  else {
    int istat;
    fildesh_compat_fd_t extra_fds[] = {-1, -1};
    extra_fds[0] = in_fd;
    fildesh_encode_fd_path(st->fd_arg, in_fd);
    istat = fildesh_compat_fd_spawnvp_wait(
        st->stdin_fd, st->stdout_fd, 2, extra_fds, st->argv);
    assert(istat == 0);
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
  PipemFnArg st[1];

  assert(argc == 2);

  st->input_query = input_query;
  st->argc = 0;
  st->argv[st->argc++] = argv[1];
  st->argv[st->argc++] = "-x-lut";
  st->argv[st->argc++] = "-";
  st->argv[st->argc++] = "-x";
  st->argv[st->argc++] = st->fd_arg;
  st->argv[st->argc++] = "-d";
  st->argv[st->argc++] = "";
  st->argv[st->argc++] = NULL;

  output_size = fildesh_tool_pipem(
      strlen(input_table), input_table,
      run_query_bestmatch, st,
      &output_data);

  assert(output_size == expect_size);
  assert(0 == memcmp(output_data, expect_data, expect_size));
  free(output_data);
  return 0;
}
