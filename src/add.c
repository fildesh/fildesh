
#include "utilace.h"
#include "cx/fileb.h"

static int
sum_int_line (XFile* xf)
{
  int x = 0, y = 0;
  while (xget_int_XFile (xf, &y))
    x += y;
  skipds_XFile (xf, 0);
  if (ccstr_of_XFile (xf) [0] != '\0')
    fputs ("Line is no good!\n", stderr);
  return x;
}

static real
sum_real_line (XFile* xf)
{
  real x = 0, y = 0;
  while (xget_real_XFile (xf, &y))
    x += y;
  skipds_XFile (xf, 0);
  if (ccstr_of_XFile (xf) [0] != '\0')
    fputs ("Line is no good!\n", stderr);
  return x;
}

LaceUtilMain(add)
{
  XFile* xfile;
  OFile* ofile;
  XFile olay[1];

  (void) argv;
  if (argi < argc)
  {
    DBog0( "Just run without arguments and type numbers." );
    DBog0( "You'll figure it out." );
    failout_sysCx ("No arguments expected...");
  }

  xfile = stdin_XFile ();
  ofile = stdout_OFile ();

  while (getlined_olay_XFile (olay, xfile, 0)) {
    const char* line = ccstr_of_XFile (olay);
    if (!line[strcspn (line, ".Ee")]) {
      int x = sum_int_line (olay);
      oput_int_OFile (ofile, x);
      oput_char_OFile (ofile, '\n');
    }
    else {
      real x = sum_real_line (olay);
      printf_OFile (ofile, "%f\n", x);
    }
    flush_OFile (ofile);
  }

  lose_sysCx ();
  return 0;
}

