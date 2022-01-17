/** Spawn a command, possibly negating its exit status by prepending a "!" arg.
 *
 * Keep small; built often.
 **/
#ifdef _MSC_VER
#include <io.h>
#include <process.h>
#else
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#endif
#include <stdio.h>
#include <stdlib.h>

static
  int
fildesh_tool_spawn_expect(char** argv, int expect) {
  int istat = 70;
#ifdef _MSC_VER
  intptr_t pid = _spawnvp(_P_NOWAIT, argv[0], argv);
  if (pid < 0) {
    return (expect == 0) ? 126 : 65;
  }
#else
  pid_t pid = fork();
  if (pid < 0)  return 71;  /* -EX_OSERR: Can't fork.*/
  if (pid == 0) {
    execvp(argv[0], argv);
    if (expect != 0)  exit(0);
    exit(126);
  }
#endif

#ifndef FILDESH_TOOL_LIBRARY
  fclose(stdin);
  fclose(stdout);
  fclose(stderr);
#endif

#ifdef _MSC_VER
  pid = _cwait(&istat, pid, 0);
#else
  pid = waitpid(pid, &istat, 0);
#endif
  if (pid < 0)  return 70;  /* -EX_SOFTWARE: Internal software error.*/

#ifdef _MSC_VER
  istat &= 0xFF;
#else
  if (!WIFEXITED(istat))  return 70;
  istat = WEXITSTATUS(istat);
#endif
  if ((expect < 0 && istat > 0) || (istat == expect)) {
    return 0;
  }
  if (expect != 0)  return 65;  /* EX_DATAERR: Input caused unexpected success.*/
  return istat;
}

#ifndef FILDESH_TOOL_LIBRARY
#define fildesh_tool_spawn_main main
#endif
int fildesh_tool_spawn_main(int argc, char** argv) {
  if (argc < 2 || !argv[1]) {
    return 64;  /* EX_USAGE: Command line usage error.*/
  }
  if (argv[1][0] != '!' || argv[1][1] != '\0') {
    /* Just run the command.*/
#if defined(FILDESH_TOOL_LIBRARY) || defined(_MSC_VER)
    /* Exec does not seem to propagate exit status on Windows, so use Spawn.*/
    return fildesh_tool_spawn_expect(&argv[1], 0);
#else
    execvp(argv[1], &argv[1]);
#endif
  }
  if (argc < 3 || !argv[2]) {
    return 64;  /* `spawn !` can serve as `false`, much like `bash -c "!"`.*/
  }
  if (argc == 3 && argv[2][0] == '!' && argv[2][1] == '\0') {
    return 0;  /* `spawn ! !` can serve as `true`, much like `bash -c "! !"`.*/
  }
  return fildesh_tool_spawn_expect(&argv[2], -1);
}
