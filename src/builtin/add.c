
#include "fildesh.h"

#include <stdio.h>
#include <string.h>

static int
sum_int_line (FildeshX* in)
{
  int x = 0, y = 0;
  while (parse_int_FildeshX(in, &y))
    x += y;
  skipchrs_FildeshX(in, " \t");
  if (in->off < in->size) {
    fputs ("Line is no good!\n", stderr);
  }
  return x;
}

static double
sum_real_line(FildeshX* in)
{
  double x = 0, y = 0;
  while (parse_double_FildeshX(in, &y))
    x += y;
  skipchrs_FildeshX(in, " \t");
  if (in->off < in->size) {
    fputs ("Line is no good!\n", stderr);
  }
  return x;
}

  int
fildesh_builtin_add_main(unsigned argc, char** argv,
                      FildeshX** inputv, FildeshO** outputv)
{
  FildeshX* in = NULL;
  FildeshO* out = NULL;
  FildeshX slice;

  (void) argv;
  if (argc > 1)
  {
    fputs("Just run without arguments and type numbers.\n", stderr);
    fputs("You'll figure it out.\n", stderr);
    return 64;
  }

  in = open_arg_FildeshXF(0, argv, inputv);
  out = open_arg_FildeshOF(0, argv, outputv);
  if (!in || !out) {
    close_FildeshX(in);
    close_FildeshO(out);
    fputs("Cannot open stdio!\n", stderr);
    return 1;
  }

  for (slice = sliceline_FildeshX(in);
       slice.size > 0;
       slice = sliceline_FildeshX(in)) {
    if (slice.size == strcspn(slice.at, ".Ee")) {
      int x = sum_int_line(&slice);
      print_int_FildeshO(out, x);
      putc_FildeshO(out, '\n');
    }
    else {
      double x = sum_real_line(&slice);
      print_double_FildeshO(out, x);
      putc_FildeshO(out, '\n');
    }
    flush_FildeshO(out);
  }

  close_FildeshX(in);
  close_FildeshO(out);
  return 0;
}

#ifndef FILDESH_BUILTIN_LIBRARY
int main(int argc, char** argv) {
  return fildesh_builtin_add_main(argc, argv, NULL, NULL);
}
#endif