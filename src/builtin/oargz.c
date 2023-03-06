/** Write args to stdout separated by NUL bytes.**/
#include <fildesh/fildesh.h>

  int
fildesh_builtin_oargz_main(
    unsigned argc, char** argv,
    FildeshX** inputv, FildeshO** outputv)
{
  FildeshO* out = open_arg_FildeshOF(0, argv, outputv);
  unsigned argi = 1;
  (void) inputv;
  if (argv[argi] &&
      argv[argi][0] == '-' && argv[argi][1] == '-' && argv[argi][2] == '\0')
  {
    argi += 1;
  }
  for (; argi < argc; ++argi) {
    puts_FildeshO(out, argv[argi]);
    if (argi + 1 < argc) {
      putc_FildeshO(out, '\0');
    }
  }
  close_FildeshO(out);
  return 0;
}

#ifndef FILDESH_BUILTIN_LIBRARY
int main(int argc, char** argv) {
  return fildesh_builtin_oargz_main(argc, argv, NULL, NULL);
}
#endif
