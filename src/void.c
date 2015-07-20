/** Simple utility to gobble a stream.**/
#include "utilace.h"
#include <stdio.h>

LaceUtilMain(void)
{
  size_t nread;
  FILE* in = stdin;

  (void) argi;
  (void) argc;
  (void) argv;
  fclose (stdout);
  fclose (stderr);
  do
  {
#define N 8192
    char buf[N];
    nread = fread (buf, sizeof(char), N, in);
#undef N
  } while (nread != 0);
  fclose (in);
  lose_sysCx ();
  return 0;
}

