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
lace_builtin_time2sec_main(unsigned argc, char** argv,
                           LaceX** inputs, LaceO** outputs)
{
  unsigned argi = 1;
  LaceX* in = NULL;
  LaceO* out = NULL;
  LaceX slice;
  unsigned width = 0;

  if (argi < argc && 0 == strcmp("-w", argv[argi])) {
    int tmp_int = -1;
    ++ argi;
    if (fildesh_parse_int(&tmp_int, argv[argi++]) && tmp_int >= 0) {
      width = (unsigned) tmp_int;
    } else {
      return 64;
    }
  }

  if (argi < argc) {
    fputs("Just run without arguments and type numbers.\n", stderr);
    fputs("You'll figure it out.\n", stderr);
    return 64;
  }

  in = open_arg_LaceXF(0, argv, inputs);
  out = open_arg_LaceOF(0, argv, outputs);
  if (!in || !out) {
    close_LaceX(in);
    close_LaceO(out);
    fildesh_log_error("Cannot open stdio!");
    return 1;
  }

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

#ifndef LACE_BUILTIN_LIBRARY
int main(int argc, char** argv) {
  return lace_builtin_time2sec_main(argc, argv, NULL, NULL);
}
#endif
