
#include "utilace.h"
#include "cx/ofile.h"

#include <stdio.h>
#include <stdlib.h>

  int
main_godo(int argi, int argc, char** argv)
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

  if (lace_specific_util(argv[argi]))
  {
    return lace_util_main (argi, argc, argv);
  }
  execvp_sysCx (&argv[argi]);
  /* Flow should not actually get here. */
  return 1;
}

#ifndef MAIN_LACE_EXECUTABLE
  int
main(int argc, char** argv)
{
  int argi = init_sysCx(&argc, &argv);
  int istat = main_godo(argi, argc, argv);
  lose_sysCx();
  return istat;
}
#endif
