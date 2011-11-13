    /** Simple utility to gobble a stream.**/
#include <stdio.h>
int main()
{
    size_t nread;
    FILE* in = stdin;
    fclose (stdout);
    fclose (stderr);
    do
    {
#define N 8192
        char buf[N];
        nread = fread (buf, sizeof(char), N, in);
#undef N
    } while (nread == 0);
    fclose (in);
    return 0;
}

