
    /** This is the lace utility
     * as written by Alex Klinkhamer.
     * This code is public domain - no restrictions.
     **/

    /* stdlib.h  mkdtemp() */
#define _BSD_SOURCE

#include "cx/assoc.h"
#include "cx/fileb.h"

#include <assert.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <errno.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/wait.h>


static char* ExeName = 0;
#define ErrOut stderr


enum SymValKind
{
    IDescVal, ODescVal, IODescVal,
    IDescArgVal,
    IDescFileVal, ODescFileVal,
    IFutureDescVal, OFutureDescVal,
    IFutureDescFileVal, OFutureDescFileVal,
    HereDocVal, IHereDocFileVal,
    NSymValKinds
};
enum CommandKind { RunCommand, HereDocCommand, NCommandKinds };

typedef enum SymValKind SymValKind;
typedef enum CommandKind CommandKind;

typedef struct SymVal SymVal;
typedef struct Command Command;

#ifndef DeclTableT_int
#define DeclTableT_int
DeclTableT( int, int );
#endif

DeclTableT( iargs, struct { int fd; bool scrap_newline; } );

#ifndef DeclTableT_CString
#define DeclTableT_CString
DeclTableT( CString, char* );
#endif

struct Command
{
    char* line;
    CommandKind kind;
    TableT( CString ) args;
    TableT( CString ) extra_args;
    TableT( CString ) tmp_files;
    pid_t pid;
        /* Input stream.*/
    int stdis;
    TableT( int ) is;
        /* Output streams.*/
    int stdos;
    TableT( int ) os;
        /* If >= 0, this is a file descriptor that will
         * close when the program command is safe to run.
         */
    int exec_fd;
        /* If != 0, this is the contents of a file to execute.*/
    const char* exec_doc;

        /* Use these input streams to fill corresponding (null) arguments.*/
    TableT( iargs ) iargs;

        /* Use this if it's a HERE document.*/
    char* doc;
};
DeclTableT( Command, Command );

struct SymVal
{
    const char* name;
    SymValKind kind;
    uint arg_idx;  /* If a file.*/
    uint cmd_idx;
    union SymVal_union
    {
        int file_desc;
        char* here_doc;
    } as;
    DeclAssocNodeField( SymVal, SymVal );
};
DeclTableT( SymVal, SymVal );

static Trit
swapped_SymVal (const SymVal* lhs, const SymVal* rhs)
{
    int ret = strcmp (lhs->name, rhs->name);
    return ((ret < 0) ? Nil : ((ret > 0) ? Yes : May));
}
DeclAssocT( SymVal, SymVal, swapped_SymVal );

static void
init_SymVal (SymVal* v)
{
    v->name = 0;
    v->kind = NSymValKinds;
}

static void
cleanup_SymVal (SymVal* v)
{
    v->name = 0;
    v->kind = NSymValKinds;
}

static void
init_Command (Command* cmd)
{
    cmd->kind = NCommandKinds;
    InitTable( CString, cmd->args );
    InitTable( CString, cmd->extra_args );
    InitTable( CString, cmd->tmp_files );
    cmd->exec_fd = -1;
    cmd->exec_doc = 0;
    cmd->stdis = -1;
    InitTable( int, cmd->is );
    cmd->stdos = -1;
    InitTable( int, cmd->os );
    InitTable( iargs, cmd->iargs );
}

static void
close_Command (Command* cmd)
{
    uint i;
    if (cmd->stdis >= 0)  close (cmd->stdis);
    if (cmd->stdos >= 0)  close (cmd->stdos);
    if (cmd->is.sz > 0)
    {
        UFor( i, cmd->is.sz )
            close (cmd->is.s[i]);
    }
    LoseTable( int, cmd->is );
    InitTable( int, cmd->is );

    if (cmd->os.sz > 0)
    {
        UFor( i, cmd->os.sz )
            close (cmd->os.s[i]);
    }
    LoseTable( int, cmd->os );
    InitTable( int, cmd->os );

    if (cmd->exec_fd >= 0)
    {
        close (cmd->exec_fd);
        cmd->exec_fd = -1;
    }
    cmd->exec_doc = 0;
}

