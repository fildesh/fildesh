
/** This is the lace utility
 * as written by Alex Klinkhamer.
 * This code is public domain - no restrictions.
 **/

    /* stdlib.h  mkdtemp() */
#define _BSD_SOURCE

#include <assert.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <errno.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/wait.h>


typedef unsigned int uint;
#define Max_uint UINT_MAX
typedef unsigned char byte;

#define true 1
#define false 0
#define UFor( i, bel )  for (i = 0; i < bel; ++i)

#define AllocT( Type, capacity ) \
    (((capacity) == 0) ? (Type*) 0 : \
     (Type*) malloc ((capacity) * sizeof (Type)))

#define ResizeT( Type, arr, capacity ) \
    ((arr) = (Type*) realloc (arr, (capacity) * sizeof (Type)))

static const char WhiteSpaceChars[] = " \t\v\r\n";

enum SymValKind
{
    IDescVal, ODescVal, IODescVal,
    IDescFileVal, ODescFileVal,
    IFutureDescVal, OFutureDescVal,
    IFutureDescFileVal, OFutureDescFileVal,
    HereDocVal,
    NSymValKinds
};
enum CommandKind { RunCommand, HereDocCommand, NCommandKinds };

typedef enum SymValKind SymValKind;
typedef enum CommandKind CommandKind;

typedef struct SymVal SymVal;
typedef struct Command Command;


struct Command
{
    char* line;
    CommandKind kind;
    char** args;
    uint nextra_args;
    char** extra_args;
    uint ntmp_files;
    char** tmp_files;
    pid_t pid;
        /* Input stream.*/
    int stdis;
    uint nis;
    int* is;
        /* Output streams.*/
    int stdos;
    uint nos;
    int* os;
        /* If >= 0, this is a file descriptor that will
         * close when the program command is safe to run.
         */
    int exec_fd;
        /* Use this if it's a here document.*/
    char* doc;
};

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
};


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
    cmd->nextra_args = 0;
    cmd->ntmp_files = 0;
    cmd->exec_fd = -1;
    cmd->stdis = -1;
    cmd->stdos = -1;
    cmd->nis = 0;
    cmd->nos = 0;
}

static void
close_Command (Command* cmd)
{
    uint i;
    if (cmd->stdis >= 0)  close (cmd->stdis);
    if (cmd->stdos >= 0)  close (cmd->stdos);
    if (cmd->nis > 0)
    {
        UFor( i, cmd->nis )
            close (cmd->is[i]);
        free (cmd->is);
        cmd->nis = 0;
    }
    if (cmd->nos > 0)
    {
        UFor( i, cmd->nos )
            close (cmd->os[i]);
        cmd->nos = 0;
        free (cmd->os);
    }
    if (cmd->exec_fd >= 0)
    {
        close (cmd->exec_fd);
        cmd->exec_fd = -1;
    }
}

static void
cleanup_Command (Command* cmd)
{
    uint i;
    close_Command (cmd);
    free (cmd->line);
    if (cmd->kind == RunCommand)
        free (cmd->args);
    else if (cmd->kind == HereDocCommand)
        free (cmd->doc);

    UFor( i, cmd->nextra_args )
        free (cmd->extra_args[i]);
    if (cmd->nextra_args > 0)
        free (cmd->extra_args);
    UFor( i, cmd->ntmp_files )
    {
        remove (cmd->tmp_files[i]);
        free (cmd->tmp_files[i]);
    }
    if (cmd->ntmp_files > 0)
        free (cmd->tmp_files);
}


    /** Find a string in an array of strings,
     * return the location of it.
     * If not found, Max_uint is returned.
     **/
static uint
lookup_SymVal (uint n, const SymVal* a, const char* s)
{
    uint i;
    UFor( i, n )
        if (0 == strcmp (a[i].name, s))
            return i;
    return Max_uint;
}

