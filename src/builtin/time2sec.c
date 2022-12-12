/**
 * Convert a time in the format DD:HH:MM:SS to seconds.
 * The longer time intervals like days or hours need not be present
 * for this to work properly. Field widths are not fixed either.
 */
#include <fildesh/fildesh.h>

#include <stdio.h>
#include <string.h>

static unsigned conv_line(FildeshX* in)
{
  unsigned i = 0;
  unsigned m;
  unsigned a[4];
  unsigned x = 0;

  for (i = 0; i < 4; ++i) {
    FildeshX slice = until_char_FildeshX(in, ':');
    int tmp_int = -1;
    if (parse_int_FildeshX(&slice, &tmp_int) && tmp_int >= 0) {
      a[i] = (unsigned) tmp_int;
    } else {
      break;
    }
    skipstr_FildeshX(in, ":");
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
fildesh_builtin_time2sec_main(unsigned argc, char** argv,
                           FildeshX** inputs, FildeshO** outputs)
{
  unsigned argi = 1;
  FildeshX* in = NULL;
  FildeshO* out = NULL;
  FildeshX slice;
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

  in = open_arg_FildeshXF(0, argv, inputs);
  out = open_arg_FildeshOF(0, argv, outputs);
  if (!in || !out) {
    close_FildeshX(in);
    close_FildeshO(out);
    fildesh_log_error("Cannot open stdio!");
    return 1;
  }

  for (slice = sliceline_FildeshX(in);
       slice.size > 0;
       slice = sliceline_FildeshX(in))
  {
    char buf[20];
    unsigned x = conv_line(&slice);
    unsigned n;
    sprintf(buf, "%u", x);
    for (n = strlen(buf); n < width; ++n) {
      putc_FildeshO(out, ' ');
    }
    print_int_FildeshO(out, x);
    putc_FildeshO(out, '\n');
  }
  close_FildeshX(in);
  close_FildeshO(out);
  return 0;
}

#ifndef FILDESH_BUILTIN_LIBRARY
int main(int argc, char** argv) {
  return fildesh_builtin_time2sec_main(argc, argv, NULL, NULL);
}
#endif
