
#include "cx/syscx.h"
#include "cx/associa.h"
#include "cx/fileb.h"

#include <stdlib.h>
#include <string.h>

typedef struct LineJoin LineJoin;
struct LineJoin
{
    AlphaTab field;
    char* lookup_line;  /* From small file.*/
    char* stream_line;  /* From large file.*/
};

DeclTableT( LineJoin, LineJoin );

    void
init_LineJoin (LineJoin* join)
{
    join->field = dflt_AlphaTab ();
    join->lookup_line = 0;
    join->stream_line = 0;
}

    void
lose_LineJoin (LineJoin* join)
{
    if (join->stream_line && join->stream_line != join->field.s)
        free (join->stream_line);
    lose_AlphaTab (&join->field);
}

static void
show_usage_and_exit ()
{
    OFileB* of = stderr_OFileB ();
    printf_OFileB (of, "Usage: %s SMALL LARGE [OPTION]*\n",
                   exename_of_sysCx ());
#define f(s)  oput_cstr_OFileB (of, s); oput_char_OFileB (of, '\n')
    f("    SMALL is a file used for lookup.");
    f("    LARGE can be a stream, which tries to match the fields in SMALL.");
    f("    -x  Nix the join field.");
    f("    -l  Output from LARGE on the left.");
    f("    -ws  Use any whitespace as the delimiter.");
    f("    -d DELIM  Use a delimiter other than tab.");
    f("    -p DFLT  Default record to use when it is empty.");
    f("    -nomatch FILE  Put lines whose fields could not be matched here.");
    f("    -dupmatch FILE  Put fields who got matched here.");
#undef f
    failout_sysCx ("");
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
        DBog2( "Cannot open file for %s: %s\n",
               writing ? "writing" : "reading",
               arg );
        failout_sysCx ("");
    }
    return f;
}

static TableT(LineJoin)
setup_lookup_table (XFileB* xf, const char* delim)
{
    DeclTable( LineJoin, table );
    const uint delim_sz = delim ? strlen (delim) : 0;
    char* s;

    for (s = getline_XFileB (xf);
         s;
         s = getline_XFileB (xf))
    {
        LineJoin* join;

            /* Disregard a trailing empty line.*/
        if (s[0] == '\0')  break;

        join = Grow1Table( table );
        init_LineJoin (join);
        join->field = cons1_AlphaTab (s);
        s = cstr_AlphaTab (&join->field);
        if (delim)
            s = strstr (s, delim);
        else
            s = &s[strcspn (s, WhiteSpaceChars)];

        if (!s || s[0] == '\0')
            s = 0;

        if (s)
        {
            join->field.sz = IdxEltTable( join->field, s );
            s[0] = 0;
            if (delim)  s = &s[delim_sz];
            else        s = &s[1 + strspn (&s[1], WhiteSpaceChars)];
        }
        join->lookup_line = s;
    }

    PackTable( table );
    return table;
}

static void
compare_lines (XFileB* xf, Associa* map, const char* delim,
               FILE* nomatch_out, FILE* dupmatch_out)
{
    const uint delim_sz = delim ? strlen (delim) : 0;
    uint line_no = 0;
    char* line;

    for (line = getline_XFileB (xf);
         line;
         line = getline_XFileB (xf))
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
            AlphaTab ts = dflt1_AlphaTab (field);
            Assoc* assoc = lookup_Associa (map, &ts);
            join = (assoc ? *(LineJoin**) val_of_Assoc (assoc) : 0);
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
                DBog1( "Already a match for: %s", line );
            }
        }
        else if (!payload)
        {
            join->stream_line = join->field.s;
        }
        else
        {
            if (delim)
                payload = &payload[delim_sz];
            else
                payload = &payload[1 + strspn (&payload[1], WhiteSpaceChars)];

            join->stream_line = dup_cstr (payload);
        }
    }
    if (nomatch_out)  fclose (nomatch_out);
    if (dupmatch_out)  fclose (dupmatch_out);
}


int main (int argc, char** argv)
{
    int argi =
        (init_sysCx (&argc, &argv),
         1);
    const char* delim = "\t";
    const char* dflt_record = 0;
    bool keep_join_field = true;
    bool stream_on_left = false;
    FILE* nomatch_file = 0;
    FILE* dupmatch_file = 0;
    FileB lookup_in = dflt_FileB ();
    FileB stream_in = dflt_FileB ();
    FILE* out = stdout;
    uint i;
    TableT(LineJoin) table;
    DeclAssocia( AlphaTab, LineJoin*, map, (SwappedFn) swapped_AlphaTab );


    if (argi >= argc)
        show_usage_and_exit ();

    lookup_in.f = open_file_arg (argv[argi++], false);

    if (argi >= argc)
        failout_sysCx ("Not enough arguments (need 2 files).");

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
                failout_sysCx ("Output (-o) needs an argument.");
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
                failout_sysCx ("Delimiter (-d) needs an argument.\n");
            delim = argv[argi++];
            Claim2( strlen (delim) ,>, 0 );
        }
        else if (0 == strcmp (arg, "-p"))
        {
            if (argi >= argc)
                failout_sysCx ("Need argument for default record (-p).");
            dflt_record = argv[argi++];
        }
        else if (0 == strcmp (arg, "-nomatch"))
        {
            if (argi >= argc)
                failout_sysCx ("Need argument for nomatch file (-nomatch).");
            nomatch_file = open_file_arg (argv[argi++], true);
        }
        else if (0 == strcmp (arg, "-dupmatch"))
        {
            if (argi >= argc)
                failout_sysCx ("Need argument for dupmatch file (-dupmatch).");
            dupmatch_file = open_file_arg (argv[argi++], true);
        }
        else
        {
            DBog1( "Unknown argument: %s", arg );
            show_usage_and_exit ();
        }
    }

    table = setup_lookup_table (&lookup_in.xo, delim);
    lose_FileB (&lookup_in);
    { BLoop( i, table.sz )
        LineJoin* join = &table.s[i];
        insert_Associa (map, &join->field, &join);
    } BLose()
    flush_OFileB (stderr_OFileB ());
    compare_lines (&stream_in.xo, map, delim, nomatch_file, dupmatch_file);

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
                fputs (join->field.s, out);
                tab = true;
            }

            if (stream_on_left && stream_line != join->field.s)
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

            if (!stream_on_left && stream_line != join->field.s)
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

    LoseTable( table );
    lose_Associa (map);
    lose_sysCx ();
    return 0;
}

