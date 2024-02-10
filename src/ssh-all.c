
#include <fildesh/fildesh.h>
#include "include/fildesh/fildesh_compat_errno.h"
#include "include/fildesh/fildesh_compat_fd.h"
#include "include/fildesh/fildesh_compat_sh.h"
#include "include/fildesh/fildesh_compat_string.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define AssertSafe( cond, msg )  assert ((cond) && (msg))

/** Show usage and exit with a bad status.
 **/
static
  void
show_usage_and_exit ()
{
#define f( s )  fputs(s "\n", stderr)
  f( "Usage: ssh-all FILE COMMAND" );
  f( "  Where /FILE/ contains a machine name on each line." );
  f( "  It can be '-' for stdin." );
  f( "  The /COMMAND/ is processed by /bin/sh, so escape it!" );
#undef f
  exit(64);
}

/** Fork and run an ssh command to /host/.
 * Have it run the command(s) specified by /cmd/ in a POSIX shell.
 **/
static
  void
spawn_ssh(const char* ssh_exe, const char* cmd, const char* host)
{
  int istat;
  Fildesh_fd source_fd = -1;
  Fildesh_fd fd_to_remote = -1;
  FildeshO* to_remote = NULL;
  FildeshCompat_pid pid;

  istat = fildesh_compat_fd_pipe(&fd_to_remote, &source_fd);
  assert(istat == 0);
  to_remote = open_fd_FildeshO(fd_to_remote);

  pid = fildesh_compat_fd_spawnlp(
      source_fd, 1, 2, NULL,
      ssh_exe,
      "-o", "StrictHostKeyChecking=no",
      "-o", "PasswordAuthentication=no",
      host,
      "sh", "-s", host,
      NULL);

  if (pid >= 0) {
    putstr_FildeshO(to_remote, cmd);
  }
  close_FildeshO(to_remote);
  if (pid >= 0) {
    istat = fildesh_compat_sh_wait(pid);
    if (istat < 0) {fildesh_compat_errno_trace();}
  }
}

  int
main_ssh_all(unsigned argc, char** argv)
{
  unsigned argi = 1;
  FildeshX* in = NULL;
  FildeshX slice;
  FildeshO oss[1] = {DEFAULT_FildeshO};
  const char* ssh_exe = "ssh";

  if (argv[argi] && 0 == strcmp(argv[argi], "-ssh")) {
    argi += 1;
    ssh_exe = argv[argi];
    argi += 1;
  }
  if (argi >= argc)  show_usage_and_exit ();

  in = open_FildeshXF(argv[argi]);
  if (!in) {
    fildesh_log_errorf("Cannot open file: %s", argv[argi]);
    return 1;
  }

  argi += 1;
  if (argi + 1 != argc)  show_usage_and_exit ();

  for (slice = sliceline_FildeshX(in);
       slice.at;
       slice = sliceline_FildeshX(in))
  {
    while_chars_FildeshX(&slice, fildesh_compat_string_blank_bytes);
    while (avail_FildeshX(&slice) &&
           strchr(fildesh_compat_string_blank_bytes, slice.at[slice.size-1]))
    {
      slice.size -= 1;
    }

    if (!avail_FildeshX(&slice)) {continue;}
    truncate_FildeshO(oss);
    putslice_FildeshO(oss, slice);
    putc_FildeshO(oss, '\0');
    spawn_ssh(ssh_exe, argv[argi], oss->at);
  }

  close_FildeshX(in);
  close_FildeshO(oss);
  return 0;
}

#ifndef FILDESH_BUILTIN_LIBRARY
  int
main(int argc, char** argv)
{
  int istat = main_ssh_all(argc, argv);
  return istat;
}
#endif
