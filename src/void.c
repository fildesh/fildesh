/** Simple utility to gobble a stream.**/
#include "cx/syscx.h"
#include <stdio.h>

  int
main_void(int argi, int argc, char** argv)
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
  return 0;
}

#ifndef MAIN_LACE_EXECUTABLE
  int
main(int argc, char** argv)
{
  int argi = init_sysCx(&argc, &argv);
  int istat = main_void(argi, argc, argv);
  lose_sysCx();
  return istat;
}
#endif
