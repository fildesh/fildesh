#include "lace.h"
#include "lace_compat_fd.h"
#include "lace_tool.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ExpectSshArgs(host) \
  "-o StrictHostKeyChecking=no -o PasswordAuthentication=no " \
  host " sh -s " host
#define ExpectHostCommand \
  "some_command --a_flag an_arg another_arg"

typedef struct PipemFnArg PipemFnArg;
struct PipemFnArg {
  const char* sshall_exe;
  const char* echocat_exe;
};

LACE_TOOL_PIPEM_CALLBACK(run_waitdo, in_fd, out_fd, const PipemFnArg*, st) {
  int istat;
  istat = lace_compat_fd_spawnlp_wait(
      in_fd, out_fd, 2, NULL,
      st->sshall_exe, "-ssh", st->echocat_exe, "-",
      ExpectHostCommand, NULL);
  assert(istat == 0);
}

int main(int argc, char** argv) {
  const char input_data[] =
    "myhost1\n"
    "myhost2\n"
    "myhost3\n"
    ;
  const size_t input_data_size = strlen(input_data);
  const char expect_data[] =
    ExpectSshArgs("myhost1") "\n" ExpectHostCommand
    ExpectSshArgs("myhost2") "\n" ExpectHostCommand
    ExpectSshArgs("myhost3") "\n" ExpectHostCommand
    ;
  const size_t expect_size = strlen(expect_data);
  size_t output_size;
  char* output_data = NULL;
  PipemFnArg st[1];

  assert(argc == 3);
  st->sshall_exe = argv[1];
  st->echocat_exe = argv[2];

  output_size = lace_tool_pipem(
      input_data_size, input_data,
      run_waitdo, st,
      &output_data);
  fprintf(stderr, "Got:\n%s\n", output_data);
  assert(output_size == expect_size);
  assert(0 == memcmp(output_data, expect_data, expect_size));
  free(output_data);
  return 0;
}
