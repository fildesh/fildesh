
#include "fildesh.h"
#include "fildesh_compat_fd.h"
#include "fildesh_compat_file.h"
#include "fildesh_tool.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int fildesh_builtin_execfd_main(unsigned, char**, FildeshX**, FildeshO**);

typedef struct PipemFnArg PipemFnArg;
struct PipemFnArg {
  unsigned fd_index;
  char stdout_buf[FILDESH_FD_PATH_SIZE_MAX];
  const char** argv;
};

FILDESH_TOOL_PIPEM_CALLBACK(run_execfd, in_fd, out_fd, PipemFnArg*, st) {
  int istat;
  unsigned fd_index = st->fd_index++;
  char buf[FILDESH_INT_BASE10_SIZE_MAX];

  if (fd_index == 0) {
    fildesh_encode_fd_path(st->stdout_buf, out_fd);
    st->argv[6] = st->stdout_buf;
    fd_index = st->fd_index++;
  }

  fildesh_encode_int_base10(buf, in_fd);

  if (fd_index == 1) {
    st->argv[9] = buf;
    fildesh_tool_pipem(
        strlen("hello"), "hello",
        run_execfd, st,
        NULL);
  } else if (fd_index == 2) {
    st->argv[11] = buf;
    fildesh_tool_pipem(
        strlen("world"), "world",
        run_execfd, st,
        NULL);
  } else {
    unsigned i;
    st->argv[13] = buf;
    for (i = 0; st->argv[i]; ++i) {
      fildesh_log_tracef("argv[%u] = %s", i, st->argv[i]);
    }
    istat = fildesh_builtin_execfd_main(i, (char**)st->argv, NULL, NULL);
    assert(istat == 0);
  }
}

int main(int argc, char** argv) {
  char* zec_exe;
  char* tmp_exe;
  FildeshX* zec_exe_in;
  const char* spawn_argv[] = {
    "execfd",
    "-exe", "replace_with_tmp_exe",
    "-stdin", "/dev/null",
    "-stdout", "replace_with_stdout_file",
    "x_a_x_a_x_a",
    "--",
    "replace_with_fd",
    "/",
    "replace_with_fd",
    " there ",
    "replace_with_fd",
    "/",
    NULL,
  };
  const char* output_directory = getenv("TEST_TMPDIR");
  PipemFnArg st[1];
  char* input_data = NULL;
  size_t input_size;
  const char expect_data[] = "hello there world";
  const size_t expect_size = strlen(expect_data);
  char* output_data = NULL;
  size_t output_size;

  assert(argc == 2);
  zec_exe = argv[1];

  tmp_exe = fildesh_compat_file_catpath(output_directory, "mylittle.exe");
  assert(tmp_exe);

  st->fd_index = 0;
  st->argv = spawn_argv;

  st->argv[2] = tmp_exe;
  zec_exe_in = open_FildeshXF(zec_exe);
  input_data = slurp_FildeshX(zec_exe_in);
  input_size = zec_exe_in->size;

  output_size = fildesh_tool_pipem(
      input_size, input_data,
      run_execfd, st,
      &output_data);

  assert(output_size > 0);
  fildesh_log_trace(output_data);
  assert(output_size == expect_size);
  assert(0 == memcmp(output_data, expect_data, expect_size));

  close_FildeshX(zec_exe_in);
  free(output_data);
  free(tmp_exe);
  return 0;
}

