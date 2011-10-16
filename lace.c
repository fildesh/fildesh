
/** This is the lace utility
 * as written by Alex Klinkhamer.
 * This code is public domain - no restrictions.
 **/

#include <assert.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <sys/wait.h>


typedef unsigned int uint;
#define Max_uint UINT_MAX

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
    IDescVal, ODescVal,
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
    char** extra_args;
    pid_t pid;
        /* Input stream.*/
    uint nis;
    int* is;
        /* Output streams.*/
    uint nos;
    int* os;
        /* Use this if it's a here document.*/
    char* doc;
};

struct SymVal
{
    const char* name;
    SymValKind kind;
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
    cmd->nis = 0;
    cmd->nos = 0;
}

static void
close_Command (Command* cmd)
{
    uint i;
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
}

static void
cleanup_Command (Command* cmd)
{
    close_Command (cmd);
    free (cmd->line);
    if (cmd->kind == RunCommand)
        free (cmd->args);
    else if (cmd->kind == HereDocCommand)
        free (cmd->doc);
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

        r = strlen (&s[i]);
        assert (r > 0);
            /* if (i > 0)  memmove (s, &s[i], r+1); */

        while (strchr (WhiteSpaceChars, s[r-1]))  --r;

        if (s[r-1] == '\\')
        {
            s[r-1] = ' ';
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
    uint i;
    SymValKind kind = NSymValKinds;

    if (!(s[0] == '$' && s[1] == '('))  return NSymValKinds;

    i = count_non_ws (s);
    if (s[i] == '\0')  return NSymValKinds;

    if (s[2] == 'X')  kind = IDescVal;
    if (s[2] == 'O')  kind = ODescVal;
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

static SymVal*
setup_commands (uint* ret_nsyms, uint ncmds, Command* cmds)
{
    uint i;
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
                else if (kind == ODescVal)
                {
                    int fd[2];
                    int ret;
                    assert (sym->kind == NSymValKinds);
                    sym->kind = ODescVal;

                    ret = pipe (fd);
                    assert (!(ret < 0));

                    sym->as.file_desc = fd[0];
                    add_ios_Command (cmd, -1, fd[1]);
                }
                else if (kind == IDescVal)
                {
                    assert (sym->kind == ODescVal);
                    sym->kind = NSymValKinds;
                    add_ios_Command (cmd, sym->as.file_desc, -1);
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

int main (int argc, char** argv)
{
    FILE* in = 0;
    uint ncmds = 0;
    Command* cmds;
    uint nsyms = 0;
    SymVal* syms;
    uint i;
    int ret;

    if (argc == 2)
        in = fopen (argv[1], "rb");
    if (!in)
        in = stdin;

    cmds = parse_file (in, &ncmds);
    fclose (in);

    syms = setup_commands (&nsyms, ncmds, cmds);

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

        UFor( j, cmd->nis )  dup2 (cmd->is[j], 0);
        UFor( j, cmd->nos )  dup2 (cmd->os[j], 1);

        execvp (cmd->args[0], cmd->args);
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

    return 0;
}

