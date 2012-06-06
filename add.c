
#include "cx/fileb.h"
#include "cx/sys-cx.h"

static real
sum_line (FileB* f)
{
    real x = 0, y = 0;
    while (load_real_FileB (f, &y))
        x += y;
    f->good = true;
    skipds_FileB (f, 0);
    if (cstr_FileB (f) [0] != '\0')
        fputs ("Line is no good!\n", stderr);
    return x;
}

int main ()
{
    DecloStack( FileB, f );
    FILE* in = stdin;
    FILE* out = stdout;
    init_sys_cx ();

    init_FileB (f);
    f->f = in;
    f->byline = true;

    while (getline_FileB (f))
    {
        FileB olay;
        real x;

        init_FileB (&olay);
        olay_FileB (&olay.xo, f);
        x = sum_line (&olay);
        fprintf (out, "%f\n", x);
        fflush (out);
    }

    lose_sys_cx ();
    return 0;
}

