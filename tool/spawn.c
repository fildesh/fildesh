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

#if defined(UNIT_TESTING) || defined(_MSC_VER)
#include <string.h>
static size_t escape_allocated_argv_for_windows(char* dst, const char* arg) {
  size_t bytec = 1;
  size_t n;
  if (dst) {
    dst[0] = '"';
    dst = &dst[1];
  }
  for (n = strcspn(arg, "\"");
       arg[n];
       n = strcspn(arg, "\""))
  {
    if (dst) {
      memcpy(dst, arg, n);
      dst[n] = '"';
      dst[n+1] = arg[n];
      dst = &dst[n+2];
    }
    bytec += n + 2;
    arg = &arg[n+1];
  }
  if (dst) {
    memcpy(dst, arg, n);
    dst[n] = '"';
    dst[n+1] = '\0';
  }
  bytec += n + 2;
  return bytec;
}

char** fildesh_tool_escape_argv_for_windows(const char* const* argv) {
  char** dstv;
  unsigned argc = 0;
  size_t bytec = 0;
  for (; argv[argc]; ++argc) {
    bytec += escape_allocated_argv_for_windows(NULL, argv[argc]);
  }
  dstv = (char**) malloc(sizeof(char*) * (argc + (size_t)1) + bytec);
  if (!dstv)  return NULL;
  dstv[0] = (char*)(void*)&dstv[argc+1];
  dstv[argc] = NULL;
  bytec = 0;
  for (argc = 0; argv[argc]; ++argc) {
    dstv[argc] = &dstv[0][bytec];
    bytec += escape_allocated_argv_for_windows(dstv[argc], argv[argc]);
  }
  return dstv;
}
#endif

static
  int
fildesh_tool_spawn_expect(char** argv, int expect) {
  int istat = 70;
#ifdef _MSC_VER
  char** escaped_argv = fildesh_tool_escape_argv_for_windows((const char**)argv);
  intptr_t pid;
  if (!escaped_argv)  return 71;
  pid = _spawnvp(_P_NOWAIT, argv[0], escaped_argv);
  free(escaped_argv);
  if (pid < 0) {
    return (expect == 0) ? 126 : 65;
  }
#else
  pid_t pid = fork();
  if (pid < 0)  return 71;  /* EX_OSERR: Can't fork.*/
  if (pid == 0) {
    execvp(argv[0], argv);
    if (expect != 0)  exit(0);
    exit(126);
  }
#endif

#ifndef UNIT_TESTING
  fclose(stdin);
  fclose(stdout);
  fclose(stderr);
#endif

#ifdef _MSC_VER
  pid = _cwait(&istat, pid, 0);
#else
  pid = waitpid(pid, &istat, 0);
#endif
  if (pid < 0)  return 70;  /* EX_SOFTWARE: Internal software error.*/

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

#ifndef UNIT_TESTING
#define fildesh_tool_spawn_main main
#endif
int fildesh_tool_spawn_main(int argc, char** argv) {
  if (argc < 2 || !argv[1]) {
    return 64;  /* EX_USAGE: Command line usage error.*/
  }
  if (argv[1][0] == '!' && argv[1][1] == '\0') {
    if (argc == 2) {
      return 64;  /* `spawn !` acts like `false`, mimicking `bash -c "!"`.*/
    }
    if (argc == 3 && argv[2][0] == '!' && argv[2][1] == '\0') {
      return 0;  /* `spawn ! !` acts like `true`, mimicking `bash -c "! !"`.*/
    }
    return fildesh_tool_spawn_expect(&argv[2], -1);
  }

  /* Just run the command.*/
#if defined(UNIT_TESTING) || defined(_MSC_VER)
  /* Exec does not seem to propagate exit status on Windows, so use Spawn.*/
  return fildesh_tool_spawn_expect(&argv[1], 0);
#else
  execvp(argv[1], &argv[1]);
  return 126;
#endif
}
