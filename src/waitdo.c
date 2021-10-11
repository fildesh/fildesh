
#include "utilace.h"
#include "fildesh_compat_sh.h"
#include "cx/syscx.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

  int
main_waitdo(unsigned argc, char** argv)
{
  const char* ExeName = argv[0];
  FILE* in = stdin;
  FILE* ErrOut = stderr;
  unsigned argi = 1;
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

    if (lace_specific_util(argv[argi]))
    {
      return lace_builtin_main(argv[argi], argc-argi, &argv[argi]);
    }
    lace_compat_sh_exec((const char**)&argv[argi]);

    fprintf (ErrOut, "%s - Failed to execute:%s\n", ExeName, argv[2]);
  }

  fprintf (ErrOut, "Usage: %s [-x IN] [--] COMMAND [ARG...]\n", ExeName);
  return 1;
}

#ifndef LACE_BUILTIN_LIBRARY
  int
main(int argc, char** argv)
{
  return main_waitdo((unsigned)argc, argv);
}
#endif
