
#include "cx/syscx.h"
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
    OFileB* of = stderr_OFileB ();
    printf_OFileB (of, "Usage: %s FILE COMMAND\n", exename_of_sysCx ());
#define f( s )  oput_cstr_OFileB (of, s); oput_char_OFileB (of, '\n')
    f( "  Where /FILE/ contains a machine name on each line." );
    f( "  It can be '-' for stdin." );
    f( "  The /COMMAND/ is processed by /bin/sh, so escape it!" );
    failout_sysCx ("Bad args...");
#undef f
}


/** Fork and run an ssh command to /host/.
 * Have it run the command(s) specified by /cmd/ in a POSIX shell.
 **/
static void
spawn_ssh (const char* cmd, const char* host)
{
    DeclTable( const_cstr, args );
    DecloStack1( OSPc, ospc, dflt_OSPc () );
    bool good = true;

    ospc->cmd = cons1_AlphaTab ("ssh");
    PushTable( args, "-o" );
    PushTable( args, "StrictHostKeyChecking=no" );
    PushTable( args, "-o" );
    PushTable( args, "PasswordAuthentication=no" );
    PushTable( args, host );
    PushTable( args, "sh" );
    PushTable( args, "-s" );
    PushTable( args, host );

    { BLoop( i, args.sz )
        PushTable( ospc->args, cons1_AlphaTab (args.s[i]) );
    } BLose()

    stdxpipe_OSPc (ospc);
    good = spawn_OSPc (ospc);
    AssertSafe( good, "spawn()" );

    oput_cstr_OFileB (ospc->of, cmd);
    good = close_OSPc (ospc);
    AssertSafe( good, "close_OSPc()" );

    lose_OSPc (ospc);
    LoseTable( args );
}

int main (int argc, char** argv)
{
    int argi =
        (init_sysCx (&argc, &argv),
         1);
    FileB xfb = dflt_FileB ();
    XFileB* xf = 0;
    char* line;

    if (argi >= argc)  show_usage_and_exit ();

    if (0 == strcmp (argv[argi], "-"))
    {
        xf = stdin_XFileB ();
    }
    else
    {
        bool good = open_FileB (&xfb, 0, argv[argi]);
        if (good)  xf = &xfb.xo;
    }

    if (!xf)
    {
        DBog1( "File: %s", argv[argi] );
        failout_sysCx ("Cannot open file for reading!");
    }

    ++ argi;
    if (argi >= argc)  show_usage_and_exit ();
    
    for (line = getline_XFileB (xf);
         line;
         line = getline_XFileB (xf))
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

    lose_FileB (&xfb);
    lose_sysCx ();
    return 0;
}

