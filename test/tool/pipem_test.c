
#include "lace_tool.h"
#include "lace_compat_fd.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

/* An aspiration test case for `shout`.*/
static const char expected_text[] =
    "SELECT * FROM TestCase WHERE TestCase.passing is TRUE;\n";
static size_t expected_text_size = sizeof(expected_text)-1;

LACE_TOOL_PIPEM_CALLBACK(run_shout, in_fd, out_fd, const char*, shout_exe) {
  int istat;
  istat = lace_compat_fd_spawnlp_wait(
      in_fd, out_fd, 2, NULL,
      shout_exe, "-",
      "SELECT", "*", "FROM", "TestCase",
      "WHERE", "TestCase.passing", "is", "TRUE;",
      NULL);
  assert(istat == 0);
}

/* An aspiration test case for `expectish`.*/
LACE_TOOL_PIPEM_CALLBACK(run_expectish, in_fd, out_fd, const char*, expectish_exe) {
  int istat;
  istat = lace_compat_fd_spawnlp_wait(
      in_fd, out_fd, 2, NULL,
      expectish_exe, "-",
      "SELECT", "*", "FROM", "TestCase",
      "WHERE", "TestCase.passing", "is", "TRUE;",
      NULL);
  assert(istat == 0);
}

int main(int argc, const char** argv) {
  const char* shout_exe;
  const char* expectish_exe;
  size_t output_size;
  char* output_data = NULL;

  assert(argc == 3);
  shout_exe = argv[1];
  expectish_exe = argv[2];

  output_size = lace_tool_pipem(
      0, NULL,
      run_shout, (void*)shout_exe,
      &output_data);
  assert(output_size == expected_text_size);
  assert(0 == memcmp(output_data, expected_text, output_size));

  /* Use the same text but as input for `expectish`.*/
  output_size = lace_tool_pipem(
      expected_text_size, expected_text,
      run_expectish, (void*)expectish_exe,
      NULL);
  assert(output_size == 0);


  if (output_data) {
    free(output_data);
  }
  return 0;
}
