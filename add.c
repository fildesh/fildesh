
#include "cx/fileb.h"
#include "cx/sys-cx.h"

static real
sum_line (XFileB* xf)
{
    real x = 0, y = 0;
    while (load_real_XFileB (xf, &y))
        x += y;
    skipds_XFileB (xf, 0);
    if (cstr_XFileB (xf) [0] != '\0')
        fputs ("Line is no good!\n", stderr);
    return x;
}

int main ()
{
    XFileB* xf;
    OFileB* of;
    char* s;

    init_sys_cx ();

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

    lose_sys_cx ();
    return 0;
}

