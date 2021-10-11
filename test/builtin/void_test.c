#include "fildesh.h"
#include "fildesh_compat_fd.h"
#include "fildesh_tool.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>

LACE_TOOL_PIPEM_CALLBACK(run_void, in_fd, out_fd, const char*, void_exe) {
  int istat;
  istat = lace_compat_fd_spawnlp_wait(in_fd, out_fd, 2, NULL, void_exe, NULL);
  assert(istat == 0);
}

int main(int argc, char** argv) {
  const size_t input_size = 10000;
  char* input_data = (char*) malloc(input_size);
  char* output_data = NULL;
  size_t output_size;
  unsigned i;
  char* void_exe = argv[1];
  assert(argc == 2);
  assert(void_exe);

  assert(input_data);
  for (i = 0; i < input_size; ++i) {
    input_data[i] = (char)((i % (127 - 20)) + 20);
  }

  output_size = lace_tool_pipem(
      input_size, input_data,
      run_void, void_exe,
      &output_data);
  assert(output_size == 0);
  free(input_data);
  if (output_data) {free(output_data);}
  return 0;
}
