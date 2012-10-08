
#include "cx/syscx.h"
#include "cx/fileb.h"

static real
sum_line (XFileB* xf)
{
    real x = 0, y = 0;
    while (xget_real_XFileB (xf, &y))
        x += y;
    skipds_XFileB (xf, 0);
    if (cstr_XFileB (xf) [0] != '\0')
        fputs ("Line is no good!\n", stderr);
    return x;
}

int main (int argc, char** argv)
{
    int argi =
        (init_sysCx (&argc, &argv),
         1);
    XFileB* xf;
    OFileB* of;
    char* s;

    if (argi < argc)
    {
        DBog0( "Just run without arguments and type numbers." );
        DBog0( "You'll figure it out." );
        failout_sysCx ("No arguments expected...");
    }

    xf = stdin_XFileB ();
    of = stdout_OFileB ();

    for (s = getline_XFileB (xf);
         s;
         s = getline_XFileB (xf))
    {
        real x;
        XFileB olay = olay_XFileB (xf, IdxEltTable( xf->buf, s ));
        x = sum_line (&olay);
        printf_OFileB (of, "%f\n", x);
        flush_OFileB (of);
    }

    lose_sysCx ();
    return 0;
}