static SymVal*
add_SymVal (uint* n, SymVal* a, const char* s)
{
    uint i;
    i = lookup_SymVal (*n, a, s);
    if (i != Max_uint)  return &a[i];
    i = *n;
    *n += 1;
    init_SymVal (&a[i]);
    a[i].name = s;
    return &a[i];
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

    /** HERE document is created by
     * ${H var_name} Optional identifying stuff.
     * Line 1 in here.
     * Line 2 in here.
     * ...
     * Line n in here.
     * ${H var_name} Optional identifying stuff.
     **/
static char*
parse_here_doc (FILE* in, const char* term)
{
    const size_t max_line_sz = 2048;
    uint nfull;
    size_t term_sz;
    uint n = 0;
    char* doc;

    term_sz = strlen (term) * sizeof (char);
    nfull = 2 * max_line_sz;
    doc = AllocT( char, nfull );

    while (true)
    {
        char* s;
        uint r;

        if (n + max_line_sz > nfull)
        {
            nfull += max_line_sz;
            ResizeT( char, doc, nfull );
        }

        s = &doc[n];
        if (!fgets (s, max_line_sz, in))  break;
        r = strlen (s);
        while (r > 0 && strchr (WhiteSpaceChars, s[r-1]))  --r;

        if (term_sz == r * sizeof (char))
            if (0 == memcmp (s, term, term_sz))
                break;

        n += strlen (s);
    }
        /* Remove trailing newline.*/
    if (n > 0)  -- n;
    doc[n] = '\0';
    ResizeT( char, doc, n+1 );
    return doc;
}

static char*
parse_line (FILE* in)
{
    const size_t max_line_sz = 1024;
    uint nfull;
    uint n = 0;
    char* line;

    nfull = 2 * max_line_sz;
    line = AllocT( char, nfull );

    while (true)
    {
        uint i, r;
        char* s;

        if (n + max_line_sz > nfull)
        {
            nfull += max_line_sz;
            ResizeT( char, line, nfull );
        }

        s = &line[n];

        if (!fgets (s, max_line_sz, in))  break;

        i = count_ws (s);
        if (s[i] == '#' || s[i] == '\0')  continue;

        r = i + strlen (&s[i]);
        assert (r > 0);
            /* if (i > 0)  memmove (s, &s[i], r+1); */

        while (strchr (WhiteSpaceChars, s[r-1]))  --r;

        if (s[r-1] == '\\')
        {
            n += r-1;
        }
        else
        {
            n += r;
            break;
        }
    }
    line[n] = '\0';
    ResizeT( char, line, n+1 );
    return line;
}

static char**
sep_line (char* s)
{
    FILE* out = stderr;
    const size_t max_nargs = 1024;
    uint n = 0;
    char** a;

    a = AllocT( char*, max_nargs );

    while (n < max_nargs)
    {
        uint i;

        i = count_ws (s);
        s = &s[i];
        if (s[0] == '\0')  break;

        if (s[0] == '\'')
        {
            s = &s[1];
            a[n++] = s;
            i = strcspn (s, "'");
            if (s[i] == '\0')
            {
                fputs ("Unterminated single quote.\n", out);
                break;
            }
            s = &s[i];
        }
        else if (s[0] == '$' && s[1] == '(')
        {
            a[n++] = s;
            s = &s[2];
            i = strcspn (s, ")");
            if (s[i] == '\0')
            {
                fputs ("Unterminated variable.\n", out);
                break;
            }
            s = &s[i+1];
        }
        else
        {
            a[n++] = s;
            i = count_non_ws (s);
            s = &s[i];
        }
        if (s[0] == '\0')  break;
        s[0] = '\0';
        s = &s[1];
    }
    a[n] = 0;
    ResizeT( char*, a, n+1 );
    return a;
}


static Command*
parse_file(FILE* in, uint* n)
{
    const uint cmds_chunk_sz = 64;
    uint ncmds_full = 0;
    uint ncmds = 0;
    Command* cmds;

    ncmds_full = 2 * cmds_chunk_sz;
    cmds = AllocT( Command, ncmds_full );
    
    while (true)
    {
        char* line;
        Command* cmd;
        if (ncmds + 1 > ncmds_full)
        {
            ncmds_full += cmds_chunk_sz;
            ResizeT( Command, cmds, ncmds_full );
        }
        cmd = &cmds[ncmds];
        init_Command (cmd);
        line = parse_line (in);
        if (line[0] == '\0')
        {
            free (line);
            break;
        }
        ncmds += 1;
        cmd->line = line;

        if (line[0] == '$' && line[1] == '(' && line[2] == 'H')
        {
            cmd->kind = HereDocCommand;
            cmd->doc = parse_here_doc (in, line);
        }
        else
        {
            cmd->kind = RunCommand;
            cmd->args = sep_line (cmd->line);
        }
    }

    *n = ncmds;
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
    if (s[2] == 'H')  kind = HereDocVal;

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
    {
        if (cmd->nis == 0)
            cmd->is = AllocT( int, 1 );
        else
            ResizeT( int, cmd->is, cmd->nis+1 );
        cmd->is[cmd->nis] = in;
        ++ cmd->nis;
    }

    if (out >= 0)
    {
        if (cmd->nos == 0)
            cmd->os = AllocT( int, 1 );
        else
            ResizeT( int, cmd->os, cmd->nos+1 );
        cmd->os[cmd->nos] = out;
        ++ cmd->nos;
    }
}

static char*
add_extra_arg_Command (Command* cmd, const char* s)
{
    uint i;
    i = cmd->nextra_args;
    if (i == 0)
        cmd->extra_args = AllocT( char*, 1 );
    else
        ResizeT( char*, cmd->extra_args, i+1 );
    cmd->extra_args[i] = strdup (s);
    ++ cmd->nextra_args;
    return cmd->extra_args[i];
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
    uint i;
    sprintf (buf, "%s/%u", tmpdir, x);
    i = cmd->ntmp_files;
    if (i == 0)
        cmd->tmp_files = AllocT( char*, 1 );
    else
        ResizeT( char*, cmd->tmp_files, i+1 );
    cmd->tmp_files[i] = strdup (buf);
    ++ cmd->ntmp_files;
    return cmd->tmp_files[i];
}

static SymVal*
setup_commands (uint* ret_nsyms, uint ncmds, Command* cmds,
                const char* tmpdir)
{
    uint i;
    uint ntmp_files = 0;
    const uint syms_chunk_sz = 1;
    uint nsyms_full;
    uint nsyms = 0;
    SymVal* syms;

    nsyms_full = 2 * syms_chunk_sz;
    syms = AllocT( SymVal, nsyms_full );

    for (i = 0; i < ncmds; ++i)
    {
        uint arg_q = 0, arg_r = 0;
        Command* cmd;
        cmd = &cmds[i];

        while (true)
        {
            SymVal* sym;
            SymValKind kind;

            if (nsyms + 1 > nsyms_full)
            {
                nsyms_full += syms_chunk_sz;
                ResizeT( SymVal, syms, nsyms_full );
            }

                /* This isn't really a loop for a here document.*/
            if (cmd->kind == HereDocCommand)
            {
                kind = parse_sym (cmd->line);
                assert (kind == HereDocVal);
                sym = add_SymVal (&nsyms, syms, cmd->line);
                sym->kind = kind;
                sym->as.here_doc = cmd->doc;
                break;
            }

            kind = parse_sym (cmd->args[arg_r]);

            if (kind < NSymValKinds)
            {
                sym = add_SymVal (&nsyms, syms, cmd->args[arg_r]);
                if (kind == HereDocVal)
                {
                    assert (sym->kind == HereDocVal);
                    cmd->args[arg_q] = sym->as.here_doc;
                    ++ arg_q;
                }
                if (kind == IDescVal || kind == IDescFileVal ||
                    kind == IODescVal)
                {
                    int fd;
                    assert (sym->kind == ODescVal);
                    sym->kind = NSymValKinds;
                    fd = sym->as.file_desc;

                    if (kind == IDescVal || kind == IODescVal)
                    {
                        cmd->stdis = fd;
                    }
                    else
                    {
                        add_ios_Command (cmd, fd, -1);
                        if (arg_q > 0)
                        {
                            cmd->args[arg_q] = add_fd_arg_Command (cmd, fd);
                        }
                        else
                        {
                            cmd->args[0] = add_tmp_file_Command (cmd, ntmp_files, tmpdir);
                            ++ ntmp_files;
                            if (sym->arg_idx < Max_uint)
                                cmds[sym->cmd_idx].args[sym->arg_idx] = cmd->args[0];
                            cmd->exec_fd = fd;
                        }
                        ++ arg_q;
                    }
                }
                else if (kind == OFutureDescVal || kind == OFutureDescFileVal)
                {
                    int fd;
                    assert (sym->kind == IFutureDescVal);
                    sym->kind = NSymValKinds;
                    fd = sym->as.file_desc;

                    if (kind == IDescVal)
                    {
                        cmd->stdis = fd;
                    }
                    else
                    {
                        add_ios_Command (cmd, fd, -1);
                        if (sym->arg_idx > 0)
                        {
                            cmd->args[arg_q] = add_fd_arg_Command (cmd, fd);
                        }
                        else
                        {
                            char* s;
                            s = add_tmp_file_Command (&cmds[sym->cmd_idx],
                                                      ntmp_files, tmpdir);
                            ++ ntmp_files;
                            cmds[sym->cmd_idx].args[0] = s;
                            cmd->args[arg_q] = s;
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
                        cmd->args[arg_q] = add_fd_arg_Command (cmd, fd[1]);
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
                        cmd->stdos = fd[0];
                    }
                    else
                    {
                        add_ios_Command (cmd, -1, fd[0]);
                        cmd->args[arg_q] = add_fd_arg_Command (cmd, fd[0]);
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
                cmd->args[arg_q] = cmd->args[arg_r];
                ++ arg_q;
            }

            ++ arg_r;
            if (!cmd->args[arg_r])
            {
                cmd->args[arg_q] = 0;
                break;
            }
        }
    }
    *ret_nsyms = nsyms;

    UFor( i, nsyms )
        assert (syms[i].kind != ODescVal && "A dangling output stream!");

    return syms;
}

static void
output_Command (FILE* out, const Command* cmd)
{
    uint i = 0;
    if (cmd->kind != RunCommand)  return;

    fputs ("COMMAND: ", out);
    while (cmd->args[i])
    {
        if (i > 0)  fputc (' ', out);
        fputs (cmd->args[i], out);
        ++ i;
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

int main (int argc, char** argv)
{
    FILE* in = 0;
    uint ncmds = 0;
    Command* cmds;
    uint nsyms = 0;
    SymVal* syms;
    uint i;
    int ret;
    char tmpdir[128];

    strcpy (tmpdir, "/tmp/lace-XXXXXX");

    if (!mkdtemp (tmpdir))
    {
        sprintf ("Unable to create temp directory: %s\n", strerror (errno));
        exit (1);
    }

    if (argc == 2)
        in = fopen (argv[1], "rb");
    if (!in)
        in = stdin;

    cmds = parse_file (in, &ncmds);
    fclose (in);

    syms = setup_commands (&nsyms, ncmds, cmds, tmpdir);

    if (false)
        UFor( i, ncmds )
            output_Command (stderr, &cmds[i]);

    for (i = 0; i < ncmds; ++i)
    {
        uint j;
        Command* cmd;

        cmd = &cmds[i];

        if (cmd->kind != RunCommand)  continue;

        cmd->pid = fork ();
        if (cmd->pid > 0)
        {
            close_Command (cmd);
            continue;
        }
        assert (!(cmd->pid < 0));

        UFor( j, ncmds )
            if (j != i)
                close_Command (&cmds[j]);

        if (cmd->stdis >= 0)  dup2 (cmd->stdis, 0);
        if (cmd->stdos >= 0)  dup2 (cmd->stdos, 1);

        if (cmd->exec_fd >= 0)
        {
            pipe_to_file (cmd->exec_fd, cmd->args[0]);
            cmd->exec_fd = -1;
            chmod (cmd->args[0], S_IRUSR | S_IWUSR | S_IXUSR);
        }

        execvp (cmd->args[0], cmd->args);

        fprintf (stderr, "Error executing: %s\n", strerror (errno));
        assert (0);
        exit (1);
    }

    UFor( i, nsyms )
        cleanup_SymVal (&syms[i]);

    UFor( i, ncmds )
    {
        if (cmds[i].kind == RunCommand)
            ret = waitpid (cmds[i].pid, 0, 0);
        cleanup_Command (&cmds[i]);
    }
    free (cmds);
    free (syms);

    ret = rmdir (tmpdir);
    if (ret != 0)
        fprintf (stderr, "Temp directory not removed: %s\n", tmpdir);

    return 0;
}

