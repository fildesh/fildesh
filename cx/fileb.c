
#include "fileb.h"

#include <assert.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

#ifndef DeclTableT_byte
#define DeclTableT_byte
DeclTableT( byte, byte );
#endif

static bool
load_chunk_FileB (FileB* f);
static inline bool
dump_chunk_FileB (FileB* f);
static void
dumpn_raw_byte_FileB (FileB* f, const byte* a, TableSzT(byte) n);

    void
init_FileB (FileB* f)
{
    static char empty[1] = { 0 };
    f->f = 0;
    InitTable( f->buf );
    f->buf.s = (byte*) empty;
    f->buf.sz = 1;
    f->good = true;
    f->off = 0;
    f->sink = false;
    f->byline = false;
    f->fmt = FileB_Ascii;
    InitTable( f->pathname );
    InitTable( f->filename );
}

    void
close_FileB (FileB* f)
{
    if (f->sink)  flusho_FileB (f);
    if (f->f)
    {
        fclose (f->f);
        f->f = 0;
    }
}

    void
lose_FileB (FileB* f)
{
    close_FileB (f);
    LoseTable( f->buf );
    LoseTable( f->pathname );
    LoseTable( f->filename );
}

    void
seto_FileB (FileB* f, bool sink)
{
    Claim( !f->f );
    f->sink = sink;
}

static inline
    TableSzT(byte)
chunksz_FileB (FileB* f)
{
    static const uint NPerChunk = BUFSIZ;
    (void) f;
    return NPerChunk;
}

    byte*
ensure_FileB (FileB* f, TableSzT(byte) n)
{
    TableSzT(byte) sz;
    if (f->sink)
    {
        dump_chunk_FileB (f);
        SizeUpTable( f->buf, f->off+n );
        return &f->buf.s[f->off];
    }

    sz = f->buf.sz;
    if (nullt_FileB (f))
    {
        Claim2( 0 ,<, sz );
        sz -= 1;
    }
    GrowTable( f->buf, n );
    return &f->buf.s[sz];
}

    void
setfmt_FileB (FileB* f, FileB_Format fmt)
{
    bool nullt0, nullt1;
    if (f->sink)
    {
        f->fmt = fmt;
        return;
    }
    nullt0 = nullt_FileB (f);
    f->fmt = fmt;
    nullt1 = nullt_FileB (f);
    if (nullt0 != nullt1)
    {
        if (nullt0)  f->off += 1;
        else         f->buf.sz -= 1;
    }
}

static
    uint
pathname_sz (const char* path)
{
    const char* s;
    s = strrchr (path, '/');
    if (!s)  return 0;
    return 1 + IdxElt( path, s );
}

static
    bool
absolute_path (const char* path)
{
    return path[0] == '/';
}

    bool
open_FileB (FileB* f, const char* pathname, const char* filename)
{
    uint pflen = pathname_sz (filename);
    uint flen = strlen (filename) - pflen;
    uint plen = (pathname ? strlen (pathname) : 0);
    char* s;

    SizeTable( f->filename, flen+1 );
    memcpy (f->filename.s, &filename[pflen], (flen+1)*sizeof(char));

    if (pflen > 0 && absolute_path (filename))
        plen = 0;

    SizeTable( f->pathname, plen+1+pflen+flen+1 );

    s = f->pathname.s;
    if (plen > 0)
    {
        memcpy (s, pathname, plen*sizeof(char));
        s = &s[plen];
        s[0] = '/';
        s = &s[1];
    }
    memcpy (s, filename, (pflen+flen+1)*sizeof(char));

    f->f = fopen (f->pathname.s, (f->sink ? "wb" : "rb"));

    plen += pflen;
    f->pathname.s[plen] = 0;
    SizeTable( f->pathname, plen+1 );

    return !!f->f;
}

    void
set_FILE_FileB (FileB* f, FILE* file)
{
    Claim( !f->f );
    f->f = file;
}

    void
olay_FileB (FileB* olay, FileB* source)
{
    init_FileB (olay);
    olay->buf.s = source->buf.s;
    olay->buf.sz = source->off;
    if (source->sink)
        olay->buf.sz = source->off + 1;
    else if (olay->buf.sz == source->buf.sz - 1)
        olay->buf.sz = source->buf.sz;
}

    char*