static void
lose_Command (Command* cmd)
{
    uint i;
    close_Command (cmd);
    free (cmd->line);
    if (cmd->kind == RunCommand)
        LoseTable( CString, cmd->args );
    else if (cmd->kind == HereDocCommand)
        free (cmd->doc);

    UFor( i, cmd->extra_args.sz )
        free (cmd->extra_args.s[i]);
    LoseTable( CString, cmd->extra_args );

    UFor( i, cmd->tmp_files.sz )
    {
        remove (cmd->tmp_files.s[i]);
        free (cmd->tmp_files.s[i]);
    }
    LoseTable( CString, cmd->tmp_files );

    LoseTable( iargs, cmd->iargs );
}


static SymVal*
getf_SymVal (TableT(SymVal)* syms, Assoc(SymVal)* assoc, const char* s)
{
    SymVal* x;  SymVal* y;

    GrowTable( SymVal, *syms, 1 );
    x = &syms->s[syms->sz-1];
    init_SymVal (x);
    x->name = s;

    FixRefsAssoc( SymVal, *assoc, syms->s, syms->sz-1 );
    y = GetfAssoc( SymVal, *assoc, *x );
    if (y != x)  MPopTable( SymVal, *syms, 1 );
    return y;
}

static uint
count_ws (const char* s)
{
    return strspn (s, WhiteSpaceChars);
}
static uint
count_non_ws (const char* s)
{
    return strcspn (s, WhiteSpaceChars);
}
static uint
trim_trailing_ws (char* s)
{
    uint n = strlen (s);
    while (0 < n && strchr (WhiteSpaceChars, s[n-1]))  --n;
    s[n] = '\0';
    return n;
}

    /** HERE document is created by
     * $(H var_name) Optional identifying stuff.
     * Line 1 in here.
     * Line 2 in here.
     * ...
     * Line n in here.
     * $(H var_name) Optional identifying stuff.
     *
     * OR it could look like:
     * $(H: var_name) value
     **/
static char*
parse_here_doc (FileB* in, const char* term)
{
    DeclTable( char, delim );
    char* s;

        /* Check for the single-line case.*/
    if (term[3] == ':')
    {
        term = strchr (term, ')');
        if (!term)  return strdup ("");
        term = &term[1];
        term = &term[count_ws (term)];
        s = strdup (term);
        trim_trailing_ws (s);
        return s;
    }

    GrowTable( char, delim, 1 + strlen(term) + 1 );
    delim.s[0] = '\n';
    strcpy (&delim.s[1], term);

    s = getlined_FileB (in, delim.s);
    LoseTable( char, delim );
    return strdup (s);
}

static void
inject_include (FileB* in, char* name)
{
    FileB src;

    name = &name[count_ws (name)];
    name[strcspn (name, ")")] = '\0';

    init_FileB (&src);
    open_FileB (&src, 0, name);
    if (!src.f)
        fprintf (ErrOut, "%s - Failed to include file:%s\n",
                 ExeName, name);

    inject_FileB (in, &src, "\n");
    lose_FileB (&src);
}

static char*
parse_line (FileB* in)
{
    DeclTable( char, line );
    char* s;

    for (s = getline_FileB (in);
         s;
         s = getline_FileB (in))
    {
        uint i, n;
        bool multiline = false;

        s = &s[count_ws (s)];
        if (s[0] == '#' || s[0] == '\0')  continue;

        n = trim_trailing_ws (s);

        multiline = s[n-1] == '\\';
        if (multiline)  --n;

        i = line.sz;
        GrowTable( char, line, n );
        memcpy (&line.s[i], s, n * sizeof (char));

        if (!multiline)  break;
    }
    GrowTable( char, line, 1 );
    line.s[line.sz-1] = '\0';
    PackTable( char, line );
    return line.s;
}

