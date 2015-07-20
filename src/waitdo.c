
#include "utilace.h"
#include "cx/def.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

LaceUtilMain(waitdo)
{
  const char* ExeName = argv[0];
  FILE* in = stdin;
  FILE* ErrOut = stderr;
  DeclLegit( good );

  DoLegitLine( "Need a command!" )
    argi < argc;

  DoLegitP( in, "File open." )
  {
    if (0 == strcmp (argv[argi], "-x"))
    {
      ++ argi;
      if (argi < argc)
        in = fopen (argv[argi], "rb");

      ++ argi;
    }
  }

  DoLegitLine( "Need a command!" )
    argi < argc;

  DoLegitP( argi < argc, "Need a command!" )
  {
    if (0 == strcmp (argv[argi], "--"))  ++ argi;
  }

  if (good)
  {
    while (! feof (in) && ! ferror (in))  fgetc (in);
    fclose (in);

    {
      int ret = lace_util_main (argi, argc, argv);
      if (ret >= 0)
        return ret;
    }
    execvp_sysCx (&argv[argi]);

    fprintf (ErrOut, "%s - Failed to execute:%s\n", ExeName, argv[2]);
  }

  fprintf (ErrOut, "Usage: %s [-x IN] [--] COMMAND [ARG...]\n", ExeName);

  lose_sysCx ();
  return 1;
}