load_FileB (FileB* f)
{
    bool good = true;
    long ret = 0;

    if (good && (good = !!f->f))
    {
        ret = fseek (f->f, 0, SEEK_END);
    }

        /* Some streams cannot be seeked.*/
    if (good && ret != 0)
    {
        bool more = true;
        while (more)
            more = load_chunk_FileB (f);
    }
    else
    {
        size_t sz = 0;
        if (good && (good = (ret == 0)))
        {
            ret = ftell (f->f);
        }
        if (good && (good = (ret >= 0)))
        {
            sz = ret;
            ret = fseek (f->f, 0, SEEK_SET);
        }
        if (good && (good = (ret == 0)))
        {
            GrowTable( f->buf, sz );

                /* Note this relation!*/
            Claim2( f->off + sz ,==, f->buf.sz-1 );

            ret = fread (&f->buf.s[f->off], 1, sz, f->f);
            if (ret >= 0)
                f->buf.s[f->off + ret] = '\0';

            good = (ret == (long)sz);
        }
    }

    close_FileB (f);

    if (good)
    {
        char* s = (char*) &f->buf.s[f->off];
        f->off = f->buf.sz-1;
        return s;
    }
    return NULL;
}

    bool
load_chunk_FileB (FileB* f)
{
    const TableSzT(byte) chunksz = chunksz_FileB (f);
    TableT(byte)* buf = &f->buf;
    size_t n;
    byte* s;

    if (!f->f)  return false;

    s = ensure_FileB (f, chunksz);

    if (byline_FileB (f))
    {
        char* line = (char*) s;
        Claim( nullt_FileB (f) );
            /* Don't worry about actually reading a full line here,
             * that's at a higher level.
             * We just want to avoid deadlock by stopping at a newline.
             */
        line = fgets (line, chunksz, f->f);
        n = (line ? strlen (line) : 0);
    }
    else
    {
        n = fread (s, 1, chunksz, f->f);
    }
    if (nullt_FileB (f))
        s[n] = 0;
    buf->sz -= (chunksz - n);
    return (n != 0);
}

    void
flushx_FileB (FileB* f)
{
    TableT(byte)* buf = &f->buf;
    if (nullt_FileB (f))
    {
        Claim2( 0 ,<, buf->sz );
        Claim2( 0 ,==, buf->s[buf->sz-1] );
        Claim2( f->off ,<, buf->sz );
    }
    else
    {
        Claim2( f->off ,<=, buf->sz );
    }
    if (f->off == 0)  return;
    buf->sz = buf->sz - f->off;
    if (buf->sz > 0)
    {
        memmove (buf->s, &buf->s[f->off], buf->sz);
    }
    else if (nullt_FileB (f))
    {
        buf->s[0] = 0;
        buf->sz = 1;
    }
    f->off = 0;
}

    char*
getline_FileB (FileB* in)
{
    char* s;

    flushx_FileB (in);
    s = strchr (cstr_FileB (in), '\n');

    while (!s)
    {
        uint off = in->buf.sz - 1;
        if (!load_chunk_FileB (in))  break;
        s = strchr ((char*) &in->buf.s[off], '\n');
    }

    if (s)
    {
        s[0] = '\0';
        if (s != (char*) in->buf.s && s[-1] == '\r')
            s[-1] = '\0';
        s = &s[1];
        in->off = IdxElt( in->buf.s, s );
    }
    else
    {
        in->off = in->buf.sz - 1;
    }

    return (in->buf.sz == 1) ? 0 : (char*) in->buf.s;
}

    char*
getlined_FileB (FileB* in, const char* delim)
{
    char* s;
    uint delim_sz = strlen (delim);

    flushx_FileB (in);
    s = strstr ((char*) in->buf.s, delim);

    while (!s)
    {
        uint off = in->buf.sz - 1;
        if (!load_chunk_FileB (in))  break;

        s = (char*) in->buf.s;
        if (off >= delim_sz)
            s = &s[1+off-delim_sz];
        s = strstr (s, delim);
    }

    if (s)
    {
        s[0] = '\0';
        s = &s[delim_sz];
        in->off = IdxElt( in->buf.s, s );
    }
    else
    {
        in->off = in->buf.sz - 1;
    }

    return (in->buf.sz == 1) ? 0 : (char*) in->buf.s;
}

    void
skipds_FileB (FileB* in, const char* delims)
{
    char* s;
    if (!delims)  delims = WhiteSpaceChars;
    flushx_FileB (in);

    s = cstr_FileB (in);
    s = &s[strspn (s, delims)];

    while (!s[0])
    {
        flushx_FileB (in);
        if (!load_chunk_FileB (in))  break;
        s = cstr_FileB (in);
        s = &s[strspn (s, delims)];
    }
    in->off = IdxElt( in->buf.s, s );
    flushx_FileB (in);
}

    char*
