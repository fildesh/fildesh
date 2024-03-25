#include <assert.h>
#include <stdlib.h>

#include <fildesh/fildesh.h>

#include "fildesh_tool.h"

int
fildesh_builtin_void_main(
    unsigned argc, char** argv,
    FildeshX** inputv, FildeshO** outputv);

FILDESH_TOOL_PIPEM_NULLARY_CALLBACK(run_void, in_fd, out_fd) {
  const unsigned argc = 1;
  char* argv[2] = {(char*)"void", NULL};
  FildeshX* inputv[2] = {NULL, NULL};
  FildeshO* outputv[2] = {NULL, NULL};
  int exstatus;
  inputv[0] = open_fd_FildeshX(in_fd);
  outputv[0] = open_fd_FildeshO(out_fd);
  exstatus = fildesh_builtin_void_main(argc, argv, inputv, outputv);
  assert(exstatus == 0);
  assert(!inputv[0]);
  assert(!outputv[0]);
}

int main() {
  const size_t input_size = 10000;
  char* input_data = (char*) malloc(input_size);
  char* output_data = NULL;
  size_t output_size;
  unsigned i;

  assert(input_data);
  for (i = 0; i < input_size; ++i) {
    input_data[i] = (char)((i % (127 - 20)) + 20);
  }

  output_size = fildesh_tool_pipem(
      input_size, input_data,
      run_void, NULL,
      &output_data);
  assert(output_size == 0);
  free(input_data);
  if (output_data) {free(output_data);}
  return 0;
}
