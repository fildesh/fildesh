#include "fildesh_compat_fd.h"
#include <assert.h>

int main() {
  fildesh_compat_fd_t new_stdin;
  fildesh_compat_fd_t new_stdout;
  fildesh_compat_fd_t fd;

  new_stdin = fildesh_compat_fd_claim(0);
  new_stdout = fildesh_compat_fd_claim(1);
  assert(new_stdin >= 3);
  assert(new_stdout >= 3);

  fd = fildesh_compat_fd_claim(0);
  assert(fd < 0);
  fd = fildesh_compat_fd_claim(1);
  assert(fd < 0);

  fildesh_compat_fd_close(new_stdin);
  fildesh_compat_fd_close(new_stdout);

  return 0;
}
