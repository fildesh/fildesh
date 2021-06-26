/**
 * Convert a time in the format DD:HH:MM:SS to seconds.
 * The longer time intervals like days or hours need not be present
 * for this to work properly. Field widths are not fixed either.
 */
#include "lace.h"

#include <stdio.h>
#include <string.h>

static unsigned conv_line(LaceX* in)
{
  unsigned i = 0;
  unsigned m;
  unsigned a[4];
  unsigned x = 0;

  for (i = 0; i < 4; ++i) {
    LaceX slice = slicechr_LaceX(in, ':');
    int tmp_int = -1;
    if (parse_int_LaceX(&slice, &tmp_int) && tmp_int >= 0) {
      a[i] = (unsigned) tmp_int;
    } else {
      break;
    }
  }
  if (i == 0)  return x;
  m = 1;
  x += m * a[--i];
  if (i == 0)  return x;
  m *= 60;
  x += m * a[--i];
  if (i == 0)  return x;
  m *= 60;
  x += m * a[--i];
  if (i == 0)  return x;
  m *= 24;
  x += m * a[--i];
  return x;
}

  int
main_time2sec(int argi, int argc, char** argv)
{
  LaceX* in = NULL;
  LaceO* out = NULL;
  LaceX slice;
  unsigned width = 0;

  if (argi < argc && 0 == strcmp("-w", argv[argi])) {
    ++ argi;
    if (argi < argc) {
      int tmp_int = -1;
      if (lace_parse_int(&tmp_int, argv[argi++]) && tmp_int >= 0) {
        width = (unsigned) tmp_int;
      } else {
        return 64;
      }
    }
  }

  if (argi < argc) {
    fputs("Just run without arguments and type numbers.\n", stderr);
    fputs("You'll figure it out.\n", stderr);
    return 64;
  }

  in = open_LaceXF("-");
  out = open_LaceOF("-");

  for (slice = sliceline_LaceX(in);
       slice.size > 0;
       slice = sliceline_LaceX(in))
  {
    char buf[20];
    unsigned x = conv_line(&slice);
    unsigned n;
    sprintf(buf, "%u", x);
    for (n = strlen(buf); n < width; ++n) {
      putc_LaceO(out, ' ');
    }
    print_int_LaceO(out, x);
    putc_LaceO(out, '\n');
  }
  close_LaceX(in);
  close_LaceO(out);
  return 0;
}

#ifndef MAIN_LACE_EXECUTABLE
int main(int argc, char** argv) {
  return main_time2sec(1, argc, argv);
}
#endif
