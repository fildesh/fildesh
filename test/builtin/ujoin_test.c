#include "fildesh.h"
#include "fildesh_builtin.h"
#include "fildesh_tool.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>

typedef struct PipemFnArg PipemFnArg;
struct PipemFnArg {
  const char* input_large;
  const char* argv[20];
  unsigned argc;
  char small_input_arg[FILDESH_FD_PATH_SIZE_MAX];
  char large_input_arg[FILDESH_FD_PATH_SIZE_MAX];
  char output_arg[FILDESH_FD_PATH_SIZE_MAX];
};

FILDESH_TOOL_PIPEM_CALLBACK(run_query_ujoin, in_fd, out_fd, PipemFnArg*, st) {
  if (st->input_large) {
    const char* input_large = st->input_large;
    st->input_large = NULL;
    fildesh_encode_fd_path(st->small_input_arg, in_fd);
    fildesh_encode_fd_path(st->output_arg, out_fd);
    fildesh_tool_pipem(
        strlen(input_large), input_large,
        run_query_ujoin, st,
        NULL);
  } else {
    int istat;
    fildesh_encode_fd_path(st->large_input_arg, in_fd);
    istat = fildesh_builtin_ujoin_main(st->argc, (char**)st->argv, NULL, NULL);
    assert(istat == 0);
  }
}

int main() {
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
  PipemFnArg st[1];

  st->input_large = input_large;
  st->argc = 0;
  st->argv[st->argc++] = "ujoin";
  st->argv[st->argc++] = "-x-lut";
  st->argv[st->argc++] = st->small_input_arg;
  st->argv[st->argc++] = "-x";
  st->argv[st->argc++] = st->large_input_arg;
  st->argv[st->argc++] = "-o";
  st->argv[st->argc++] = st->output_arg;
  st->argv[st->argc++] = "-l";
  st->argv[st->argc++] = "-p";
  st->argv[st->argc++] = "x";
  st->argv[st->argc] = NULL;

  output_size = fildesh_tool_pipem(
      strlen(input_small), input_small,
      run_query_ujoin, st,
      &output_data);

  assert(output_size == expect_size);
  assert(0 == memcmp(output_data, expect_data, expect_size));
  free(output_data);
  return 0;
}