static void
sep_line (TableT(CString)* args, char* s)
{
    while (1)
    {
        uint i;

        i = count_ws (s);
        s = &s[i];
        if (s[0] == '\0')  break;

        if (s[0] == '\'')
        {
            s = &s[1];
            PushTable( CString, *args, s );
            i = strcspn (s, "'");
            if (s[i] == '\0')
            {
                fputs ("Unterminated single quote.\n", ErrOut);
                break;
            }
            s = &s[i];
        }
        else if (s[0] == '$' && s[1] == '(')
        {
            PushTable( CString, *args, s );
            s = &s[2];
            i = strcspn (s, ")");
            if (s[i] == '\0')
            {
                fputs ("Unterminated variable.\n", ErrOut);
                break;
            }
            s = &s[i+1];
        }
        else
        {
            PushTable( CString, *args, s );
            i = count_non_ws (s);
            s = &s[i];
        }
        if (s[0] == '\0')  break;
        s[0] = '\0';
        s = &s[1];
    }
    PackTable( CString, *args );
}


static TableT(Command)
parse_file (FileB* in)
{
    DeclTable( Command, cmds );
    while (true)
    {
        char* line;
        Command* cmd;
        line = parse_line (in);
        if (line[0] == '\0')
        {
            free (line);
            break;
        }
        GrowTable( Command, cmds, 1 );
        cmd = &cmds.s[cmds.sz - 1];
        init_Command (cmd);
        cmd->line = line;

        if (line[0] == '$' && line[1] == '(' &&
            line[2] == 'H' && line[3] != 'F')
        {
            cmd->kind = HereDocCommand;
            cmd->doc = parse_here_doc (in, line);
        }
        else if (line[0] == '$' && line[1] == '(' &&
                 line[2] == '<' && line[3] == '<')
        {
            inject_include (in, &line[4]);
                /* We don't add a command, just add more file content!*/
            lose_Command (&cmds.s[cmds.sz-1]);
            MPopTable( Command, cmds, 1 );
        }
        else
        {
            cmd->kind = RunCommand;
            sep_line (&cmd->args, cmd->line);
        }
    }
    PackTable( Command, cmds );
    return cmds;
}

static SymValKind
parse_sym (char* s)
{
    uint i, o;
    SymValKind kind = NSymValKinds;

    if (!(s[0] == '$' && s[1] == '('))  return NSymValKinds;

    i = count_non_ws (s);
    if (s[i] == '\0')  return NSymValKinds;

        /* Offset into string.*/
    o = 2;

    if (s[o] == 'X')
    {
        if (s[o+1] == 'O')
        {
            kind = IODescVal;
        }
        else if (s[o+1] == 'A')
        {
            kind = IDescArgVal;
        }
        else if (s[o+1] == 'F')
        {
            if (s[o+2] == 'v')  kind = IFutureDescFileVal;
            else                kind = IDescFileVal;
        }
        else
        {
            if (s[o+1] == 'v')  kind = IFutureDescVal;
            else                kind = IDescVal;
        }
    }
    if (s[o] == 'O')
    {
        if (s[o+1] == 'F')
        {
            if (s[o+2] == '^')  kind = OFutureDescFileVal;
            else                kind = ODescFileVal;
        }
        else
        {
            if (s[o+1] == '^')  kind = OFutureDescVal;
            else                kind = ODescVal;
        }
    }
    if (s[o] == 'H')
    {
        if (s[o+1] == 'F')  kind = IHereDocFileVal;
        else                kind = HereDocVal;
    }

    if (kind != NSymValKinds)
    {
        uint n;
        i += count_ws (&s[i]);
        n = strcspn (&s[i], ")");
        if (s[i+n] == ')')
        {
            memmove (s, &s[i], n * sizeof (char));
            s[n] = '\0';
        }
        else
        {
            kind = NSymValKinds;
        }
    }
    return kind;
}

