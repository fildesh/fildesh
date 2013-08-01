
#include "cx/syscx.h"
#include "cx/ofile.h"

#include <stdio.h>
#include <stdlib.h>

int main (int argc, char** argv)
{
  int argi =
    (init_sysCx (&argc, &argv),
     1);

  if (argc < 3)
  {
    printf_OFile (stderr_OFile (),
                  "Usage: %s PATH COMMAND [ARG...]\n",
                  exename_of_sysCx ());
    failout_sysCx ("Exiting...");
  }

  if (!chdir_sysCx (argv[argi]))
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

