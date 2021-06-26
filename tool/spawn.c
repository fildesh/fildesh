/** Spawn a command, possibly negating its exit status by prepending a "!" arg.
 *
 * Keep small; built often.
 **/
#ifdef _WIN32
#include <io.h>
#include <process.h>
#else
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#endif
#include <stdlib.h>

int lace_tool_spawn_close_expect(char** argv, const int* fdv, int expect) {
  int istat = 70;
#ifdef _WIN32
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

  /* Close the requested file descriptors.*/
  for (; fdv && *fdv >= 0; fdv = &fdv[1]) {
#ifdef _WIN32
    _close(*fdv);
#else
    close(*fdv);
#endif
  }

#ifdef _WIN32
  pid = _cwait(&istat, pid, 0);
#else
  pid = waitpid(pid, &istat, 0);
#endif
  if (pid < 0)  return 70;  /* -EX_SOFTWARE: Internal software error.*/

#ifdef _WIN32
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

#ifndef LACE_TOOL_LIBRARY
#define lace_tool_spawn_main main
#endif
int lace_tool_spawn_main(int argc, char** argv) {
#ifdef LACE_TOOL_LIBRARY
  const int* const fds_to_close = NULL;
#else
  const int fds_to_close[4] = {0, 1, 2, -1};
#endif
  if (argc < 2 || !argv[1]) {
    return 64;  /* EX_USAGE: Command line usage error.*/
  }
  if (argv[1][0] != '!' || argv[1][1] != '\0') {
    /* Just run the command.*/
#if defined(LACE_TOOL_LIBRARY) || defined(_WIN32)
    /* Exec does not seem to propagate exit status on Windows, so use Spawn.*/
    return lace_tool_spawn_close_expect(&argv[1], fds_to_close, 0);
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
  return lace_tool_spawn_close_expect(&argv[2], fds_to_close, -1);
}
