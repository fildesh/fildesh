
#include "cx/syscx.h"
#include "cx/fileb.h"

static real
sum_line (XFile* xf)
{
  real x = 0, y = 0;
  while (xget_real_XFile (xf, &y))
    x += y;
  skipds_XFile (xf, 0);
  if (cstr_XFile (xf) [0] != '\0')
    fputs ("Line is no good!\n", stderr);
  return x;
}

int main (int argc, char** argv)
{
  int argi = init_sysCx (&argc, &argv);
  XFile* xf;
  OFile* of;
  char* s;

  if (argi < argc)
  {
    DBog0( "Just run without arguments and type numbers." );
    DBog0( "You'll figure it out." );
    failout_sysCx ("No arguments expected...");
  }

  xf = stdin_XFile ();
  of = stdout_OFile ();

  for (s = getline_XFile (xf);
       s;
       s = getline_XFile (xf))
  {
    real x;
    XFile olay[1];
    olay_txt_XFile (olay, xf, IdxEltTable( xf->buf, s ));
    x = sum_line (olay);
    printf_OFile (of, "%f\n", x);
    flush_OFile (of);
  }

  lose_sysCx ();
  return 0;
}

