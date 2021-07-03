#include "lace.h"
#include "lace_tool.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>

static const char input_data[] =
    "1\n"
    "1:00\n"
    "1:00:00\n"
    "1:00:00:00\n"
    "30\n"
    "1:20\n"
    "11:53:00\n"
    "29:12:44:3\n"
;
static const size_t input_data_size = sizeof(input_data)-1;

static const char expected_output[] =
    "1\n"
    "60\n"
    "3600\n"
    "86400\n"
    "30\n"
    "80\n"
    "42780\n"
    "2551443\n"
;
static const size_t expected_output_size = sizeof(expected_output)-1;


int lace_builtin_time2sec_main(int, char**, LaceX**, LaceO**);

void run_time2sec() {
  const char* const argv[2] = { "time2sec", NULL };
  int istat = lace_builtin_time2sec_main(1, (char**)argv, NULL, NULL);
  assert(istat == 0);
}

int main() {
  size_t output_size;
  char* output_data = NULL;

  output_size = lace_tool_pipem(
      input_data_size, input_data, 0,
      (void (*) (void*))run_time2sec, NULL,
      1, &output_data);
  assert(output_size == expected_output_size);
  assert(0 == memcmp(output_data, expected_output, output_size));

  if (output_data) {
    free(output_data);
  }
  return 0;
}
