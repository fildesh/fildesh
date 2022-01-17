
#include "fildesh.h"
#include "fildesh_compat_errno.h"
#include "fildesh_compat_fd.h"
#include "fildesh_compat_sh.h"
#include "fildesh_compat_string.h"

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
  fildesh_compat_fd_t source_fd = -1;
  fildesh_compat_fd_t fd_to_remote = -1;
  FildeshO* to_remote = NULL;
  fildesh_compat_pid_t pid;

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
    puts_FildeshO(to_remote, cmd);
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
  const char* ssh_exe = "ssh";
  char* line;

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

  for (line = getline_FildeshX(in);
       line;
       line = getline_FildeshX(in))
  {
    int q = 0;
    int r = strlen (line);
    while (q < r && strchr(fildesh_compat_string_blank_bytes, line[q]  )) {++q;}
    while (r > q && strchr(fildesh_compat_string_blank_bytes, line[r-1])) {--r;}

    if (r == q)  continue;
    line[r] = '\0';
    line = &line[q];

    spawn_ssh(ssh_exe, argv[argi], line);
  }

  close_FildeshX(in);
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
