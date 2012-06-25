
#include "cx/def.h"
#include "cx/sys-cx.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

int main (int argc, char** argv)
{
    const char* ExeName = argv[0];
    FILE* in = stdin;
    FILE* ErrOut = stderr;
    int argi = 1;
    bool good = true;

    init_sys_cx ();
    BInit();

    BCasc( argi < argc, good, 0 );

    if (0 == strcmp (argv[argi], "-x"))
    {
        ++ argi;
        if (argi < argc)
            in = fopen (argv[argi], "rb");

        ++ argi;
    }

    BCasc( in, good, "File open." );

    BCasc( argi < argc, good, 0 );

    if (0 == strcmp (argv[argi], "--"))  ++ argi;

    BCasc( argi < argc, good, "Need a command!" );

    while (! feof (in) && ! ferror (in))  fgetc (in);
    fclose (in);

    execvp (argv[argi], &argv[argi]);

    fprintf (ErrOut, "%s - Failed to execute:%s\n", ExeName, argv[2]);

    BLose();

    fprintf (ErrOut, "Usage: %s [-x IN] [--] COMMAND [ARG...]\n", ExeName);

    lose_sys_cx ();
    return 1;
}

