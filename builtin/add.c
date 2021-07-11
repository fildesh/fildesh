
#include "lace.h"

#include <stdio.h>
#include <string.h>

static int
sum_int_line (LaceX* in)
{
  int x = 0, y = 0;
  while (parse_int_LaceX(in, &y))
    x += y;
  skipchrs_LaceX(in, " \t");
  if (in->off < in->size) {
    fputs ("Line is no good!\n", stderr);
  }
  return x;
}

static double
sum_real_line(LaceX* in)
{
  double x = 0, y = 0;
  while (parse_double_LaceX(in, &y))
    x += y;
  skipchrs_LaceX(in, " \t");
  if (in->off < in->size) {
    fputs ("Line is no good!\n", stderr);
  }
  return x;
}

  int
lace_builtin_add_main(unsigned argc, char** argv,
                      LaceX** inputv, LaceO** outputv)
{
  LaceX* in = NULL;
  LaceO* out = NULL;
  LaceX slice;

  (void) argv;
  if (argc > 1)
  {
    fputs("Just run without arguments and type numbers.\n", stderr);
    fputs("You'll figure it out.\n", stderr);
    return 64;
  }

  in = open_arg_LaceXF(0, argv, inputv);
  out = open_arg_LaceOF(0, argv, outputv);
  if (!in || !out) {
    close_LaceX(in);
    close_LaceO(out);
    fputs("Cannot open stdio!\n", stderr);
    return 1;
  }

  for (slice = sliceline_LaceX(in);
       slice.size > 0;
       slice = sliceline_LaceX(in)) {
    if (slice.size == strcspn(slice.at, ".Ee")) {
      int x = sum_int_line(&slice);
      print_int_LaceO(out, x);
      putc_LaceO(out, '\n');
    }
    else {
      double x = sum_real_line(&slice);
      print_double_LaceO(out, x);
      putc_LaceO(out, '\n');
    }
    flush_LaceO(out);
  }

  close_LaceX(in);
  close_LaceO(out);
  return 0;
}

#ifndef LACE_BUILTIN_LIBRARY
int main(int argc, char** argv) {
  return lace_builtin_add_main(argc, argv, NULL, NULL);
}
#endif
