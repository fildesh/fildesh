
    /** This program functions somewhat like /xargs/ but simply forks a
     * process for each different line of input and forwards that line of input
     * to the forked process' stdin.
     **/

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <errno.h>
#include <unistd.h>
#include <sys/wait.h>

    /** Bail out.**/
static void
errxit (const char* msg)
{
    FILE* errout = stderr;
    fprintf (errout, "%s: %s\n", msg, strerror (errno));
    exit (1);
}

int main(int argc, char** argv)
{
    FILE* errout = stderr;
    const int n = 8192;
    char buf[8192];
    int argoff = 1;
    int ret;

    if (argc <= 1)
    {
        fprintf (errout, "%s: Need at least one argument.\n", argv[0]);
        return 1;
    }

    while (fgets (buf, n, stdin))
    {
        pid_t pid;
        int io[2];

        ret = pipe (io);
        if (ret < 0)  errxit ("Pipe failed!");

        pid = fork ();
        if (pid > 0)
        {
            int status = 0;
            close (io[0]);
            write (io[1], buf, strlen (buf) * sizeof (char));
            close (io[1]);
            ret = waitpid (pid, &status, 0);
            if (ret < 0)  errxit ("Wait failed!");
            if (status != 0)
            {
                fprintf (errout, "Child exited with status:%d, exiting!\n",
                         status);
                exit (status);
            }
            continue;
        }

        if (pid != 0)  errxit ("Fork failed!");

        close  (io[1]);
        dup2 (io[0], 0);
        execvp (argv[argoff], &argv[argoff]);
        errxit ("Exec failed!");
    }

    return 0;
}