static void
add_ios_Command (Command* cmd, int in, int out)
{
    if (in >= 0)
        PushTable( int, cmd->is, in );

    if (out >= 0)
        PushTable( int, cmd->os, out );
}

static void
add_iarg_Command (Command* cmd, int in, bool scrap_newline)
{
    add_ios_Command (cmd, in, -1);
    GrowTable( iargs, cmd->iargs, 1 );
    cmd->iargs.s[cmd->iargs.sz-1].fd = in;
    cmd->iargs.s[cmd->iargs.sz-1].scrap_newline = scrap_newline;
}

static char*
add_extra_arg_Command (Command* cmd, const char* s)
{
    PushTable( CString, cmd->extra_args, strdup (s) );
    return cmd->extra_args.s[cmd->extra_args.sz - 1];
}

static char*
add_fd_arg_Command (Command* cmd, int fd)
{
    char buf[1024];
    sprintf (buf, "/dev/fd/%d", fd);
    return add_extra_arg_Command (cmd, buf);
}

static char*
add_tmp_file_Command (Command* cmd, uint x, const char* tmpdir)
{
    char buf[1024];
    sprintf (buf, "%s/%u", tmpdir, x);
    PushTable( CString, cmd->tmp_files, strdup (buf) );
    return cmd->tmp_files.s[cmd->tmp_files.sz - 1];
}

    /** Write a file /name/ given contents /doc/.**/
static void
write_here_doc_file (const char* name, const char* doc)
{
    uint n;
    FILE* out;

    out = fopen (name, "wb");
    if (!out)
    {
        fprintf (ErrOut, "Cannot open file for writing!:%s\n",
                 name);
        return;
    }
    n = strlen (doc);
    fwrite (doc, sizeof (char), n, out);
    fclose (out);
}

