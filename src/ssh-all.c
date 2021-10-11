
#include "lace.h"
#include "lace_compat_errno.h"
#include "lace_compat_fd.h"
#include "lace_compat_sh.h"
#include "lace_compat_string.h"

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
  lace_compat_fd_t source_fd = -1;
  lace_compat_fd_t fd_to_remote = -1;
  LaceO* to_remote = NULL;
  lace_compat_pid_t pid;

  istat = lace_compat_fd_pipe(&fd_to_remote, &source_fd);
  assert(istat == 0);
  to_remote = open_fd_LaceO(fd_to_remote);

  pid = lace_compat_fd_spawnlp(
      source_fd, 1, 2, NULL,
      ssh_exe,
      "-o", "StrictHostKeyChecking=no",
      "-o", "PasswordAuthentication=no",
      host,
      "sh", "-s", host,
      NULL);

  if (pid >= 0) {
    puts_LaceO(to_remote, cmd);
  }
  close_LaceO(to_remote);
  if (pid >= 0) {
    istat = lace_compat_sh_wait(pid);
    if (istat < 0) {lace_compat_errno_trace();}
  }
}

  int
main_ssh_all(unsigned argc, char** argv)
{
  unsigned argi = 1;
  LaceX* in = NULL;
  const char* ssh_exe = "ssh";
  char* line;

  if (argv[argi] && 0 == strcmp(argv[argi], "-ssh")) {
    argi += 1;
    ssh_exe = argv[argi];
    argi += 1;
  }
  if (argi >= argc)  show_usage_and_exit ();

  in = open_LaceXF(argv[argi]);
  if (!in) {
    fildesh_log_errorf("Cannot open file: %s", argv[argi]);
    return 1;
  }

  argi += 1;
  if (argi + 1 != argc)  show_usage_and_exit ();

  for (line = getline_LaceX(in);
       line;
       line = getline_LaceX(in))
  {
    int q = 0;
    int r = strlen (line);
    while (q < r && strchr(lace_compat_string_blank_bytes, line[q]  )) {++q;}
    while (r > q && strchr(lace_compat_string_blank_bytes, line[r-1])) {--r;}

    if (r == q)  continue;
    line[r] = '\0';
    line = &line[q];

    spawn_ssh(ssh_exe, argv[argi], line);
  }

  close_LaceX(in);
  return 0;
}

#ifndef LACE_BUILTIN_LIBRARY
  int
main(int argc, char** argv)
{
  int istat = main_ssh_all(argc, argv);
  return istat;
}
#endif
