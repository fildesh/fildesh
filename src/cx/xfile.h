/*
 * \file xfile.h
 */
#ifndef XFile_H_
#define XFile_H_
#include "alphatab.h"

typedef struct XFile XFile;
typedef struct XFileCtx XFileCtx;
typedef struct XFileVT XFileVT;

struct XFile
{
  TableT(byte) buf;
  zuint off;
  zuint flushsz;
  bool mayflush;
  const XFileVT* vt;
  XFileCtx* ctx;
};
#define DEFAULT3_XFile(flushsz, mayflush, vt) \
{ DEFAULT_Z_Table(byte), 0, flushsz, mayflush, vt, 0 }
#define DEFAULT_XFile  DEFAULT3_XFile(1,false,0)


struct XFileCtx
{
  byte nothing;
};


struct XFileVT
{
  bool (*xget_chunk_fn) (XFile*);
  /*void (*flush_fn) (XFile);*/

  void (*close_fn) (XFile*);
  void (*free_fn) (XFile*);
};
#define DEFAULT3_XFileVT(flush_fn, close_fn, free_fn) \
{ flush_fn, close_fn, free_fn \
}

void
close_XFile (XFile* xf);
void
lose_XFile (XFile* xf);

void
xget_XFile (XFile* xf);
char*
getline_XFile (XFile* in);
char*
nextok_XFile (XFile* xf, char* ret_match, const char* delims);

void
olay_txt_XFile (XFile* olay, XFile* xf, zuint off);

qual_inline
  void
init_data_XFile (XFile* xf)
{
  InitZTable( xf->buf );
  xf->off = 0;
  xf->flushsz = 1;
  xf->mayflush = false;
}

qual_inline
  void
init_XFile (XFile* xf)
{
  init_data_XFile (xf);
  xf->vt = 0;
  xf->ctx = 0;
}

qual_inline
  void
olay_XFile (XFile* olay, XFile* xf, zuint off)
{
  init_XFile (olay);
  olay->buf.s = &xf->buf.s[off];
  olay->buf.sz = xf->off - off;
}

qual_inline
  void
offto_XFile (XFile* xf, const char* pos)
{ xf->off = IdxElt( xf->buf.s, pos ); }

qual_inline
  const char*
ccstr1_of_XFile (const XFile* xf, zuint off)
{ return (char*) &xf->buf.s[off]; }

qual_inline
  const char*
ccstr_of_XFile (const XFile* xf)
{ return ccstr1_of_XFile (xf, xf->off); }

qual_inline
  char*
cstr1_of_XFile (XFile* f, zuint off)
{ return (char*) &f->buf.s[off]; }

qual_inline
  char*
cstr_of_XFile (XFile* xf)
{ return cstr1_of_XFile (xf, xf->off); }

qual_inline
  char*
cstr1_XFile (XFile* f, zuint off)
{ return cstr1_of_XFile (f, off); }

qual_inline
  char*
cstr_XFile (XFile* xf)
{ return cstr1_of_XFile (xf, xf->off); }

qual_inline
  AlphaTab
AlphaTab_XFile (XFile* xf, zuint off)
{
  AlphaTab t = DEFAULT_AlphaTab;
  t.s = (char*) &xf->buf.s[off];
  t.sz = (xf->off - off) / sizeof(char);
  return t;
}

/** Get a window into the XFile content.
 * \param beg  Inclusive beginning index.
 * \param end  Non-inclusive end index.
 **/
qual_inline
  AlphaTab
window2_XFile (XFile* xfile, zuint beg, zuint end)
{
  AlphaTab t = DEFAULT_AlphaTab;
  Claim2( beg ,<=, end );
  Claim2( end ,<=, xfile->buf.sz );
  if (end < xfile->buf.sz && xfile->buf.s[end] == 0) {
    ++ end;
  }
  t.s = (char*) &xfile->buf.s[beg];
  t.sz = end - beg;
  return t;
}

qual_inline
  void
olay2_txt_XFile (XFile* olay, XFile* xf, zuint beg, zuint end)
{
  init_XFile (olay);
  olay->buf.s = &xf->buf.s[beg];
  olay->buf.sz = end - beg;
}

qual_inline
  void
init_AlphaTab_move_XFile (AlphaTab* t, XFile* xf)
{
  *t = AlphaTab_XFile (xf, 0);
  t->alloc_lgsz = xf->buf.alloc_lgsz;
  init_data_XFile (xf);
  PackTable( *t );
}

qual_inline
  void
init_XFile_olay_AlphaTab (XFile* xf, AlphaTab* ts)
{
  init_XFile (xf);
  xf->buf.s = (byte*) cstr_AlphaTab (ts);
  xf->buf.sz = ts->sz;
}

qual_inline
  void
init_XFile_move_AlphaTab (XFile* xf, AlphaTab* ts)
{
  init_XFile (xf);
  if (ts->sz == 0 || ts->s[0]=='\0') {
    lose_AlphaTab (ts);
    *ts = dflt_AlphaTab ();
    return;
  }
  xf->buf.s = (byte*) cstr_AlphaTab (ts);
  xf->buf.sz = ts->sz;
  xf->buf.alloc_lgsz = ts->alloc_lgsz;
  PackTable( xf->buf );
  *ts = dflt_AlphaTab ();
  xf->mayflush = true;
  xf->flushsz = xf->buf.sz-1;
}

qual_inline
  void
init_XFile_olay_cstr (XFile* xf, char* s)
{
  init_XFile (xf);
  xf->buf.s = (byte*) s;
  xf->buf.sz = strlen(s)+1;
}

qual_inline
  void
putlast_char_XFile (XFile* xfile, char c)
{
  if (xfile->off > 0 && (char)xfile->buf.s[xfile->off - 1] == '\0') {
    xfile->buf.s[xfile->off - 1] = c;
  }
}

#endif

