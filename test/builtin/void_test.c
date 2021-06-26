#include "lace_tool.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>

int main_void(int argi, int argc, char** argv);

void run_void() {
  const char* const argv[2] = { "void", NULL };
  int istat = main_void(1, 1, (char**)argv);
  assert(istat == 0);
}

int main() {
  const size_t input_size = 10000;
  char* input_data = (char*) malloc(input_size);
  size_t output_size;
  unsigned i;

  assert(input_data);
  for (i = 0; i < input_size; ++i) {
    input_data[i] = (char)((i % (127 - 20)) + 20);
  }

  output_size = lace_tool_pipem(
      input_size, input_data, 0,
      (void (*) (void*))run_void, NULL,
      1, NULL);
  assert(output_size == 0);
  free(input_data);
  return 0;
}
