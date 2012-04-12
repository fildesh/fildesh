
#include "cx/assoc.h"
#include "cx/fileb.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

static const char* ExeName = 0;
#define ErrOut stderr

typedef struct LineJoin LineJoin;
struct LineJoin
{
    DeclAssocNodeField( LineJoin, LineJoin );
    char* field;
    char* lookup_line;  /* From small file.*/
    char* stream_line;  /* From large file.*/
};

DeclTableT( LineJoin, LineJoin );

static Trit
swapped_LineJoin (const LineJoin* lhs, const LineJoin* rhs)
{
    int ret = strcmp (lhs->field, rhs->field);
    return ((ret < 0) ? Nil : ((ret > 0) ? Yes : May));
}

DeclAssocT( LineJoin, LineJoin, swapped_LineJoin );

    void
init_LineJoin (LineJoin* join)
{
    join->field = 0;
    join->lookup_line = 0;
    join->stream_line = 0;
}

    void
lose_LineJoin (LineJoin* join)
{
    if (join->field)
        free (join->field);
    if (join->stream_line && join->stream_line != join->field)
        free (join->stream_line);
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
setup_lookup_table (FileB* in, const char* delim)
{
    DeclTable( LineJoin, table );
    const uint delim_sz = delim ? strlen (delim) : 0;
    char* s;

    for (s = getline_FileB (in);
         s;
         s = getline_FileB (in))
    {
        LineJoin* join;

            /* Disregard a trailing empty line.*/
        if (s[0] == '\0')  break;

        GrowTable( LineJoin, table, 1 );
        join = &table.s[table.sz-1];
        init_LineJoin (join);
        s = strdup (s);
        join->field = s;
        if (delim)
            s = strstr (s, delim);
        else
            s = &s[strcspn (s, WhiteSpaceChars)];

        if (!s || s[0] == '\0')
            s = 0;

        if (s)
        {
            s[0] = 0;
            if (delim)  s = &s[delim_sz];
            else        s = &s[1 + strspn (&s[1], WhiteSpaceChars)];
        }
        join->lookup_line = s;
    }

    PackTable( LineJoin, table );
    return table;
}

static void
compare_lines (FileB* in, Assoc(LineJoin)* assoc, const char* delim,
               FILE* nomatch_out, FILE* dupmatch_out)
{
    const uint delim_sz = delim ? strlen (delim) : 0;
    uint line_no = 0;
    char* line;

    for (line = getline_FileB (in);
         line;
         line = getline_FileB (in))
    {
        char* field = line;
        char* payload;
        char nixed_char = '\0';
        LineJoin* join = 0;

        ++ line_no;

        if (delim)  payload = strstr (line, delim);
        else        payload = &line[strcspn (line, WhiteSpaceChars)];

        if (!payload || payload[0] == '\0')
            payload = 0;

        if (payload)
        {
            nixed_char = payload[0];
            payload[0] = '\0';
        }

        {
            LineJoin tmp;
            tmp.field = field;
            join = GetAssoc( LineJoin, *assoc, tmp );
        }

        if (!join)
        {
            if (nomatch_out)
            {
                if (payload)  payload[0] = nixed_char;
                fputs (line, nomatch_out);
                fputc ('\n', nomatch_out);
            }
        }
        else if (join->stream_line)
        {
            if (payload)  payload[0] = nixed_char;
            if (dupmatch_out)
            {
                fputs (line, dupmatch_out);
                fputc ('\n', dupmatch_out);
            }
            else
            {
                fprintf (ErrOut, "Already a match for:%s\n", line);
            }
        }
        else if (!payload)
        {
            join->stream_line = join->field;
        }
        else
        {
            if (delim)
                payload = &payload[delim_sz];
            else
                payload = &payload[1 + strspn (&payload[1], WhiteSpaceChars)];

            join->stream_line = strdup (payload);
        }
    }
    if (nomatch_out)  fclose (nomatch_out);
    if (dupmatch_out)  fclose (dupmatch_out);
}


int main (int argc, char** argv)
{
    const char* delim = "\t";
    const char* dflt_record = 0;
    bool keep_join_field = true;
    bool stream_on_left = false;
    FILE* nomatch_file = 0;
    FILE* dupmatch_file = 0;
    FileB lookup_in;
    FileB stream_in;
    FILE* out = stdout;
    int argi = 1;
    uint i;
    Table(LineJoin) table;
    Assoc(LineJoin) assoc;

    ExeName = argv[0];

    init_FileB (&lookup_in);
    init_FileB (&stream_in);

    if (argi >= argc)
        show_usage_and_exit ();

    lookup_in.f = open_file_arg (argv[argi++], false);

    if (argi >= argc)
    {
        fprintf (ErrOut, "%s - Not enough arguments (need 2 files).\n",
                 ExeName);
        exit (1);
    }

    stream_in.f = open_file_arg (argv[argi++], false);

    while (argi < argc)
    {
        const char* arg = argv[argi];
        ++ argi;
        if (0 == strcmp (arg, "-h"))
        {
            show_usage_and_exit ();
        }
        else if (0 == strcmp (arg, "-o"))
        {
            if (argi >= argc)
            {
                fprintf (ErrOut, "%s - Output (-o) needs an argument.\n",
                         ExeName);
                exit (1);
            }
            out = open_file_arg (argv[argi++], true);
        }
        else if (0 == strcmp (arg, "-x"))
        {
            keep_join_field = false;
        }
        else if (0 == strcmp (arg, "-l"))
        {
            stream_on_left = true;
        }
        else if (0 == strcmp (arg, "-ws"))
        {
            delim = 0;
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
        else if (0 == strcmp (arg, "-nomatch"))
        {
            if (argi >= argc)
            {
                fprintf (ErrOut, "%s - Need argument for nomatch file (-nomatch).\n",
                         ExeName);
                exit (1);
            }
            nomatch_file = open_file_arg (argv[argi++], true);
        }
        else if (0 == strcmp (arg, "-dupmatch"))
        {
            if (argi >= argc)
            {
                fprintf (ErrOut, "%s - Need argument for nomatch file (-dupmatch).\n",
                         ExeName);
                exit (1);
            }
            dupmatch_file = open_file_arg (argv[argi++], true);
        }
        else
        {
            fprintf (ErrOut, "%s - Unknown argument:%s\n", ExeName, arg);
            show_usage_and_exit ();
        }
    }

    load_FileB (&lookup_in);
    table = setup_lookup_table (&lookup_in, delim);
    lose_FileB (&lookup_in);
    InitAssoc( LineJoin, assoc );
    UFor( i, table.sz )
        SetfAssoc( LineJoin, assoc, table.s[i] );
    compare_lines (&stream_in, &assoc, delim, nomatch_file, dupmatch_file);
    lose_FileB (&stream_in);

    if (!delim)  delim = "\t";
    UFor( i, table.sz )
    {
        LineJoin* join = &table.s[i];
        const char* stream_line = join->stream_line;
        if (!stream_line && dflt_record)
            stream_line = dflt_record;
        if (stream_line)
        {
            bool tab = false;
            if (keep_join_field)
            {
                fputs (join->field, out);
                tab = true;
            }

            if (stream_on_left && stream_line != join->field)
            {
                if (tab)  fputs (delim, out);
                tab = true;
                fputs (stream_line, out);
            }

            if (join->lookup_line)
            {
                if (tab)  fputs (delim, out);
                tab = true;
                fputs (join->lookup_line, out);
            }

            if (!stream_on_left && stream_line != join->field)
            {
                if (tab)  fputs (delim, out);
                tab = true;
                fputs (stream_line, out);
            }
            fputc ('\n', out);
        }
        lose_LineJoin (join);
    }
    fclose (out);

    LoseTable( LineJoin, table );
    return 0;
}

