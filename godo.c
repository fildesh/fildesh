
#include "cx/syscx.h"
#include "cx/fileb.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main (int argc, char** argv)
{
    int argi =
        (init_sysCx (&argc, &argv),
         1);
    int ret;

    if (argc < 3)
    {
        printf_OFileB (stderr_OFileB (),
                       "Usage: %s PATH COMMAND [ARG...]\n",
                       exename_of_sysCx ());
        failout_sysCx ("Exiting...");
    }

    ret = chdir (argv[argi]);
    if (ret != 0)
    {
        DBog1( "Failed to chdir() to: %s", argv[argi] );
        failout_sysCx ("");
    }
    ++ argi;

    execvp_sysCx (&argv[argi]);
    // Flow should not actually get here.
    lose_sysCx ();
    return 1;
}