static void
setup_commands (TableT(Command)* cmds,
                const char* tmpdir)
{
    uint i;
    uint ntmp_files = 0;
    DeclTable( SymVal, syms );
    Assoc( SymVal ) assoc;

    InitAssoc( SymVal, assoc );

    for (i = 0; i < cmds->sz; ++i)
    {
        uint arg_q = 0, arg_r = 0;
        Command* cmd;
        cmd = &cmds->s[i];

            /* The command defines a HERE document.*/
        if (cmd->kind == HereDocCommand)
        {
            SymVal* sym;
            SymValKind kind;

                /* The loop below should not run.*/
            assert (cmd->args.sz == 0 && "Invariant.");

            kind = parse_sym (cmd->line);
            assert (kind == HereDocVal);
            sym = getf_SymVal (&syms, &assoc, cmd->line);
            sym->kind = kind;
            sym->as.here_doc = cmd->doc;
        }

        for (arg_r = 0; arg_r < cmd->args.sz; ++ arg_r)
        {
            SymVal* sym;
            SymValKind kind;

            kind = parse_sym (cmd->args.s[arg_r]);

            if (kind < NSymValKinds)
            {
                sym = getf_SymVal (&syms, &assoc, cmd->args.s[arg_r]);
                if (kind == HereDocVal)
                {
                    assert (sym->kind == HereDocVal);
                    cmd->args.s[arg_q] = sym->as.here_doc;
                    ++ arg_q;
                }
                if (kind == IDescVal || kind == IDescFileVal ||
                    kind == IDescArgVal ||
                    kind == IODescVal || kind == IHereDocFileVal)
                {
                    int fd;
                    if (kind == IHereDocFileVal)
                    {
                        assert (sym->kind == HereDocVal);
                    }
                    else
                    {
                        assert (sym->kind == ODescVal);
                        sym->kind = NSymValKinds;
                    }
                    fd = sym->as.file_desc;

                    if (kind == IDescVal || kind == IODescVal)
                    {
                        cmd->stdis = fd;
                    }
                    else if (kind == IDescArgVal)
                    {
                        add_iarg_Command (cmd, fd, true);
                        cmd->args.s[arg_q] = 0;
                        ++ arg_q;
                    }
                    else if (kind == IDescFileVal)
                    {
                        add_ios_Command (cmd, fd, -1);
                        if (arg_q > 0)
                        {
                            cmd->args.s[arg_q] = add_fd_arg_Command (cmd, fd);
                        }
                        else
                        {
                            cmd->args.s[0] = add_tmp_file_Command (cmd, ntmp_files, tmpdir);
                            ++ ntmp_files;
                            if (sym->arg_idx < Max_uint)
                                cmds->s[sym->cmd_idx].args.s[sym->arg_idx] = cmd->args.s[0];
                            cmd->exec_fd = fd;
                        }
                        ++ arg_q;
                    }
                    else
                    {
                        assert (kind == IHereDocFileVal);
                        cmd->args.s[arg_q] =
                            add_tmp_file_Command (cmd, ntmp_files, tmpdir);
                        ++ ntmp_files;
                            /* Write the temp file now.*/
                        write_here_doc_file (cmd->args.s[arg_q],
                                             sym->as.here_doc);
                        if (arg_q == 0)
                            cmd->exec_doc = sym->as.here_doc;
                        ++ arg_q;
                    }
                }
                else if (kind == OFutureDescVal || kind == OFutureDescFileVal)
                {
                    int fd;
                    assert (sym->kind == IFutureDescVal);
                    sym->kind = NSymValKinds;
                    fd = sym->as.file_desc;

                    if (kind == OFutureDescVal)
                    {
                        cmd->stdos = fd;
                    }
                    else
                    {
                        add_ios_Command (cmd, fd, -1);
                        if (sym->arg_idx > 0)
                        {
                            cmd->args.s[arg_q] = add_fd_arg_Command (cmd, fd);
                        }
                        else
                        {
                            char* s;
                            s = add_tmp_file_Command (&cmds->s[sym->cmd_idx],
                                                      ntmp_files, tmpdir);
                            ++ ntmp_files;
                            cmds->s[sym->cmd_idx].args.s[0] = s;
                            cmd->args.s[arg_q] = s;
                        }
                        ++ arg_q;
                    }
                }
                if (kind == ODescVal || kind == ODescFileVal ||
                    kind == IODescVal)
                {
                    int fd[2];
                    int ret;
                    assert (sym->kind == NSymValKinds);
                    sym->kind = ODescVal;
                    sym->cmd_idx = i;
                    sym->arg_idx = Max_uint;

                    ret = pipe (fd);
                    assert (!(ret < 0));

                    sym->as.file_desc = fd[0];
                    if (kind == ODescVal || kind == IODescVal)
                    {
                        cmd->stdos = fd[1];
                    }
                    else
                    {
                        add_ios_Command (cmd, -1, fd[1]);
                        cmd->args.s[arg_q] = add_fd_arg_Command (cmd, fd[1]);
                        sym->arg_idx = arg_q;
                        ++ arg_q;
                    }
                }
                else if (kind == IFutureDescVal || kind == IFutureDescFileVal)
                {
                    int fd[2];
                    int ret;
                    assert (sym->kind == NSymValKinds);
                    sym->kind = IFutureDescVal;
                    sym->cmd_idx = i;
                    sym->arg_idx = Max_uint;

                    ret = pipe (fd);
                    assert (!(ret < 0));

                    sym->as.file_desc = fd[1];
                    if (kind == IFutureDescVal)
                    {
                        cmd->stdis = fd[0];
                    }
                    else
                    {
                        add_ios_Command (cmd, -1, fd[0]);
                        cmd->args.s[arg_q] = add_fd_arg_Command (cmd, fd[0]);
                        if (arg_q == 0)
                        {
                            sym->arg_idx = 0;
                            cmd->exec_fd = fd[0];
                        }
                        ++ arg_q;
                    }
                }
            }
            else
            {
                cmd->args.s[arg_q] = cmd->args.s[arg_r];
                ++ arg_q;
            }
        }

        if (cmd->args.sz > 0)
            cmd->args.sz = arg_q;
    }

    UFor( i, syms.sz )
    {
        if (syms.s[i].kind == ODescVal)
        {
            fprintf (ErrOut, "%s - Dangling output stream! Symbol:%s\n",
                     ExeName, syms.s[i].name);
            assert (syms.s[i].kind != ODescVal && "A dangling output stream!");
        }
    }

    UFor( i, syms.sz )
        cleanup_SymVal (&syms.s[i]);
    LoseTable( SymVal, syms );
}

