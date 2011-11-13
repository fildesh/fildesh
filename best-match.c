
#include <assert.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef unsigned int uint;

static char*
read_file (FILE* in)
{
    const uint n_per_chunk = 8192;
    uint nfull;
    uint n = 0;
    char* buf;

    nfull = n_per_chunk;
    buf = (char*) malloc (nfull * sizeof (char));

    while (1)
    {
        size_t nread;
        if (n + n_per_chunk > nfull)
        {
            nfull += n_per_chunk;
            buf = (char*) realloc (buf, nfull * sizeof(char));
        }
        nread = fread (&buf[n], sizeof(char), n_per_chunk, in);
        if (nread == 0)  break;
        n += nread;
    }
    fclose (in);

    buf = realloc (buf, n+1);
    buf[n] = '\0';
    return buf;
}

static char**
split_lines (char* buf, uint* ret_max_len)
{
    uint i;
    uint n = 1;
    uint max_len = 1;
    char* s = buf;
    char** lines;

    while (1)
    {
        s = strchr (s, '\n');
        if (!s)  break;
        ++n;
        s[0] = '\0';
        s = &s[1];
    }

    lines = (char**) malloc ((n+1) * sizeof (char**));
    lines[n] = 0;

    s = buf;
    for (i = 0; i < n; ++i)
    {
        uint len;
        lines[i] = s;
        len = strlen (s);
        if (len > max_len)  max_len = len;
        s = &s[1 + len];
    }

    *ret_max_len = max_len;
    return lines;
}

    /** Find the length of the longest common subsequence between /x/ and /y/.
     * /width/ of /a/ must not be less than the length of /y/.
     **/
static uint
lcs_count (uint* a, uint width, const char* x, const char* y)
{
    uint i, j, m, n;

    memset (a, 0, width * sizeof (uint));
    m = strlen (x);
    n = strlen (y);
    assert (n <= width);

    if (m == 0 || n == 0)  return 0;

    for (i = 0; i < m; ++i)
    {
        uint back_j = 0, back_ij = 0;
        char xc;
        xc = tolower (x[i]);
        for (j = 0; j < n; ++j)
        {
            uint back_i;
            char yc;
            yc = tolower (y[j]);
            back_i = a[j];

            if (xc == yc)
                a[j] = back_ij + 1;
            else if (back_j > back_i)
                a[j] = back_j;

            back_ij = back_i;
            back_j = a[j];
        }
    }

    return a[n-1];
}

static uint
matching_line (uint* a, uint width, const char* s, char* const* lines)
{
    uint max_count = 0;
    uint match_idx = 0;
    uint i;

    for (i = 0; lines[i]; ++i)
    {
        uint count;
        count = lcs_count (a, width, s, lines[i]);
        if (count > max_count)
        {
            max_count = count;
            match_idx = i;
        }
    }
    return match_idx;
}

int main (int argc, char** argv)
{
    uint* lcs_array;
    FILE* in;
    FILE* out;
    char* buf;
    char** lines;
    uint width;
    int arg_offset = 1;


    if (argc <= arg_offset)
    {
        fprintf (stderr, "%s: Require a file name.\n", argv[0]);
        return 1;
    }

    in = fopen (argv[arg_offset], "rb");
    if (!in)
    {
        fprintf (stderr, "%s: Cannot open table file: %s\n",
                 argv[0], argv[arg_offset]);
        return 1;
    }
    ++arg_offset;

    buf = read_file (in);
    lines = split_lines (buf, &width);
    lcs_array = (uint*) malloc (width * sizeof (uint));

    in = stdin;
    if (argc > arg_offset)
    {
        in = fopen (argv[arg_offset], "rb");
        if (!in)
        {
            fprintf (stderr, "%s: Cannot open query file: %s\n",
                     argv[0], argv[arg_offset]);
            return 1;
        }
        ++ arg_offset;
    }

    out = stdout;
    while (1)
    {
        uint i;
        const uint max_len = 8192;
        char line[8192];
        char* s;

        s = fgets (line, max_len, in);
        if (!s)  break;
        i = matching_line (lcs_array, width, line, lines);
        fputs (lines[i], out);
        fputc ('\n', out);
    }

    free (lcs_array);
    free (lines);
    free (buf);
    return 0;
}

