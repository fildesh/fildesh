
#include "cx/fileb.h"
#include "cx/ospc.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define AssertSafe( cond, msg )  assert ((cond) && (msg))

/** Show usage and exit with a bad status.
 **/
  static void
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
  static void
spawn_ssh (const char* cmd, const char* host)
{
  DeclTable( const_cstr, args );
  OSPc ospc[] = {DEFAULT_OSPc};
  bool good = true;
  uint i;

  ospc->cmd = cons1_AlphaTab ("ssh");
  PushTable( args, "-o" );
  PushTable( args, "StrictHostKeyChecking=no" );
  PushTable( args, "-o" );
  PushTable( args, "PasswordAuthentication=no" );
  PushTable( args, host );
  PushTable( args, "sh" );
  PushTable( args, "-s" );
  PushTable( args, host );

  UFor( i, args.sz ) {
    PushTable( ospc->args, cons1_AlphaTab (args.s[i]) );
  }

  stdxpipe_OSPc (ospc);
  good = spawn_OSPc (ospc);
  AssertSafe( good, "spawn()" );

  oput_cstr_OFile (ospc->of, cmd);
  good = close_OSPc (ospc);
  AssertSafe( good, "close_OSPc()" );

  lose_OSPc (ospc);
  LoseTable( args );
}

  int
main_ssh_all(unsigned argc, char** argv)
{
  unsigned argi = 1;
  LaceX* in = NULL;
  char* line;

  if (argi >= argc)  show_usage_and_exit ();

  in = open_LaceXF(argv[argi]);
  if (!in) {
    DBog1( "File: %s", argv[argi] );
    failout_sysCx ("Cannot open file for reading!");
  }

  ++ argi;
  if (argi >= argc)  show_usage_and_exit ();

  for (line = getline_LaceX(in);
       line;
       line = getline_LaceX(in))
  {
    int q = 0;
    int r = strlen (line);
    while (q < r && strchr (WhiteSpaceChars, line[q]  ))  ++q;
    while (r > q && strchr (WhiteSpaceChars, line[r-1]))  --r;

    if (r == q)  continue;
    line[r] = 0;
    line = &line[q];

    spawn_ssh (argv[argi], line);
  }

  close_LaceX(in);
  return 0;
}

#ifndef LACE_BUILTIN_LIBRARY
  int
main(int argc, char** argv)
{
  int argi = init_sysCx(&argc, &argv);
  int istat = main_ssh_all(argc-(argi-1), &argv[argi-1]);
  lose_sysCx();
  return istat;
}
#endif
