
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

    void
init_FileB (FileB* f)
{
    f->f = 0;
    InitTable( char, f->buf );
    f->off = 0;
}

    void
lose_FileB (FileB* f)
{
    if (f->f)  fclose (f->f);
    LoseTable( char, f->buf );
}

    void
close_FileB (FileB* f)
{
    lose_FileB (f);
    init_FileB (f);
}

    char*
read_FileB (FileB* in)
{
    char* s = read_FILE (in->f);
    in->f = 0;
    close_FileB (in);
    in->buf.s = s;
    in->buf.sz = strlen (s);
    in->buf.alloc_sz = in->buf.sz + 1;
    return s;
}

    char*
getline_FileB (FileB* in)
{
    const uint n_per_chunk = BUFSIZ;
    char* s = 0;
    uint off = in->off;

    if (off > 0)
    {
        size_t n;
        s = &in->buf.s[off];
        n = strlen (s) + 1;
        memmove (in->buf.s, s, n * sizeof (char));
        off = n - 1;
        s = strchr (in->buf.s, '\n');
    }
    else if (!in->f)
    {
        s = strchr (in->buf.s, '\n');
    }

    if (in->f)  while (!s)
    {
        size_t n;

        n = off + n_per_chunk + 1;
        if (in->buf.sz < n)
            GrowTable( char, in->buf, n - in->buf.sz );

        s = &in->buf.s[off];
        n = fread (s, sizeof (char), n_per_chunk, in->f);
        s[n] = 0;
        s = strchr (s, '\n');
        off += n;
        if (n == 0)  break;
    }

    off = 0;
    if (s)
    {
        if (s != in->buf.s && s[-1] == '\r')
            s[-1] = '\0';
        if (s[0] != '\0')
        {
            s[0] = '\0';
            s = &s[1];
        }
        off = IndexInTable( char, in->buf, s );
    }

    in->off = off;
    return (off == 0) ? 0 : in->buf.s;
}

