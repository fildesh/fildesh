
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
  if (x->flush_lgsz == 0) {
    return;
  }
  if (x->off < x->buf.sz &&
      (x->off >> x->flush_lgsz) == 0) {
    return;
  }
  flush_LaceX(x);
}

static
  char*
cstr_of_LaceX(LaceX* in)
{
  if (in->buf.alloc_lgsz < LACE_LGSIZE_MAX) {
    *grow_LaceX(in, 1) = '\0';
    in->buf.sz -= 1;
  }
  assert(in->buf.at[in->buf.sz] == '\0');
  return &in->buf.at[in->off];
}


/* Get line, return slice.*/
  LaceX
sliceline_LaceX(LaceX* in)
{
  LaceX slice = DEFAULT_LaceX;
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

  if (ret_off == in->buf.sz) {
    in->off = in->buf.sz;
    return slice;  /* Empty.*/
  }
  assert(in->buf.at[in->buf.sz] == '\0');

  slice.buf.at = &in->buf.at[ret_off];
  slice.buf.alloc_lgsz = LACE_LGSIZE_MAX;
  if (s) {
    in->off = 1 + (s - in->buf.at);
    slice.buf.sz = in->off - ret_off - 1;
    slice.buf.at[slice.buf.sz] = '\0';
    /* Ignore carriage returns.*/
    if (slice.buf.sz >= 1 && slice.buf.at[slice.buf.sz-1] == '\r') {
      slice.buf.sz -= 1;
      slice.buf.at[slice.buf.sz] = '\0';
    }
  } else {
    assert(in->off == in->buf.sz);
    slice.buf.sz = in->buf.sz - ret_off;
  }
  assert(slice.buf.at[slice.buf.sz] == '\0');
  return slice;
}

/** Like strstr but returns a slice.**/
  LaceX
slicestr_LaceX(LaceX* in, const char* delim)
{
  LaceX slice = DEFAULT_LaceX;
  size_t delim_length = strlen(delim);
  char* s = NULL;
  size_t ret_off;

  assert(delim_length > 0);

  maybe_flush_LaceX(in);
  ret_off = in->off;

  while (in->off + delim_length > in->buf.sz) {
    if (0 == read_LaceX(in)) {
      break;
    }
  }
  if (in->off + delim_length <= in->buf.sz) {
    s = strstr(cstr_of_LaceX(in), delim);
    while (!s) {
      in->off = in->buf.sz + 1 - delim_length;
      if (0 == read_LaceX(in)) {
        break;
      }
      s = strstr(cstr_of_LaceX(in), delim);
    }
  }

  if (ret_off == in->buf.sz) {
    return slice;  /* Empty.*/
  }
  assert(in->buf.at[in->buf.sz] == '\0');

  slice.buf.at = &in->buf.at[ret_off];
  slice.buf.alloc_lgsz = LACE_LGSIZE_MAX;
  if (s) {
    in->off = delim_length + (s - in->buf.at);
    slice.buf.sz = in->off - ret_off - delim_length;
    slice.buf.at[slice.buf.sz] = '\0';
  } else {
    in->off = in->buf.sz;
    slice.buf.sz = in->buf.sz - ret_off;
  }
  assert(slice.buf.at[slice.buf.sz] == '\0');
  return slice;
}

/* Get line, return string.*/
  char*
getline_LaceX(LaceX* in)
{
  LaceX slice = sliceline_LaceX(in);
  return slice.buf.at;
}

/* Get string up to a delimiter. Push offset past delimiter.*/
  char*
gets_LaceX(LaceX* in, const char* delim)
{
  LaceX slice = slicestr_LaceX(in, delim);
  return slice.buf.at;
}
