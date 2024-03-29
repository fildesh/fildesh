
/** This program functions somewhat like /xargs/ but simply spawns a
 * process for each different line of input and forwards that line of input
 * to the spawned process' stdin.
 **/
#include "src/builtin/fildesh_builtin.h"

#include <stdlib.h>

#include "include/fildesh/fildesh_compat_errno.h"
#include "include/fildesh/fildesh_compat_fd.h"
#include "include/fildesh/fildesh_compat_sh.h"

static
  void
run_with_line(const char* fildesh_exe, unsigned argc, const char** argv,
              const char* line)
{
  /* We create a new stdin for the spawned process.
   * It inherits stdout too, but we want to reuse it.
   */
  Fildesh_fd source_fd = -1;
  const char** actual_argv;
  int istat;
  unsigned offset = 0;
  unsigned argi = 0;
  const char* spawned_exe_name = "/dev/null";
  FildeshO* to_spawned = NULL;
  FildeshCompat_pid pid;

  {
    Fildesh_fd to_spawned_fd = -1;
    istat = fildesh_compat_fd_pipe(&to_spawned_fd, &source_fd);
    if (istat != 0) {fildesh_compat_errno_trace(); return;}
    to_spawned = open_fd_FildeshO(to_spawned_fd);
    if (!to_spawned) {return;}
  }

  actual_argv = (const char**)malloc(sizeof(char*) * (2+argc+1));
  if (fildesh_builtin_main_fn_lookup(argv[0])) {
    actual_argv[offset++] = fildesh_exe;
    actual_argv[offset++] = "-as";
  }
  if (argi < argc) {spawned_exe_name = argv[argi];}
  while (argi < argc) {
    actual_argv[offset++] = argv[argi++];
  }
  actual_argv[offset] = NULL;

  pid = fildesh_compat_fd_spawnvp(source_fd, 1, 2, NULL, actual_argv);
  free(actual_argv);
  if (pid >= 0) {
    putstr_FildeshO(to_spawned, line);
    putc_FildeshO(to_spawned, '\n');
  } else {
    fildesh_log_error("Spawn failed.");
  }
  close_FildeshO(to_spawned);
  if (pid >= 0) {
    istat = fildesh_compat_sh_wait(pid);
    if (istat != 0) {
      fildesh_compat_errno_trace();
      fildesh_log_errorf(
          "Child (%s) exited with status: %d", spawned_exe_name, istat);
    }
  }
}

  int
main_xpipe(unsigned argc, char** argv)
{
  FildeshX* in = NULL;
  FildeshX slice;
  FildeshO oss[1] = {DEFAULT_FildeshO};
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

  for (slice = sliceline_FildeshX(in);
       slice.at;
       slice = sliceline_FildeshX(in))
  {
    truncate_FildeshO(oss);
    putslice_FildeshO(oss, slice);
    putc_FildeshO(oss, '\0');
    run_with_line(argv[0], argc - argi, (const char**)&argv[argi], oss->at);
  }

  close_FildeshX(in);
  close_FildeshO(oss);
  return 0;
}
