
#include "lace.h"
#include "utilace.h"
#include "cx/fileb.h"

static int
sum_int_line (LaceX* in)
{
  int x = 0, y = 0;
  while (parse_int_LaceX(in, &y))
    x += y;
  skipchrs_LaceX(in, WhiteSpaceChars);
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
  skipchrs_LaceX(in, WhiteSpaceChars);
  if (in->off < in->size) {
    fputs ("Line is no good!\n", stderr);
  }
  return x;
}

LaceUtilMain(add)
{
  LaceXF xf[1] = {DEFAULT_LaceXF};
  LaceX slice;
  OFile* ofile;

  (void) argv;
  if (argi < argc)
  {
    DBog0( "Just run without arguments and type numbers." );
    DBog0( "You'll figure it out." );
    failout_sysCx ("No arguments expected...");
  }

  open_LaceXF(xf, "-");
  ofile = stdout_OFile ();

  for (slice = sliceline_LaceX(&xf->base);
       slice.size > 0;
       slice = sliceline_LaceX(&xf->base)) {
    if (slice.size == strcspn(slice.at, ".Ee")) {
      int x = sum_int_line(&slice);
      oput_int_OFile (ofile, x);
      oput_char_OFile (ofile, '\n');
    }
    else {
      double x = sum_real_line(&slice);
      printf_OFile (ofile, "%f\n", x);
    }
    flush_OFile (ofile);
  }

  close_LaceX(&xf->base);
  lose_sysCx ();
  return 0;
}

