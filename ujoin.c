
#include "futil.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

static const char* ExeName = 0;
#define ErrOut stderr

typedef struct LineJoin LineJoin;
struct LineJoin
{
    char* field;
    char* small_line;  /* From small file.*/
    char* large_line;  /* From large file.*/
};

DeclTableT( LineJoin );

    void
init_LineJoin (LineJoin* join)
{
    join->field = 0;
    join->small_line = 0;
    join->large_line = 0;
}

static void
show_usage_and_exit ()
{
    const char* s;
    s = "Usage: %s SMALL LARGE [OPTION]*\n";
    fprintf (ErrOut, s, ExeName);
    s = "    SMALL is a file used for lookup.\n";
    fputs (s, ErrOut);
    s = "    LARGE can be a stream, which tries to match the fields in SMALL.\n";
    fputs (s, ErrOut);
    exit (1);
}

    /** Open a file for reading or writing.
     * Exit with an error status if this is not possible.
     **/
static FILE*
open_file_arg (const char* arg, bool writing)
{
    FILE* f;
    if (writing)
    {
        if (0 == strcmp (arg, "-"))  f = stdout;
        else                         f = fopen (arg, "wb");
    }
    else
    {
        if (0 == strcmp (arg, "-"))  f = stdin;
        else                         f = fopen (arg, "rb");
    }
    if (!f)
    {
        fprintf (ErrOut, "%s - Cannot open file for %s:%s\n",
                 ExeName,
                 writing ? "writing" : "reading",
                 arg);
        exit (1);
    }
    return f;
}

static Table(LineJoin)
setup_lookup_table (char* contents, const char* delim)
{
    DeclTable( LineJoin, table );
    char* s = contents;
    const uint delim_sz = delim ? strlen (delim) : 0;

    while (s)
    {
        LineJoin* join;

        if (table.sz > 0)
        {
            if (s != contents && s[-1] == '\r')
                s[-1] = '\0';
            if (s[0] != '\0')
            {
                s[0] = '\0';
                s = &s[1];
            }
        }
        if (s[0] == '\0')  break;

        GrowTable( LineJoin, table, 1 );
        join = &table.s[table.sz-1];
        init_LineJoin (join);
        join->field = s;
        if (delim)
            s = strstr (s, delim);
        else
            s = &s[strcspn (s, WhiteSpaceChars)];

        if (!s)
        {
            s = &join->field[strlen (join->field)];
            fprintf (ErrOut, "%s - Small file has no delimiter, line:%u\n",
                     ExeName, table.sz);
        }
        if (s[0] != '\0')
        {
            s[0] = 0;
            if (delim)  s = &s[delim_sz];
            else        s = &s[1 + strspn (&s[1], WhiteSpaceChars)];
        }
        join->small_line = s;
        s = strchr (s, '\n');
    }

    PackTable( LineJoin, table );
    return table;
}

static void
compare_lines (FILE* in, Table(LineJoin)* table, const char* delim,
               FILE* nomatch_out)
{
    DeclTable( char, line );
    const uint delim_sz = delim ? strlen (delim) : 0;
    uint line_no = 0;
    uint off;

    for (off = getline_FILE (in, &line, 0);
         off > 0;
         off = getline_FILE (in, &line, off))
    {
        uint i;
        char* field = line.s;
        char* payload;

        ++ line_no;

        if (delim)  payload = strstr (line.s, delim);
        else        payload = &line.s[strcspn (line.s, WhiteSpaceChars)];

        if (!payload || payload[0] == '\0')
        {
            fprintf (ErrOut, "%s - Large file has no delimiter, line:%u\n",
                     ExeName, line_no);
            continue;
        }

        payload[0] = '\0';
        if (delim)
            payload = &payload[delim_sz];
        else
            payload = &payload[1 + strspn (&payload[1], WhiteSpaceChars)];

        UFor( i, table->sz )
        {
            if (0 == strcmp (field, table->s[i].field))
            {
                if (table->s[i].large_line)
                    free (table->s[i].large_line);
                table->s[i].large_line = strdup (payload);
                break;
            }
        }
        if (nomatch_out && !table->s[i].large_line)
        {
            fprintf (nomatch_out, "%s%s%s\n",
                     field,
                     delim ? delim : "\t",
                     payload);
        }
    }
    LoseTable( char, line );
    fclose (in);
    if (nomatch_out)  fclose (nomatch_out);
}


int main (int argc, char** argv)
{
    const char* delim = 0;
    const char* dflt_record = 0;
    bool keep_join_field = true;
    bool large_on_left = false;
    FILE* nomatch_file = 0;
    FILE* small_file = 0;
    FILE* large_file = 0;
    FILE* out = stdout;
    int argi = 1;
    uint i;
    char* small_file_contents;
    Table(LineJoin) table;

    ExeName = argv[0];

    if (argi >= argc)
        show_usage_and_exit ();

    small_file = open_file_arg (argv[argi++], false);

    if (argi >= argc)
    {
        fprintf (ErrOut, "%s - Not enough arguments (need 2 files).\n",
                 ExeName);
        exit (1);
    }

    large_file = open_file_arg (argv[argi++], false);

    while (argi < argc)
    {
        const char* arg = argv[argi];
        ++ argi;
        if (0 == strcmp (arg, "-h"))
        {
            show_usage_and_exit ();
        }
        else if (0 == strcmp (arg, "-x"))
        {
            keep_join_field = false;
        }
        else if (0 == strcmp (arg, "-l"))
        {
            large_on_left = true;
        }
        else if (0 == strcmp (arg, "-d"))
        {
            if (argi >= argc)
            {
                fprintf (ErrOut, "%s - Delimiter (-d) needs an argument.\n",
                         ExeName);
                exit (1);
            }
            delim = argv[argi++];
            assert (strlen (delim) > 0);
        }
        else if (0 == strcmp (arg, "-p"))
        {
            if (argi >= argc)
            {
                fprintf (ErrOut, "%s - Need argument for default record (-p).\n",
                         ExeName);
                exit (1);
            }
            dflt_record = argv[argi++];
        }
        else if (0 == strcmp (arg, "--nomatch"))
        {
            if (argi >= argc)
            {
                fprintf (ErrOut, "%s - Need argument for nomatch file (--nomatch).\n",
                         ExeName);
                exit (1);
            }
            nomatch_file = open_file_arg (argv[argi++], true);
        }
        else
        {
            fprintf (ErrOut, "%s - Unknown argument:%s\n", ExeName, arg);
            show_usage_and_exit ();
        }
    }

    small_file_contents = read_FILE (small_file);
    table = setup_lookup_table (small_file_contents, delim);
    compare_lines (large_file, &table, delim, nomatch_file);

    if (!delim)  delim = "\t";
    UFor( i, table.sz )
    {
        LineJoin* join = &table.s[i];
        const char* large_line = join->large_line;
        if (!large_line && dflt_record)
            large_line = dflt_record;
        if (large_line)
        {
            if (keep_join_field)
            {
                fputs (join->field, out);
                fputs (delim, out);
            }
            if (large_on_left)
            {
                fputs (large_line, out);
                fputs (delim, out);
            }
            fputs (join->small_line, out);
            if (!large_on_left)
            {
                fputs (delim, out);
                fputs (large_line, out);
            }
            fputc ('\n', out);
        }
        if (join->large_line)
            free (join->large_line);
    }

    free (small_file_contents);
    LoseTable( LineJoin, table );
    return 0;
}

