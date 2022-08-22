#include "fildesh_compat_sh.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char** fildesh_tool_escape_argv_for_windows(const char* const*);

int main() {
  int istat;
  const char* v;
  istat = fildesh_compat_sh_setenv("MY_TEST_VARIABLE_A", "hello");
  assert(istat == 0);
  istat = fildesh_compat_sh_setenv("MY_TEST_VARIABLE_B", "world");
  assert(istat == 0);

  v = getenv("MY_TEST_VARIABLE_A");
  assert(v);
  assert(0 == strcmp(v, "hello"));

  v = getenv("MY_TEST_VARIABLE_B");
  assert(v);
  assert(0 == strcmp(v, "world"));

  return 0;
}
