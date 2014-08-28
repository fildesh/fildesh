/**
 * Convert a time in the format DD:HH:MM:SS to seconds.
 * The longer time intervals like days or hours need not be present
 * for this to work properly. Field widths are not fixed either.
 */
#include "cx/syscx.h"
#include "cx/fileb.h"

static uint
conv_line (XFile* xf)
{
  uint i = 0;
  uint m;
  uint a[4];
  uint x = 0;
  skipds_XFile (xf, 0);
  while (i < ArraySz(a) && xget_uint_XFile (xf, &a[i]))
  {
    skipds_XFile (xf, ":");
    ++i;
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

int main (int argc, char** argv)
{
  int argi =
    (init_sysCx (&argc, &argv),
     1);
  XFile* xf;
  OFile* of;
  char* s;
  uint width = 1;

  if (argi < argc && eq_cstr ("-w", argv[argi])) {
    Bool good = false;
    ++ argi;
    if (argi < argc) {
      if (xget_uint_cstr (&width, argv[argi++])) {
        good = true;
      }
    }
    if (!good) {
      failout_sysCx ("-w needs a numerical width!");
    }
  }

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
    uint x;
    XFile olay[1];
    olay_txt_XFile (olay, xf, IdxEltTable( xf->buf, s ));
    x = conv_line (olay);
    printf_OFile (of, "%*u\n", width, x);
    flush_OFile (of);
  }

  lose_sysCx ();
  return 0;
}

