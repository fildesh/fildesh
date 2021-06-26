
#include "lace_tool.h"

int lace_tool_vspawnlp(const int* fds_to_close, const char* cmd, va_list argp) {
  const unsigned max_argc = 50;
  const char* argv[51];
  unsigned i;

  if (!cmd) {return -1;}
  argv[0] = cmd;
  for (i = 1; i <= max_argc && argv[i-1]; ++i) {
    argv[i] = va_arg(argp, const char*);
  }
  if (argv[i-1]) {return -1;}
  return lace_tool_spawn_close_expect((char**)argv, fds_to_close, 0);
}

int lace_tool_spawnlp(const int* fds_to_close, const char* cmd, ...) {
  va_list argp;
  int istat;
  va_start(argp, cmd);
  istat = lace_tool_vspawnlp(fds_to_close, cmd, argp);
  va_end(argp);
  return istat;
}