nextds_FileB (FileB* in, char* ret_match, const char* delims)
{
    char* s;
    if (!delims)  delims = WhiteSpaceChars;
    flushx_FileB (in);

    s = cstr_FileB (in);
    s = &s[strcspn (s, delims)];

    while (!s[0])
    {
        uint off = in->buf.sz - 1;
        if (!load_chunk_FileB (in))  break;
        s = (char*) &in->buf.s[off];
        s = &s[strcspn (s, delims)];
    }

    if (ret_match)  *ret_match = s[0];
    if (s[0])
    {
        s[0] = 0;
        ++ s;
        in->off = IdxElt( in->buf.s, s );
    }
    else
    {
        in->off = in->buf.sz - 1;
    }

    return (in->buf.sz == 1) ? 0 : (char*) in->buf.s;
}

    char*
nextok_FileB (FileB* in, char* ret_match, const char* delims)
{
    skipds_FileB (in, delims);
    return nextds_FileB (in, ret_match, delims);
}

    /** Inject content from a file /src/
     * at the current read position of file /in/.
     * This allows a trivial implementation of #include.
     **/
    void
inject_FileB (FileB* in, FileB* src, const char* delim)
{
    uint delim_sz = strlen (delim);
    uint sz;

    load_FileB (src);
    Claim2( src->buf.sz ,>, 0 );
    sz = in->buf.sz - in->off;

    GrowTable( in->buf, src->buf.sz-1 + delim_sz );
        /* Make room for injection.*/
    memmove (&in->buf.s[in->off + src->buf.sz-1 + delim_sz],
             &in->buf.s[in->off],
             sz * sizeof (char));
        /* Inject file contents, assume src->buf.sz is strlen!*/
    memcpy (&in->buf.s[in->off],
            src->buf.s,
            (src->buf.sz-1) * sizeof (char));

        /* Add the delimiter at the end.*/
    if (delim_sz > 0)
        memcpy (&in->buf.s[in->off + src->buf.sz-1],
                delim,
                delim_sz * sizeof (char));
}

    void
skipto_FileB (FileB* in, const char* pos)
{
    in->off = IdxElt( in->buf.s, pos );
}

static
    bool
fdump_FileB (FileB* f, const byte* a, uint n)
{
    size_t nout;
    nout = fwrite (a, 1, n, f->f);
    return (nout == n);
}

static inline
    bool
selfcont_FileB (FileB* f)
{
    return (!f->f);
}

static
    bool
flusho1_FileB (FileB* f, const byte* a, uint n)
{
    bool good = true;
    if (selfcont_FileB (f))
    {
        if (n == 0)  return true;
        GrowTable( f->buf, n );
        memcpy (&f->buf.s[f->off], a, n);
        f->off += n;
    }
    else
    {
        if (f->off > 0)
        {
            good = fdump_FileB (f, f->buf.s, f->off);
            if (!good)  return false;
            f->buf.sz = 1;
            f->off = 0;
        }
        if (n > 0)
        {
            good = fdump_FileB (f, a, n);
            if (!good)  return false;
        }
    }


    if (nullt_FileB (f))
    {
            /* Not sure why...*/
        f->buf.s[f->off] = 0;
    }
    return true;
}

    bool
flusho_FileB (FileB* f)
{
    return flusho1_FileB (f, 0, 0);
}

    bool
dump_chunk_FileB (FileB* f)
{
    if (f->off < chunksz_FileB (f))  return true;
        /* In the future, we may not want to flush all the time!*/
        /* Also, we may not wish to flush the whole buffer.*/
    return flusho_FileB (f);
}

    void
dump_uint_FileB (FileB* f, uint x)
{
    SizeUpTable( f->buf, f->off + 50 );
    f->off += sprintf ((char*)&f->buf.s[f->off], "%u", x);
    dump_chunk_FileB (f);
}

    void
dump_real_FileB (FileB* f, real x)
{
    SizeUpTable( f->buf, f->off + 50 );
    f->off += sprintf ((char*)&f->buf.s[f->off], "%.16e", x);
    dump_chunk_FileB (f);
}

    void
dump_char_FileB (FileB* f, char c)
{
    SizeUpTable( f->buf, f->off + 2 );
    f->buf.s[f->off] = c;
    f->buf.s[++f->off] = 0;
    dump_chunk_FileB (f);
}

    void
dump_cstr_FileB (FileB* f, const char* s)
{
    uint n = strlen (s);
    GrowTable( f->buf, n );
    memcpy (&f->buf.s[f->off], s, (n+1)*sizeof(char));
    f->off += n;
    dump_chunk_FileB (f);
}

    void
