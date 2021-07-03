
#include "xfile.h"
#include "alphatab.h"

  void
close_XFile (XFile* xf)
{
  VTCall( xf->vt, (void),close_fn,(xf) );
}

  void
lose_XFile (XFile* xf)
{
  LoseTable( xf->buf );
}

static
  bool
xget_chunk_XFile (XFile* xf)
{
  uint sz = xf->buf.sz;
  VTCall( xf->vt, (void),xget_chunk_fn,(xf) );
  return (sz < xf->buf.sz);
}

static
  void
flush_XFile (XFile* f)
{
  TableT(byte)* buf = &f->buf;
  Claim2( f->off ,<=, buf->sz );

  if (!f->vt && f->off + 1 == buf->sz) {
    LoseTable( f->buf );
    InitZTable( f->buf );
    f->off = 0;
    f->flushsz = 1;
    return;
  }

  if (f->off == 0)  return;
  buf->sz = buf->sz - f->off;
  if (buf->sz > 0)
    memmove (buf->s, &buf->s[f->off], buf->sz);
  f->off = 0;
}

static
  Trit
mayflush_XFile (XFile* xf, Trit may)
{
  bool old_mayflush = xf->mayflush;
  if (may == Yes)  xf->mayflush = true;

  if (xf->mayflush && xf->off >= xf->flushsz)
    flush_XFile (xf);

  if (may == Nil)  xf->mayflush = false;
  return (old_mayflush ? Yes : Nil);
}


  void
xget_XFile (XFile* xf)
{
  bool more = true;
  while (more)
    more = xget_chunk_XFile (xf);
}

  char*
getline_XFile (XFile* in)
{
  uint ret_off;
  char* s;

  mayflush_XFile (in, May);
  ret_off = in->off;
  s = strchr (cstr_XFile (in), '\n');

  while (!s)
  {
    in->off = in->buf.sz - 1;
    if (!xget_chunk_XFile (in))  break;
    s = strchr (cstr_XFile (in), '\n');
  }

  if (s)
  {
    s[0] = '\0';
    in->off = IdxElt( in->buf.s, s );
    if (in->off > ret_off && s[-1] == '\r')
      s[-1] = '\0';
    if (in->off + 1 < in->buf.sz)
      in->off += 1;
  }
  else
  {
    in->off = in->buf.sz - 1;
  }

  return (ret_off + 1 == in->buf.sz) ? 0 : (char*) &in->buf.s[ret_off];
}

static
  void
skipds_XFile (XFile* xf, const char* delims)
{
  char* s;
  if (!delims)  delims = WhiteSpaceChars;
  mayflush_XFile (xf, May);
  s = (char*) &xf->buf.s[xf->off];
  s = &s[strspn (s, delims)];

  while (!s[0])
  {
    if (!xget_chunk_XFile (xf))  break;
    mayflush_XFile (xf, May);
    s = (char*) &xf->buf.s[xf->off];
    s = &s[strspn (s, delims)];
  }
  xf->off = IdxEltTable( xf->buf, s );
  mayflush_XFile (xf, May);
}

/**
 * Get text up to the next delimiter (or NUL) without modifying the stream
 * position or content.
 *
 * \param delims Delimiters to check against.
 *   For default whitespace delimiters, simply pass NULL.
 *   For only NUL delimiter, pass an empty string.
 *   To skip NUL delimiters, prepend the string with "!!".
 *   A string of "!!" exactly will use whitespace charaters without NUL.
 * \return A pointer to the next delimiter or the NUL at the end
 *   of this buffer if no such delimiter can be found in the stream.
 *   The returned pointer will never be NULL itself.
 **/
static
  char*
tods_XFile (XFile* xfile, const char* delims)
{
  zuint off;
  const bool skip_nul = pfxeq_cstr ("!!", delims);
  /* Fix up {delims} to not be NULL in any case,
   * and skip over the "!!" if it exists.
   */
  if (skip_nul && delims[2])
    delims = &delims[2];
  else if (!delims || skip_nul)
    delims = WhiteSpaceChars;

  mayflush_XFile (xfile, May);
  off = xfile->off;
  Claim2( off ,<, xfile->buf.sz );
  Claim( !xfile->buf.s[xfile->buf.sz-1] );

  while (off+1 < xfile->buf.sz ||
      xget_chunk_XFile (xfile))
  {
    /* Recompute {s} pointer after file read! */
    char* s = cstr1_of_XFile (xfile, off);
    off += (delims[0]  ?  strcspn (s, delims)  :  strlen (s));
    if (off+1 == xfile->buf.sz)  continue;

    s = cstr1_of_XFile (xfile, off);
    if (s[0] || !skip_nul)  return s;
    off += 1;
  }
  return cstr1_of_XFile (xfile, xfile->buf.sz-1);
}

/**
 * Read text up to the next delimiter and replace it with a NUL.
 *
 * The stream position is moved past the next delimiter.
 *
 * \param delims See tods_XFile() description.
 * \return The text starting at the current stream offset.
 *
 * \sa nextok_XFile()
 **/
static
  char*
nextds_XFile (XFile* xfile, char* ret_match, const char* delims)
{
  char* s = tods_XFile(xfile, delims);
  const zuint ret_off = xfile->off;

  offto_XFile (xfile, s);
  if (ret_match)  *ret_match = s[0];

  if (xfile->off+1 < xfile->buf.sz) {
    /* Nullify and step over the delimiter. */
    s[0] = '\0';
    xfile->off += 1;
  }
  else {
    Claim2( xfile->off ,<, xfile->buf.sz);
  }

  if (ret_off < xfile->off)
    return cstr1_of_XFile (xfile, ret_off);

  Claim2( ret_off ,==, xfile->off );
  return 0;
}


/**
 * Read the next token.
 *
 * \param xf Input stream whose position will shifted be past the next token.
 * \return The next token. Will be NULL if no such token exists.
 **/
  char*
nextok_XFile (XFile* xf, char* ret_match, const char* delims)
{
  skipds_XFile (xf, delims);
  return nextds_XFile (xf, ret_match, delims);
}

  void
olay_txt_XFile (XFile* olay, XFile* xf, zuint off)
{
  const zuint end = (xf->off + 1 < xf->buf.sz  ?  xf->off  :  xf->buf.sz);
  Claim2( off ,<, end );

  init_XFile (olay);
  olay->buf.s = &xf->buf.s[off];
  olay->buf.sz = end - off;

  Claim( !olay->buf.s[olay->buf.sz-1] );
  while (olay->buf.sz > 1 && !olay->buf.s[olay->buf.sz-2]) {
    olay->buf.sz -= 1;
  }
}
