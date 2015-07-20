
#include "utilace.h"
#include "cx/ofile.h"

#include <stdio.h>
#include <stdlib.h>

LaceUtilMain(godo)
{
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

  {
    int ret = lace_util_main (argi, argc, argv);
    if (ret >= 0)
      return ret;
  }
  execvp_sysCx (&argv[argi]);
  // Flow should not actually get here.
  lose_sysCx ();
  return 1;
}

