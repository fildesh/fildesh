#include "lace_compat_sh.h"
#include "lace_compat_errno.h"

#ifdef _MSC_VER
#include <io.h>
#include <process.h>
#else
#include <sys/wait.h>
#include <unistd.h>
#endif
#include <stdlib.h>

  lace_compat_pid_t
lace_compat_sh_spawn(const char* const* argv)
{
  lace_compat_pid_t pid;
#ifdef _MSC_VER
  pid = _spawnvp(_P_NOWAIT, argv[0], (char**)argv);
#else
  pid = fork();
  if (pid == 0) {
    execvp(argv[0], (char**)argv);
    exit(126);
  }
#endif
  return pid;
}

  void
lace_compat_sh_exec(const char* const* argv)
{
#ifdef _MSC_VER
  intptr_t istat = _spawnvp(_P_WAIT, argv[0], (char**)argv);
  if (istat >= 0) {
    exit(istat & 0xFF);
  }
#else
  execvp(argv[0], (char**)argv);
#endif
}

  int
lace_compat_sh_wait(lace_compat_pid_t pid)
{
  int istat = -1;
#ifdef _MSC_VER
  pid = _cwait(&istat, pid, 0);
#else
  pid = waitpid(pid, &istat, 0);
#endif
  if (pid < 0) {return -1;}
#ifdef _MSC_VER
  istat &= 0xFF;
#else
  if (!WIFEXITED(istat)) {return -1;}
  istat = WEXITSTATUS(istat);
#endif
  return istat;
}


  int
lace_compat_sh_chdir(const char* directory)
{
  int istat = -1;
#ifdef _MSC_VER
  istat = _chdir(directory);
#else
  istat = chdir(directory);
#endif
  return istat;
}
