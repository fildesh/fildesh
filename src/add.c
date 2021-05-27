
#include "lace.h"
#ifdef MAIN_LACE_EXECUTABLE
#include "utilace.h"
#endif

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

#ifdef MAIN_LACE_EXECUTABLE
LaceUtilMain(add)
#else
int main(int argc, const char** argv)
#endif
{
#ifndef MAIN_LACE_EXECUTABLE
  int argi = 1;
#endif
  LaceXF infile = DEFAULT_LaceXF;
  LaceOF outfile = DEFAULT_LaceOF;
  LaceX* const in = &infile.base;
  LaceO* const out = &outfile.base;
  LaceX slice;

  (void) argv;
  if (argi < argc)
  {
    fputs("Just run without arguments and type numbers.\n", stderr);
    fputs("You'll figure it out.\n", stderr);
#ifdef MAIN_LACE_EXECUTABLE
    lose_sysCx();
#endif
    return 1;
  }

  open_LaceXF(&infile, "-");
  open_LaceOF(&outfile, "-");

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
#ifdef MAIN_LACE_EXECUTABLE
  lose_sysCx ();
#endif
  return 0;
}

