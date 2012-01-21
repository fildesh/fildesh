
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main (int argc, char** argv)
{
    int ret;
    const char* ExeName = argv[0];
    FILE* ErrOut = stderr;

    if (argc < 3)
    {
        fprintf (ErrOut, "Usage: %s PATH COMMAND [ARG...]\n", ExeName);
        exit (1);
    }

    ret = chdir (argv[1]);
    if (ret != 0)
    {
        fprintf (ErrOut, "%s - Failed to chdir() to:%s\n", ExeName, argv[1]);
        exit (1);
    }

    execvp (argv[2], &argv[2]);
    fprintf (ErrOut, "%s - Failed to execute:%s\n", ExeName, argv[2]);
    return 1;
}

