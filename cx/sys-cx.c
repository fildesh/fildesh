
#include "sys-cx.h"

#include "fileb.h"

#include <errno.h>

DeclTableT( HookFn, struct { void (*f) (); void* x; } );

static FileB* LogFileB = 0;
static DeclTable( HookFn, LoseFns );

    void
init_sys_cx ()
{
    static FileB logf;
    LogFileB = &logf;
    stdin_FileB ();
    stdout_FileB ();
    stderr_FileB ();
}

    void
push_losefn_sys_cx (void (*f) ())
{
    DeclGrow1Table( HookFn, hook, LoseFns );
    hook->f = f;
    hook->x = 0;
}
    void
push_losefn1_sys_cx (void (*f) (void*), void* x)
{
    DeclGrow1Table( HookFn, hook, LoseFns );
    hook->f = (void (*) ()) f;
    hook->x = x;
}

    void
lose_sys_cx ()
{
    { BLoop( i, LoseFns.sz )
            /* Do in reverse because it's a stack.*/
        DeclEltTable( HookFn, hook, LoseFns, LoseFns.sz-i-1 );
        if (hook->x)
            ((void (*) (void*)) hook->f) (hook->x);
        else
            hook->f ();
    } BLose()
    LoseTable( LoseFns );

    lose_FileB (stdin_FileB ());
    lose_FileB (stdout_FileB ());
    lose_FileB (stderr_FileB ());
}

    void
dbglog_printf3 (const char* file,
                const char* func,
                uint line,
                const char* fmt,
                ...)
{
    va_list args;
    int err = errno;
    FileB* f = stderr_FileB ();

    printf_FileB (f, "%s(%u) %s: ", file, line, func);

    va_start (args, fmt);
    vprintf_FileB (f, fmt, args);
    va_end(args);

    dump_char_FileB (f, '\n');

    if (err != 0)
    {
#if 0
            /* Why no work? */
        const uint n = 2048 * sizeof(char);
        char* s;

        printf_FileB (f, "^^^ errno:%d ", err);

        s = (char*) ensure_FileB (f, n);
        s[0] = '\0';

        strerror_r (err, s, n);

        f->off += strlen (s) * sizeof(char);
        dump_char_FileB (f, '\n');
#else
        printf_FileB (f, "^^^ errno:%d %s\n", err, strerror (err));
#endif
        errno = 0;
    }
    flusho_FileB (f);
}


    FileB*
stdin_FileB ()
{
    static FileB f_mem;
    static FileB* f = 0;
    if (!f)
    {
        f = &f_mem;
        init_FileB (f);
        set_FILE_FileB (f, stdin);
    }
    return f;
}

    FileB*
stdout_FileB ()
{
    static FileB f_mem;
    static FileB* f = 0;
    if (!f)
    {
        f = &f_mem;
        init_FileB (f);
        seto_FileB (f, true);
        set_FILE_FileB (f, stdout);
    }
    return f;
}

    FileB*
stderr_FileB ()
{
    static FileB f_mem;
    static FileB* f = 0;
    if (!f)
    {
        f = &f_mem;
        init_FileB (f);
        seto_FileB (f, true);
        set_FILE_FileB (f, stderr);
    }
    return f;
}

