#include "fildesh_compat_sh.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

int main() {
  const char* const orig_argv[] = {
    "my_program",
    "--a_flag",
    "a somewhat longer string",
    "this one has \"some kind of quote\" in it",
    NULL,
  };
  const char* const expect_argv[] = {
    "\"my_program\"",
    "\"--a_flag\"",
    "\"a somewhat longer string\"",
    "\"this one has \"\"some kind of quote\"\" in it\"",
    NULL,
  };
  char** escaped_argv;
  unsigned i;

  escaped_argv = fildesh_compat_sh_escape_argv_for_windows(orig_argv);
  assert(escaped_argv);
  for (i = 0; expect_argv[i]; ++i) {
    assert(escaped_argv[i]);
    fprintf(stderr, "Got: %s\n", escaped_argv[i]);
    assert(0 == strcmp(escaped_argv[i], expect_argv[i]));
  }
  assert(!escaped_argv[i]);

  fildesh_compat_sh_free_escaped_argv(escaped_argv);
  return 0;
}