static void
output_Command (FILE* out, const Command* cmd)
{
    uint i = 0;
    if (cmd->kind != RunCommand)  return;

    fputs ("COMMAND: ", out);
    UFor( i, cmd->args.sz )
    {
        if (i > 0)  fputc (' ', out);
        if (cmd->args.s[i])
            fputs (cmd->args.s[i], out);
        else
            fputs ("NULL", out);
    }
    fputc ('\n', out);
}

    /** Read from the file descriptor /in/ and write to file /name/.
     * If the input stream contains no data, the file will not be
     * written (or overwritten).
     **/
static void
pipe_to_file (int in, const char* name)
{
    FILE* out = 0;

    while (true)
    {
        ssize_t sz;
        byte buf[BUFSIZ];

        sz = read (in, buf, BUFSIZ);

        if (sz <= 0)  break;
        if (!out)  out = fopen (name, "wb");

        fwrite (buf, 1, sz, out);
    }

    close (in);
    if (out)  fclose (out);
}

    /** Read everything from the file descriptor /in/.**/
static char*
readin_fd (int in, bool scrap_newline)
{
    const uint n_per_chunk = 8192;
    DeclTable( char, buf );
    uint off = 0;

    while (1)
    {
        ssize_t n;
        n = off + n_per_chunk + 1;
        if (buf.sz < (size_t) n)
            GrowTable( char, buf, n - buf.sz );

        n = read (in, &buf.s[off], n_per_chunk * sizeof(char));
        assert (n >= 0 && "Problem reading file descriptor!");
        if (n == 0)  break;
        off += n;
    }
    close (in);

    MPopTable( char, buf, buf.sz - off + 1 );
    buf.s[off] = '\0';
    if (scrap_newline && off > 0 && buf.s[off-1] == '\n')
    {
        buf.s[--off] = '\0';
        if (off > 0 && buf.s[off-1] == '\r')
            buf.s[--off] = '\0';
    }
    return buf.s;
}

    /** Fill in all NULL arguments, they should be
     * filled by the output of another command.
     **/
static void
fill_dependent_args (Command* cmd)
{
    uint i, j;
    j = 0;
    UFor( i, cmd->args.sz )
    {
        if (!cmd->args.s[i])
        {
            assert (j < cmd->iargs.sz);
            cmd->args.s[i] = readin_fd (cmd->iargs.s[j].fd,
                                        cmd->iargs.s[j].scrap_newline);
            ++ j;
        }
    }
    assert (j == cmd->iargs.sz);
}

    /** Add the utility bin directory to the PATH environment variable.**/
static void
add_util_path_env ()
{
    static const char k[] = "PATH";
#ifndef UtilBin
#define UtilBin "bin"
#endif
    static const char path[] = UtilBin;
#undef UtilBin
    char* v;
    DeclTable( char, dec );

    v = getenv (k);
    if (!v)
    {
        setenv (k, path, 1);
        return;
    }

    GrowTable( char, dec, strlen (path) + 1 + strlen (v) + 1 );
    sprintf (dec.s, "%s:%s", path, v);
    setenv (k, dec.s, 1);
    LoseTable( char, dec );
}

