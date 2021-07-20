#include "lace_compat_fd.h"
#include <assert.h>

int main() {
  lace_compat_fd_t new_stdin;
  lace_compat_fd_t new_stdout;
  lace_compat_fd_t fd;

  new_stdin = lace_compat_fd_claim(0);
  new_stdout = lace_compat_fd_claim(1);
  assert(new_stdin >= 3);
  assert(new_stdout >= 3);

  fd = lace_compat_fd_claim(0);
  assert(fd < 0);
  fd = lace_compat_fd_claim(1);
  assert(fd < 0);

  lace_compat_fd_close(new_stdin);
  lace_compat_fd_close(new_stdout);

  return 0;
}
