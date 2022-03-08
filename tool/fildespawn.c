
#include <string.h>
#include "fildesh_compat_fd.h"

int fildesh_tool_spawn_main(unsigned argc, char** argv) {
  static const char stdin_open_pfx[] = "stdin=open_readonly:";
  static const char stdout_open_pfx[] = "stdout=open_writeonly:";
  static const char stderr_open_pfx[] = "stderr=open_writeonly:";
  static const size_t stdin_open_len = sizeof(stdin_open_pfx)-1;
  static const size_t stdout_open_len = sizeof(stdout_open_pfx)-1;
  static const size_t stderr_open_len = sizeof(stderr_open_pfx)-1;
  static const char stdin_claim_0[] = "stdin=claim:0";
  static const char stdout_claim_1[] = "stdout=claim:1";

  fildesh_compat_fd_t stdin_fd = -1;
  fildesh_compat_fd_t stdout_fd = -1;
  fildesh_compat_fd_t stderr_fd = 2;
  unsigned argi = 1;
  int exstatus = 0;

  for (; argi < argc; ++argi) {
    const char* arg = argv[argi];
    if (0 == strcmp(arg, "--")) {
      ++argi;
      break;
    }
    if (strlen(arg) > stdin_open_len &&
        0 == memcmp(arg, stdin_open_pfx, stdin_open_len)) {
      stdin_fd = fildesh_compat_file_open_readonly(&arg[stdin_open_len]);
      if (stdin_fd < 0) {
        exstatus = 66;  /* EX_NOINPUT: Could not open input file.*/
        break;
      }
    }
    else if (strlen(arg) > stdout_open_len &&
        0 == memcmp(arg, stdout_open_pfx, stdout_open_len)) {
      stdout_fd = fildesh_compat_file_open_writeonly(&arg[stdout_open_len]);
      if (stdout_fd < 0) {
        exstatus = 73;  /* EX_CANTCREAT: Cannot create output file.*/
        break;
      }
    }
    else if (strlen(arg) > stderr_open_len &&
        0 == memcmp(arg, stderr_open_pfx, stderr_open_len)) {
      stderr_fd = fildesh_compat_file_open_writeonly(&arg[stderr_open_len]);
      if (stderr_fd < 0) {
        exstatus = 73;  /* EX_CANTCREAT: Cannot create output file.*/
        break;
      }
    }
    else if (0 == strcmp(arg, stdin_claim_0)) {
      stdin_fd = fildesh_compat_fd_claim(0);
    }
    else if (0 == strcmp(arg, stdout_claim_1)) {
      stdout_fd = fildesh_compat_fd_claim(1);
    }
    else {
      exstatus = 64;  /* EX_USAGE: Command line usage error.*/
      break;
    }
  }

  if (argi >= argc) {
    exstatus = 64;  /* EX_USAGE: Command line usage error.*/
  }

  if (exstatus == 0) {
    exstatus = fildesh_compat_fd_spawnvp_wait(
        stdin_fd, stdout_fd, stderr_fd, NULL, (const char**)&argv[argi]);
    if (exstatus < 0) {
      exstatus = 126;  /* Cannot execute program.*/
    }
  }
  else {
    if (stdin_fd >= 0) {fildesh_compat_fd_close(stdin_fd);}
    if (stdout_fd >= 0) {fildesh_compat_fd_close(stdout_fd);}
    if (stderr_fd >= 0) {fildesh_compat_fd_close(stderr_fd);}
  }

  return exstatus;
}

int main(int argc, char** argv) {
  return fildesh_tool_spawn_main((unsigned) argc, argv);
}
