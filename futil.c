
#include "futil.h"

#include <stdlib.h>
#include <string.h>

    char*
read_FILE (FILE* in)
{
    const uint n_per_chunk = BUFSIZ;
    DeclTable( char, buf );
    uint off = 0;

    while (1)
    {
        size_t n;

        n = off + n_per_chunk + 1;
        if (buf.sz < n)
            GrowTable( char, buf, n - buf.sz );

        n = fread (&buf.s[off], sizeof(char), n_per_chunk, in);
        off += n;
        if (n < n_per_chunk)  break;
    }
    fclose (in);

    MPopTable( char, buf, buf.sz - off + 1 );
    buf.s[off] = '\0';
    return buf.s;
}

    uint
getline_FILE (FILE* in, Table(char)* line, uint off)
{
    const uint n_per_chunk = BUFSIZ;
    char* s = 0;

    if (off > 0)
    {
        size_t n;
        s = &line->s[off];
        n = strlen (s) + 1;
        memmove (line->s, s, n * sizeof (char));
        off = n - 1;
        s = strchr (line->s, '\n');
    }

    while (!s)
    {
        size_t n;

        n = off + n_per_chunk + 1;
        if (line->sz < n)
            GrowTable( char, *line, n - line->sz );

        s = &line->s[off];
        n = fread (s, sizeof (char), n_per_chunk, in);
        s[n] = 0;
        s = strchr (s, '\n');
        off += n;
        if (n == 0)  break;
    }

    off = 0;
    if (s)
    {
        if (s != line->s && s[-1] == '\r')
            s[-1] = '\0';
        if (s[0] != '\0')
        {
            s[0] = '\0';
            s = &s[1];
        }
        off = IndexInTable( char, *line, s );
    }

    return off;
}

