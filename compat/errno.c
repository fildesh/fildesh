#include "include/fildesh/fildesh_compat_errno.h"
#include "include/fildesh/fildesh_compat_file.h"
#include <errno.h>
#include <stdio.h>

  int
fildesh_compat_errno_clear()
{
  int e = errno;
  errno = 0;
  return e;
}

  int
fildesh_compat_errno_trace_(const char* file, const char* func, unsigned line)
{
  int e = errno;
  if (e != 0) {
    file = fildesh_compat_file_basename(file);
    fprintf(stderr, "TRACE %s(%u): Errno %d in %s.\n", file, line, e, func);
    errno = e;
    perror("TRACE perror");
  }
  errno = 0;
  return e;
}
