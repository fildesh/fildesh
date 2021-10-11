
/** This program functions somewhat like /xargs/ but simply spawns a
 * process for each different line of input and forwards that line of input
 * to the spawned process' stdin.
 **/

#include "fildesh.h"
#include "fildesh_compat_errno.h"
#include "fildesh_compat_fd.h"
#include "fildesh_compat_sh.h"
#include "utilace.h"
#include <stdlib.h>


static
  void
run_with_line(const char* lace_exe, unsigned argc, const char** argv,
              const char* line)
{
  /* We create a new stdin for the spawned process.
   * It inherits stdout too, but we want to reuse it.
   */
  lace_compat_fd_t source_fd = -1;
  const char** actual_argv;
  int istat;
  unsigned offset = 0;
  unsigned argi = 0;
  FildeshO* to_spawned = NULL;
  lace_compat_pid_t pid;

  {
    lace_compat_fd_t to_spawned_fd = -1;
    istat = lace_compat_fd_pipe(&to_spawned_fd, &source_fd);
    if (istat != 0) {lace_compat_errno_trace(); return;}
    to_spawned = open_fd_FildeshO(to_spawned_fd);
    if (!to_spawned) {return;}
  }

  actual_argv = (const char**)malloc(sizeof(char*) * (2+argc+1));
  if (lace_specific_util(argv[0])) {
    actual_argv[offset++] = lace_exe;
    actual_argv[offset++] = "-as";
  }
  while (argi < argc) {
    actual_argv[offset++] = argv[argi++];
  }
  actual_argv[offset] = NULL;

  pid = lace_compat_fd_spawnvp(source_fd, 1, 2, NULL, actual_argv);
  free(actual_argv);
  if (pid >= 0) {
    puts_FildeshO(to_spawned, line);
    putc_FildeshO(to_spawned, '\n');
  } else {
    fildesh_log_error("Spawn failed.");
  }
  close_FildeshO(to_spawned);
  if (pid >= 0) {
    istat = lace_compat_sh_wait(pid);
    if (istat != 0) {
      lace_compat_errno_trace();
      fildesh_log_errorf("Child (%s) exited with status: %d", actual_argv[0], istat);
    }
  }
}

  int
main_xpipe(unsigned argc, char** argv)
{
  FildeshX* in = NULL;
  const char* s;
  unsigned argi = 1;

  if (argi >= argc) {
    fildesh_log_error("Need at least one argument.");
    return 64;
  }

  in = open_FildeshXF("-");
  if (!in) {
    fildesh_log_error("Cannot open stdin.");
    return 1;
  }

  for (s = getline_FildeshX(in); s; s = getline_FildeshX(in)) {
    run_with_line(argv[0], argc - argi, (const char**)&argv[argi], s);
  }

  close_FildeshX(in);
  return 0;
}
