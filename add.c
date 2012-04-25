
#include "cx/fileb.h"

static real
sum_line (FileB* f)
{
    real x = 0, y = 0;
    while (load_real_FileB (f, &y))
        x += y;
    return x;
}

int main ()
{
    DecloStack( FileB, f );
    FILE* in = stdin;
    FILE* out = stdout;

    init_FileB (f);
    f->f = in;
    f->byline = true;

    while (getline_FileB (f))
    {
        FileB olay;
        real x;

        olay_FileB (&olay, f);
        x = sum_line (&olay);
        fprintf (out, "%f\n", x);
        fflush (out);
    }

    return 0;
}

