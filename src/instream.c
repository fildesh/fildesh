
#include "lace.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

  size_t
read_LaceX(LaceX* in)
{
  if (in->vt && in->vt->read_fn) {
    size_t old_sz = in->buf.sz;
    in->vt->read_fn(in);
    return in->buf.sz - old_sz;
  }
  return 0;
}

  void
close_LaceX(LaceX* in)
{
  if (in->vt && in->vt->close_fn) {
    in->vt->close_fn(in);
  }
  if (in->buf.at) {
    free(in->buf.at);
    ZEROIZE(in->buf);
  }
  in->off = 0;
}

  char*
grow_LaceX(LaceX* in, size_t capac)
{
  return (char*) grow_LaceA_(
      (void**)&in->buf.at, &in->buf.sz, &in->buf.alloc_lgsz,
      1, capac, realloc);
}

  void
flush_LaceX(LaceX* x)
{
  assert(x->off <= x->buf.sz);
  if (x->off == 0)  return;
  x->buf.sz -= x->off;
  if (x->buf.sz > 0) {
    memmove(x->buf.at, &x->buf.at[x->off], x->buf.sz);
  }
  x->off = 0;
}

  void
maybe_flush_LaceX(LaceX* x)
{
  if (x->flush_lgsz > 0 &&
      (x->off >> x->flush_lgsz) > 0) {
    flush_LaceX(x);
  }
}

static
  char*
cstr_of_LaceX(LaceX* in)
{
  *grow_LaceX(in, 1) = '\0';
  in->buf.sz -= 1;
  return &in->buf.at[in->off];
}

/* Get line, return string.*/
  char*
getline_LaceX(LaceX* in)
{
  size_t ret_off;
  char* s;

  maybe_flush_LaceX(in);

  ret_off = in->off;
  s = strchr(cstr_of_LaceX(in), '\n');

  while (!s) {
    in->off = in->buf.sz;
    if (0 == read_LaceX(in)) {
      break;
    }
    s = strchr(cstr_of_LaceX(in), '\n');
  }

  if (s) {
    s[0] = '\0';
    in->off = 1 + (s - in->buf.at);
    /* Ignore carriage returns.*/
    if (in->off - ret_off >= 2 && s[-1] == '\r')
      s[-1] = '\0';
  }
  if (ret_off == in->off) {
    return NULL;
  }
  assert(in->buf.at[in->buf.sz] == '\0');
  return &in->buf.at[ret_off];
}

/* Get string up to a delimiter. Push offset past delimiter.*/
  char*
gets_LaceX(LaceX* in, const char* delim)
{
  size_t delim_length = strlen(delim);
  size_t ret_off;
  char* s;

  assert(delim_length > 0);

  maybe_flush_LaceX(in);

  ret_off = in->off;
  s = strstr(cstr_of_LaceX(in), delim);

  while (!s) {
    if (in->off + delim_length <= in->buf.sz) {
      in->off = in->buf.sz + 1 - delim_length;
    }
    if (0 == read_LaceX(in)) {
      break;
    }
    s = strstr(cstr_of_LaceX(in), delim);
  }

  if (s) {
    s[0] = '\0';
    in->off = delim_length + (s - in->buf.at);
  }
  if (ret_off == in->off) {
    return NULL;
  }
  assert(in->buf.at[in->buf.sz] == '\0');
  return &in->buf.at[ret_off];
}