static void
show_usage_and_exit ()
{
    const char* s;
    s = "Usage: %s [[-f] SCRIPTFILE | -- SCRIPT]\n";
    fprintf (ErrOut, s, ExeName);
    exit (1);
}

int main (int argc, char** argv)
{
    FileB in;
    TableT( Command ) cmds;
    uint i;
    int ret;
    int argi = 0;
    char tmpdir[128];

    init_FileB (&in);

    ExeName = argv[argi++];

    add_util_path_env ();

    strcpy (tmpdir, "/tmp/lace-XXXXXX");

    if (!mkdtemp (tmpdir))
    {
        fprintf (ErrOut, "%s - Unable to create temp directory:%s\n",
                 ExeName,
                 strerror (errno));
        exit (1);
    }

    if (argi < argc)
    {
        const char* arg;
        arg = argv[argi++];
        if (0 == strcmp (arg, "--"))
        {
            if (argi >= argc)  show_usage_and_exit ();
            arg = argv[argi++];
            in.buf.s = (byte*) strdup (arg);
            in.buf.sz = strlen (cstr_FileB (&in)) + 1;
            in.buf.alloc_sz = in.buf.sz;
        }
        else
        {
                /* Optional -f flag.*/
            if (0 == strcmp (arg, "-f"))
            {
                if (argi >= argc)  show_usage_and_exit ();
                arg = argv[argi++];
            }

            in.f = fopen (arg, "rb");
            if (!in.f)
            {
                fprintf (ErrOut, "%s - Cannot read script:%s\n",
                         ExeName, arg);
                exit (1);
            }
        }

        if (argi < argc)  show_usage_and_exit ();
    }
    else
    {
        in.f = stdin;
    }

    cmds = parse_file (&in);
    lose_FileB (&in);

    setup_commands (&cmds, tmpdir);


    if (false)
        UFor( i, cmds.sz )
            output_Command (ErrOut, &cmds.s[i]);

    for (i = 0; i < cmds.sz; ++i)
    {
        uint j;
        Command* cmd;

        cmd = &cmds.s[i];

        if (cmd->kind != RunCommand)  continue;

        cmd->pid = fork ();
        if (cmd->pid > 0)
        {
            close_Command (cmd);
            continue;
        }
        assert (!(cmd->pid < 0));

        UFor( j, cmds.sz )
            if (j != i)
                close_Command (&cmds.s[j]);

        if (cmd->stdis >= 0)  dup2 (cmd->stdis, 0);
        if (cmd->stdos >= 0)  dup2 (cmd->stdos, 1);

        if (cmd->exec_fd >= 0)
        {
            pipe_to_file (cmd->exec_fd, cmd->args.s[0]);
            cmd->exec_fd = -1;
            chmod (cmd->args.s[0], S_IRUSR | S_IWUSR | S_IXUSR);
        }
        if (cmd->exec_doc)
        {
            cmd->exec_doc = 0;
            chmod (cmd->args.s[0], S_IRUSR | S_IWUSR | S_IXUSR);
        }

        fill_dependent_args (cmd);

        PushTable( CString, cmd->args, 0 );
        execvp (cmd->args.s[0], cmd->args.s);
        ret = errno;

        fprintf (ErrOut, "%s - Error executing:%s\n", ExeName, cmd->args.s[0]);
        fprintf (ErrOut, "  Reason:%s\n", strerror (ret));
        assert (0);
        exit (1);
    }

    UFor( i, cmds.sz )
    {
        if (cmds.s[i].kind == RunCommand)
            ret = waitpid (cmds.s[i].pid, 0, 0);
        lose_Command (&cmds.s[i]);
    }
    LoseTable( Command, cmds );

    ret = rmdir (tmpdir);
    if (ret != 0)
        fprintf (ErrOut, "Temp directory not removed: %s\n", tmpdir);

    return 0;
}

