
#ifndef AlphaTab_H_
#define AlphaTab_H_
#include "table.h"
typedef TableT(char) AlphaTab;
#define DeclTableT_AlphaTab
DeclTableT( AlphaTab, AlphaTab );

#define DEFAULT_AlphaTab  DEFAULT_Table

/* dflt_AlphaTab() - Jump down to this for AlphaTab functions.*/

qual_inline
  Sign
cmp_cstr (const char* a, const char* b)
{
  int ret;
  if (a == b)  return 0;
  if (!a)  return -1;
  if (!b)  return 1;
  ret = strcmp (a, b);
  return sign_of (ret);
}

qual_inline
  bool
eq_cstr (const char* a, const char* b)
{
  if (a == b)  return true;
  if (!a)  return false;
  if (!b)  return false;
  return (0 == strcmp (a, b));
}

  qual_inline bool
eql_cstr (const char* a, const char* b)
{ return eq_cstr (a, b); }

qual_inline
  bool
pfxeq_cstr (const char* pfx, const char* s)
{
  if (!s)  return false;
  return (0 == strncmp (pfx, s, strlen (pfx)));
}

qual_inline
  AlphaTab
dflt_AlphaTab ()
{
  AlphaTab t = DEFAULT_AlphaTab;
  return t;
}

qual_inline
  void
init_AlphaTab (AlphaTab* ab)
{
  *ab = dflt_AlphaTab ();
}

qual_inline
  AlphaTab
dflt1_AlphaTab (const char* s)
{
  AlphaTab t = DEFAULT_AlphaTab;
  t.s = (char*) s;
  if (s)
    t.sz = strlen (s) + 1;
  return t;
}

qual_inline
  AlphaTab
dflt2_AlphaTab (const char* s, zuint sz)
{
  AlphaTab t = DEFAULT_AlphaTab;
  t.s = (char*) s;
  t.sz = sz;
  return t;
}

qual_inline
void lose_AlphaTab (AlphaTab* ts) { LoseTable( *ts ); }

qual_inline
  void
cat_AlphaTab (AlphaTab* a, const AlphaTab* b)
{
  zuint n = b->sz;
  if (n == 0)  return;
  if (!b->s[n-1])  -- n;

  if (a->sz > 0)  -- a->sz;
  GrowTable( *a, n+1 );

  memcpy (&a->s[a->sz-(n+1)], b->s, n);
  a->s[a->sz-1] = 0;
}

qual_inline
  void
cat_cstr_AlphaTab (AlphaTab* t, const char* s)
{
  const AlphaTab b = dflt1_AlphaTab (s);
  cat_AlphaTab (t, &b);
}

qual_inline
  void
cat1_cstr_AlphaTab (AlphaTab* t, const char* s, zuint sz)
{
  const AlphaTab b = dflt2_AlphaTab (s, sz);
  cat_AlphaTab (t, &b);
}

qual_inline
  AlphaTab
cons1_AlphaTab (const char* s)
{
  AlphaTab a = DEFAULT_AlphaTab;
  cat_cstr_AlphaTab (&a, s);
  return a;
}

qual_inline
  char*
cstr_AlphaTab (AlphaTab* ts)
{
  if (ts->sz == 0 || ts->s[ts->sz-1] != '\0')
    PushTable( *ts, '\0' );
  return ts->s;
}

qual_inline
  const char*
ccstr_of_AlphaTab (const AlphaTab* ts)
{
  if (ts->sz == 0)
    return (char*) Static00;
  return ts->s;
}

qual_inline
  void
copy_AlphaTab (AlphaTab* a, const AlphaTab* b)
{
  CopyTable( *a, *b );
}

qual_inline
  void
copy_cstr_AlphaTab (AlphaTab* a, const char* s)
{
  AlphaTab b = dflt1_AlphaTab (s);
  CopyTable( *a, b );
}

qual_inline
  char*
forget_AlphaTab (AlphaTab* ts)
{
  char* s;
  PackTable( *ts );
  s = ts->s;
  *ts = dflt_AlphaTab ();
  return s;
}

qual_inline
  void
assign2_AlphaTab (AlphaTab* dst, const AlphaTab* src, zuint beg, zuint end)
{
  const zuint sz = (end - beg) - OneIf(beg!=end && src->s[end-1]=='\0');
  if (sz == 0) {
    ClearTable(*dst);
    return;
  }
  if (dst != src) {
    ResizeTable( *dst, sz+1 );
    memcpy (dst->s, &src->s[beg], sz);
  }
  else {
    if (beg != 0)
      memmove (dst->s, &src->s[beg], sz*sizeof(char));
    ResizeTable( *dst, sz+1 );
  }
  dst->s[sz] = '\0';
}

Sign
cmp_AlphaTab (const AlphaTab* a, const AlphaTab* b);
Sign
cmp_cstr_loc (const char* const* a, const char* const* b);
void
cat_uint_AlphaTab (AlphaTab* a, uint x);

#endif

