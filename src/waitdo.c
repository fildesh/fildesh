
#include "cx/syscx.h"
#include "cx/def.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main (int argc, char** argv)
{
  int argi =
    (init_sysCx (&argc, &argv),
     1);
  const char* ExeName = argv[0];
  FILE* in = stdin;
  FILE* ErrOut = stderr;
  bool good = true;

  if (LegitCk( argi < argc, good, 0 ))
  {
    if (0 == strcmp (argv[argi], "-x"))
    {
      ++ argi;
      if (argi < argc)
        in = fopen (argv[argi], "rb");

      ++ argi;
    }
  }
  if (LegitCk( in, good, "File open." ))
  {}
  if (LegitCk( argi < argc, good, 0 ))
  {
    if (0 == strcmp (argv[argi], "--"))  ++ argi;
  }
  if (LegitCk( argi < argc, good, "Need a command!" ))
  {
    while (! feof (in) && ! ferror (in))  fgetc (in);
    fclose (in);

    execvp_sysCx (&argv[argi]);

    fprintf (ErrOut, "%s - Failed to execute:%s\n", ExeName, argv[2]);
  }

  fprintf (ErrOut, "Usage: %s [-x IN] [--] COMMAND [ARG...]\n", ExeName);

  lose_sysCx ();
  return 1;
}

