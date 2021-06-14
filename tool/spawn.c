/** Spawn a command, possibly negating its exit status by prepending a "!" arg.
 *
 * Keep small; built often.
 **/
#ifdef _WIN32
#include <process.h>
#else
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#endif
#include <errno.h>

static int lace_tool_spawn_expect(char** argv, int expect) {
#ifdef _WIN32
  intptr_t istat = _spawnvp(_P_WAIT, argv[0], argv);
  if (istat < 0)  return -1;
  istat &= 0xFF;
#else
  int istat = -1;
  pid_t pid = fork();
  if (pid < 0)  return -71;  /* -EX_OSERR: Can't fork.*/
  if (pid == 0) {
    execvp(argv[0], argv);
    if (expect != 0)  return 0;
    return -126;
  }
  pid = waitpid(pid, &istat, 0);
  if (pid < 0)  return -70;  /* -EX_SOFTWARE: Internal software error.*/
  if (!WIFEXITED(istat))  return -70;
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
  int istat = -1;
  if (argc < 2 || !argv[1])  return 64;  /* EX_USAGE: Command line usage error.*/

  if (argv[1][0] != '!' || argv[1][1] != '\0') {
    /* Just run the command.*/
#if defined(LACE_TOOL_LIBRARY) || defined(_WIN32)
    /* Exec does not seem to propagate exit status on Windows, so use Spawn.*/
    istat = lace_tool_spawn_expect(&argv[1], 0);
#else
    execvp(argv[1], &argv[1]);
#endif
  }
  else if (argc < 3 || !argv[2]) {
    return 64;  /* `spawn !` can serve as `false`, much like `bash -c "!"`.*/
  }
  else if (argc == 3 && argv[2][0] == '!' && argv[2][1] == '\0') {
    return 0;  /* `spawn ! !` can serve as `true`, much like `bash -c "! !"`.*/
  }
  else {
    istat = lace_tool_spawn_expect(&argv[2], -1);
  }
  if (istat >= 0)  return istat;
  if (istat < -1)  return -istat;
  if (errno == ENOENT)  return 127;  /* Command not found.*/
  return 126;  /* Command invoked cannot execute.*/
}
