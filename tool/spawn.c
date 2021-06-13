/* Keep small. Built often.*/
#ifdef _WIN32
#include <process.h>
#else
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#endif
#include <errno.h>

int main(int argc, char** argv) {
  if (argc < 2 || !argv[1])  return 66;  /* EX_USAGE: Command line usage error.*/

  if (argv[1][0] != '!' || argv[1][1] != '\0') {
    /* Just run the command.*/
#ifdef _WIN32
    _execvp(argv[1], &argv[1]);
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
#ifdef _WIN32
    intptr_t istat = _spawnvp(_P_WAIT, argv[2], &argv[2]);
#else
    int istat;
    pid_t pid = fork();
    if (pid < 0)  return 71;  /* EX_OSERR: Can't fork.*/
    if (pid == 0) {
      execvp(argv[2], &argv[2]);
      return 0;  /* Pretend success so overall command fails.*/
    }
    pid = waitpid(pid, &istat, 0);
    if (pid < 0)  return 70;  /* EX_SOFTWARE: Internal software error.*/
    if (!WIFEXITED(istat))  return 70;
    istat = WEXITSTATUS(istat);
#endif
    if (istat > 0)  return 0;
    if (istat == 0)  return 65;  /* EX_DATAERR: Input caused unexpected success.*/
  }
  if (errno == ENOENT)  return 127;  /* Command not found.*/
  return 126;  /* Command invoked cannot execute.*/
}