vprintf_FileB (FileB* f, const char* fmt, va_list args)
{
  uint sz = 2048;  /* Not good :( */
  int iret = 0;

  SizeUpTable( f->buf, f->off + sz );
  iret = vsprintf ((char*) &f->buf.s[f->off], fmt, args);
  Claim2( iret ,>=, 0 );
  Claim2( (uint) iret ,<=, sz );
  f->off += iret;
}

    void
printf_FileB (FileB* f, const char* fmt, ...)
{
  va_list args;
  va_start (args, fmt);
  vprintf_FileB (f, fmt, args);
  va_end (args);
}

    void
dumpn_raw_byte_FileB (FileB* f, const byte* a, TableSzT(byte) n)
{
    const TableSzT(byte) ntotal = f->off + n;
    if (ntotal <= allocsz_Table ((Table*) &f->buf))
    {
        memcpy (&f->buf.s[f->off], a, n);
        f->off = ntotal;
        if (f->off > f->buf.sz)
            f->buf.sz = f->off;
    }
    else if (ntotal <= 2*chunksz_FileB (f))
    {
        SizeUpTable( f->buf, 2*chunksz_FileB (f) );
        memcpy (&f->buf.s[f->off], a, n);
        f->off = ntotal;
    }
    else
    {
        flusho1_FileB (f, a, n);
    }
}

    void
dumpn_byte_FileB (FileB* f, const byte* a, TableSzT(byte) n)
{
    if (f->fmt == FileB_Raw)
    {
        dumpn_raw_byte_FileB (f, a, n);
        return;
    }
    { BLoop( i, n )
        dump_uint_FileB (f, a[i]);
        if (i+1 < n)
            dump_char_FileB (f, ' ');
    } BLose()
}

    void
dumpn_char_FileB (FileB* f, const char* a, TableSzT(byte) n)
{
    GrowTable( f->buf, n );
    memcpy (&f->buf.s[f->off], a, (n+1)*sizeof(char));
    f->off += n;
    dump_chunk_FileB (f);
}

    char*
load_uint_cstr (uint* ret, const char* in)
{
    unsigned long v;
    char* out = 0;

    assert (ret);
    assert (in);
    v = strtoul (in, &out, 10);

    if (v > Max_uint)  out = 0;
    if (out == in)  out = 0;
    if (out)  *ret = (uint) v;
    return out;
}

    char*
load_real_cstr (real* ret, const char* in)
{
    double v;
    char* out = 0;

    assert (ret);
    assert (in);
    v = strtod (in, &out);

    if (out == in)  out = 0;
    if (out)  *ret = (real) v;
    return out;
}

    bool
load_uint_FileB (FileB* f, uint* x)
{
    const char* s;
    if (!f->good)  return false;
    if (nullt_FileB (f))
    {
        skipds_FileB (f, 0);
        if (f->buf.sz - f->off < 50)
            load_chunk_FileB (f);
        s = load_uint_cstr (x, (char*)&f->buf.s[f->off]);
        f->good = !!s;
        if (!f->good)  return false;
        f->off = IdxElt( f->buf.s, s );
    }
    else
    {
        union Castless {
            uint x;
            byte b[sizeof(uint)];
        } y;
        f->good = loadn_byte_FileB (f, y.b, sizeof(uint));
        if (!f->good)  return false;
        *x = y.x;
    }
    return true;
}

    bool
load_real_FileB (FileB* f, real* x)
{
    const char* s;
    if (!f->good)  return false;
        /* if (f->buf.sz - f->off < 50) load_chunk_FileB (f); */
    s = load_real_cstr (x, (char*)&f->buf.s[f->off]);
    f->good = !!s;
    if (!f->good)  return false;
    f->off = IdxElt( f->buf.s, s );
    return true;
}

static
    bool
loadn_raw_byte_FileB (FileB* f, byte* a, TableSzT(byte) n)
{
    Claim2( f->fmt ,==, FileB_Raw );
    flushx_FileB (f);
    while (n > 0)
    {
        TableSzT(byte) m;
        if (f->buf.sz == 0)
            f->good = load_chunk_FileB (f);
        if (!f->good)  return false;
        m = (n < f->buf.sz ? n : f->buf.sz);
        memcpy (a, f->buf.s, m);
        f->off = m;
        flushx_FileB (f);
        a = &a[m];
        n -= m;
    }
    return true;
}

    bool
loadn_byte_FileB (FileB* f, byte* a, TableSzT(byte) n)
{
    if (f->fmt == FileB_Raw)
        return loadn_raw_byte_FileB (f, a, n);

    while (n > 0)
    {
        uint y;
        f->good = load_uint_FileB (f, &y);
        if (!f->good)  return false;
        a[0] = (byte) y;
        a = &a[1];
        n -= 1;
    }
    return true;
}

