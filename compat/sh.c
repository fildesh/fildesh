#include "include/fildesh/fildesh_compat_sh.h"
#include "include/fildesh/fildesh_compat_errno.h"
#include "include/fildesh/fildesh_compat_string.h"

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
fildesh_compat_sh_escape_argv_for_windows(const char* const* argv)
{
  char** escaped_argv;
  size_t i, argc;
  if (!argv || !argv[0]) {return NULL;}
  for (argc = 0; argv[argc]; ++argc) {/* Nothing.*/}
  escaped_argv = (char**) malloc(sizeof(char*) * (argc+1));
  if (!escaped_argv) {fildesh_compat_errno_trace(); return NULL;}
  for (i = 0; i < argc; ++i) {
    static const char* const replacements[] = {
      "\"\"",
    };
    escaped_argv[i] = fildesh_compat_string_byte_translate(
        argv[i], "\"", replacements, "\"", "\"");
  }
  escaped_argv[argc] = NULL;
  return escaped_argv;
}

  void
fildesh_compat_sh_free_escaped_argv(char** argv)
{
  unsigned i;
  unsigned n = 0;
  if (!argv || !argv[0]) {return;}
  while (argv[n]) {
    n += 1;
  }
  for (i = 0; i < n; ++i) {
    free(argv[i]);
  }
  free(argv);
}

  FildeshCompat_pid
fildesh_compat_sh_spawn(const char* const* argv)
{
  FildeshCompat_pid pid;
#ifdef _MSC_VER
  char** escaped_argv = fildesh_compat_sh_escape_argv_for_windows(argv);
  pid = _spawnvp(_P_NOWAIT, argv[0], escaped_argv);
  fildesh_compat_sh_free_escaped_argv(escaped_argv);
#else
  pid = fork();
  if (pid == 0) {
    execvp(argv[0], (char**)argv);
    fildesh_compat_errno_trace();
    exit(126);
  }
#endif
  if (pid < 0) {fildesh_compat_errno_trace();}
  return pid;
}

  void
fildesh_compat_sh_exec(const char* const* argv)
{
#ifdef _MSC_VER
  char** escaped_argv = fildesh_compat_sh_escape_argv_for_windows(argv);
  intptr_t istat = _spawnvp(_P_WAIT, argv[0], escaped_argv);
  fildesh_compat_sh_free_escaped_argv(escaped_argv);
  if (istat >= 0) {
    exit(istat & 0xFF);
  }
#else
  execvp(argv[0], (char**)argv);
#endif
}

  int
fildesh_compat_sh_wait(FildeshCompat_pid pid)
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
fildesh_compat_sh_chdir(const char* directory)
{
  int istat = -1;
#ifdef _MSC_VER
  istat = _chdir(directory);
#else
  istat = chdir(directory);
#endif
  if (istat != 0) {fildesh_compat_errno_trace();}
  return istat;
}

/* See kill.c for fildesh_compat_sh_kill() definition.*/
/* See env.c for fildesh_compat_sh_env() definition.*/
