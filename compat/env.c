
#ifndef _MSC_VER
# ifndef _POSIX_C_SOURCE
#  define _POSIX_C_SOURCE 200112L
# endif
#endif
#include <stdlib.h>

#include "fildesh_compat_errno.h"
#include "fildesh_compat_sh.h"

  int
fildesh_compat_sh_setenv(const char* key, const char* value)
{
  int istat = -1;
#ifndef _MSC_VER
  istat = setenv(key, value, 1);
#else
  istat = _putenv_s(key, value) == 0 ? 0 : -1;
#endif
  if (istat != 0) {fildesh_compat_errno_trace();}
  return istat;
}
