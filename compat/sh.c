#include "lace_compat_sh.h"
#include "lace_compat_errno.h"
#include "lace_compat_string.h"

#ifdef _MSC_VER
/* For _chdir().*/
#include <direct.h>

#include <io.h>
#include <process.h>
#else
#include <sys/wait.h>
#include <unistd.h>
#endif
#include <stdlib.h>

  char**
lace_compat_sh_escape_argv_for_windows(const char* const* argv)
{
  char** escaped_argv;
  unsigned i, argc;
  if (!argv || !argv[0]) {return NULL;}
  for (argc = 0; argv[argc]; ++argc) {/* Nothing.*/}
  escaped_argv = (char**) malloc(sizeof(char*) * (argc+1));
  for (i = 0; i < argc; ++i) {
    static const char* const replacements[] = {
      "\"\"",
    };
    escaped_argv[i] = lace_compat_string_byte_translate(
        argv[i], "\"", replacements, "\"", "\"");
  }
  escaped_argv[argc] = NULL;
  return escaped_argv;
}

  void
lace_compat_sh_free_escaped_argv(char** argv)
{
  unsigned i;
  if (!argv || !argv[0]) {return;}
  for (i = 0; argv[i]; ++i) {
    free(argv[i]);
  }
  free(argv);
}

  lace_compat_pid_t
lace_compat_sh_spawn(const char* const* argv)
{
  lace_compat_pid_t pid;
#ifdef _MSC_VER
  char** escaped_argv = lace_compat_sh_escape_argv_for_windows(argv);
  pid = _spawnvp(_P_NOWAIT, argv[0], escaped_argv);
  lace_compat_sh_free_escaped_argv(escaped_argv);
#else
  pid = fork();
  if (pid == 0) {
    execvp(argv[0], (char**)argv);
    lace_compat_errno_trace();
    exit(126);
  }
#endif
  if (pid < 0) {lace_compat_errno_trace();}
  return pid;
}

  void
lace_compat_sh_exec(const char* const* argv)
{
#ifdef _MSC_VER
  char** escaped_argv = lace_compat_sh_escape_argv_for_windows(argv);
  intptr_t istat = _spawnvp(_P_WAIT, argv[0], escaped_argv);
  lace_compat_sh_free_escaped_argv(escaped_argv);
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
