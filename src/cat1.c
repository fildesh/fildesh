    /** Simple utility to cat a single file.**/
#include <stdio.h>
int main(int argc, char** argv)
{
    int i;
    int argi = argc-1;
    size_t nread;
    FILE* in;
    FILE* out = stdout;

    if (argi < 1)
    {
        fprintf (stderr, "%s: Need file argument!\n", argv[0]);
        return 1;
    }

    if (argv[argi][0] == '-' && argv[argi][1] == '\0')
        in = stdin;
    else
        in = fopen (argv[argi], "rb");

    if (!in)
    {
        fprintf (stderr, "%s: Cannot open file! %s\n", argv[0], argv[argi]);
        return 1;
    }

    for (i = 1; i < argi; ++i)
        fputs (argv[i], out);

    do
    {
#define N 8192
        char buf[N];
        nread = fread (buf, sizeof(char), N, in);
        if (nread != 0)
            nread = fwrite (buf, sizeof(char), nread, out);
#undef N
    } while (nread != 0);
    fclose (in);
    return 0;
}

