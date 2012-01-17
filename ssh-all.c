
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <sys/wait.h>

#define BigEnuff 4096

#define AllocT( Type, capacity ) \
    (((capacity) == 0) ? (Type*) 0 : \
     (Type*) malloc ((capacity) * sizeof (Type)))

#define ArraySz( a )  sizeof(a) / sizeof(*a)

#define AssertSafe( cond, msg )  assert ((cond) && (msg))

static const char WhiteSpaceChars[] = " \t\v\r\n";

static const char* ExeName = 0;
#define ErrOut stderr

    /** Show usage and exit with a bad status.
     **/
static void
show_usage_and_exit ()
{
    fprintf (ErrOut, "Usage:%s FILE COMMAND\n", ExeName);
    fputs ("  Where /FILE/ contains a machine name on each line.\n", ErrOut);
    fputs ("  It can be '-' for stdin.\n", ErrOut);
    fputs ("  The /COMMAND/ is processed by /bin/sh, so escape it!\n", ErrOut);
    exit (1);
}

    /** Make an ssh command the current process.
     * Commands to be run on the machine (in a POSIX shell)
     * should be given on this process' stdin.
     **/
static void
exec_ssh (const char* host)
{
    static const char* const ssh_cmd[] =
    {
        "ssh", "-o", "StrictHostKeyChecking=no",
        0,  /* /host/ goes here.*/
        "sh", "-s",
        0  /* /host/ also goes here as an argument.*/
    };
    const int argc = ArraySz( ssh_cmd );
    int argi = 0;
    char** argv;

    argv = AllocT( char*, argc+1 );

    for (; ssh_cmd[argi]; ++ argi)
        argv[argi] = strdup (ssh_cmd[argi]);

    argv[argi++] = strdup (host);

    for (; ssh_cmd[argi]; ++ argi)
        argv[argi] = strdup (ssh_cmd[argi]);

    argv[argi++] = strdup (host);

    AssertSafe( argi == argc, "" );
    argv[argc] = 0;

    execvp (argv[0], argv);
}

    /** Fork and run an ssh command to /host/.
     * Have it run the command(s) specified by /cmd/.
     **/
static void
fork_ssh (const char* cmd, const char* host)
{
    int ret;
    pid_t pid;
    int io[2];

    ret = pipe (io);
    AssertSafe( ret >= 0, "pipe" );

    pid = fork ();
    if (pid == 0)
    {
        close  (io[1]);
        dup2 (io[0], 0);
        exec_ssh (host);
    }
    else if (pid > 0)
    {
        int status = 0;
        close (io[0]);
        write (io[1], cmd, strlen (cmd) * sizeof (char));
        close (io[1]);
        ret = waitpid (pid, &status, 0);
        AssertSafe( ret >= 0, "waitpid" );
    }
    else
    {
        AssertSafe( 0, "fork" );
    }
}

int main (int argc, char** argv)
{
    int argi = 0;
    FILE* in = 0;
    char* line;
    char buf[BigEnuff];

    ExeName = argv[argi++];
    if (argi >= argc)  show_usage_and_exit ();

    if (0 == strcmp (argv[argi], "-"))
        in = stdin;
    else
        in = fopen (argv[argi], "rb");

    if (!in)
    {
        fprintf (ErrOut, "%s - Cannot open file:%s\n",
                 ExeName, argv[argi]);
        exit (1);
    }

    ++ argi;
    if (argi >= argc)  show_usage_and_exit ();
    
    for (line = fgets (buf, BigEnuff, in);
         line;
         line = fgets (buf, BigEnuff, in))
    {
        int q, r;
        q = 0;
        r = strlen (line);
        while (q < r && strchr (WhiteSpaceChars, line[q]  ))  ++q;
        while (r > q && strchr (WhiteSpaceChars, line[r-1]))  --r;

        if (r == q)  continue;
        line[r] = 0;
        line = &line[q];

        fork_ssh (argv[argi], line);
    }

    return 0;
}

