#include "lace.h"
#include <stdlib.h>

int lace_builtin_zec_main(
    unsigned argc, char** argv, LaceX** inputv, LaceO** outputv);

int main(int argc, char** argv) {
  unsigned zec_argc = 4 + 2*(argc-1) + 3;
  const char** zec_argv = (const char**) malloc(sizeof(char*) * (zec_argc+1));
  int i, istat;
  zec_argv[0] = argv[0];
  zec_argv[1] = "-o";
  zec_argv[2] = "-";
  zec_argv[3] = "--";
  zec_argv[4] = "/";
  for (i = 0; i < argc-1; ++i) {
    zec_argv[5+2*i] = argv[1+i];
    if (i+1 < argc-1) {
      zec_argv[5+2*i+1] = " ";
    } else {
      zec_argv[5+2*i+1] = "\n";
    }
  }
  zec_argv[4+2*(argc-1)+1] = "/";
  zec_argv[4+2*(argc-1)+2] = "-";
  zec_argv[4+2*(argc-1)+3] = NULL;
  istat = lace_builtin_zec_main(zec_argc, (char**)zec_argv, NULL, NULL);
  free(zec_argv);
  return istat;
}

