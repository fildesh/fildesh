
#if defined(_MSC_VER)
# include <windows.h>
#else
# if !defined(_POSIX_C_SOURCE)
#  define _POSIX_C_SOURCE 1
# endif
# include <sys/types.h>
# include <signal.h>
#endif

#include "include/fildesh/fildesh_compat_errno.h"
#include "include/fildesh/fildesh_compat_sh.h"

  int
fildesh_compat_sh_kill(FildeshCompat_pid pid)
{
  int istat = -1;
#ifdef _MSC_VER
  HANDLE handle = OpenProcess(PROCESS_TERMINATE, 0, pid);
  if (handle) {
    istat = TerminateProcess(handle, 1) ? 0 : -1;
    CloseHandle(handle);
  }
#else
  istat = kill(pid, SIGKILL);
#endif
  if (istat != 0) {fildesh_compat_errno_trace();}
  return istat;
}

