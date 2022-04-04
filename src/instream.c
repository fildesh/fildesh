
#include "fildesh.h"
#include "fildesh_compat_string.h"
#include "mascii.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

static inline bool sliced_FildeshX(const FildeshX* slice) {
  return (slice->at && (slice->alloc_lgsize == 0));
}

static inline FildeshX slice_FildeshX(FildeshX* in, size_t beg_off, size_t end_off) {
  FildeshX slice = DEFAULT_FildeshX;
  assert(beg_off <= end_off);
  slice.at = &in->at[beg_off];
  slice.size = end_off - beg_off;
  slice.flush_lgsize = 0;
  return slice;
}

  size_t
read_FildeshX(FildeshX* in)
{
  if (in->vt && in->vt->read_fn) {
    size_t old_size = in->size;
    in->vt->read_fn(in);
    return in->size - old_size;
  }
  return 0;
}

  void
close_FildeshX(FildeshX* in)
{
  if (!in) {return;}
  if (in->vt && in->vt->close_fn) {
    in->vt->close_fn(in);
  }
  if (in->at && !sliced_FildeshX(in)) {
    free(in->at);
    in->at = NULL;
    in->size = 0;
    in->alloc_lgsize = 0;
  }
  in->off = 0;
  if (in->vt && in->vt->free_fn) {
    in->vt->free_fn(in);
  }
}

  char*
grow_FildeshX(FildeshX* in, size_t capac)
{
  return (char*) grow_FildeshA_(
      (void**)&in->at, &in->size, &in->alloc_lgsize,
      1, capac, realloc);
}

  void
flush_FildeshX(FildeshX* x)
{
  assert(x->off <= x->size);
  if (x->off == 0)  return;
  x->size -= x->off;
  if (x->size > 0) {
    memmove(x->at, &x->at[x->off], x->size);
  }
  x->off = 0;
  if (sliced_FildeshX(x)) {
    x->at[x->size] = '\0';
  }
}

  void
maybe_flush_FildeshX(FildeshX* x)
{
  if (x->flush_lgsize == 0) {
    return;
  }
  if (x->off < x->size &&
      (x->off >> x->flush_lgsize) == 0) {
    return;
  }
  flush_FildeshX(x);
}

static
  char*
cstr_of_FildeshX(FildeshX* in)
{
  if (!sliced_FildeshX(in)) {
    *grow_FildeshX(in, 1) = '\0';
    in->size -= 1;
  }
  assert(in->at[in->size] == '\0');
  return &in->at[in->off];
}

  char*
slurp_FildeshX(FildeshX* in)
{
  maybe_flush_FildeshX(in);
  while (read_FildeshX(in) > 0) {/* Yummy.*/}
  return cstr_of_FildeshX(in);
}

  void
wait_close_FildeshX(FildeshX* in)
{
  if (!in) {return;}
  do {
    in->off = 0;
    in->size = 0;
  } while (read_FildeshX(in) > 0);
  close_FildeshX(in);
}

/** Like strcspn or strtok but returns a slice.**/
  FildeshX
slicechr_FildeshX(FildeshX* in, const char delim)
{
  FildeshX slice = DEFAULT_FildeshX;
  size_t ret_off;
  char* s = NULL;

  maybe_flush_FildeshX(in);
  ret_off = in->off;
  if (in->size > 0) {
    s = strchr(cstr_of_FildeshX(in), delim);
  }

  while (!s) {
    in->off = in->size;
    if (0 == read_FildeshX(in)) {
      break;
    }
    s = strchr(cstr_of_FildeshX(in), delim);
  }

  if (ret_off == in->size) {
    assert(in->off == in->size);
    return slice;  /* Empty.*/
  }
  assert(in->at[in->size] == '\0');

  if (s) {
    in->off = 1 + (s - in->at);
    slice = slice_FildeshX(in, ret_off, in->off-1);
    slice.at[slice.size] = '\0';
  } else {
    assert(in->off == in->size);
    slice = slice_FildeshX(in, ret_off, in->size);
  }
  assert(slice.at[slice.size] == '\0');
  return slice;
}

/** Get line, return slice.**/
  FildeshX
sliceline_FildeshX(FildeshX* in)
{
  FildeshX slice = slicechr_FildeshX(in, '\n');
  /* Ignore carriage returns.*/
  if (slice.size > 0 && slice.at[slice.size-1] == '\r') {
    slice.size -= 1;
    slice.at[slice.size] = '\0';
  }
  return slice;
}

/** Like strcspn but returns a slice.**/
static
  FildeshX
until_mascii_FildeshX(FildeshX* in, const FildeshMascii* mascii)
{
  FildeshX slice = DEFAULT_FildeshX;
  size_t ret_off;
  size_t end;

  maybe_flush_FildeshX(in);
  ret_off = in->off;
  end = 0;
  if (in->size > 0) {
    end = in->off + find_FildeshMascii(
        mascii, &in->at[in->off], in->size - in->off);
  }

  while (end == in->size) {
    in->off = in->size;
    if (0 == read_FildeshX(in)) {
      break;
    }
    end = in->off + find_FildeshMascii(
        mascii, &in->at[in->off], in->size - in->off);
  }

  if (ret_off == in->size) {
    assert(in->off == in->size);
    return slice;  /* Empty.*/
  }

  if (end < in->size) {
    in->off = end;
    slice = slice_FildeshX(in, ret_off, in->off);
  } else {
    assert(in->off == in->size);
    slice = slice_FildeshX(in, ret_off, in->size);
  }
  return slice;
}

/** Like strcspn but returns a slice.**/
  FildeshX
until_chars_FildeshX(FildeshX* in, const char* delims)
{
  FildeshMascii mascii = charset_FildeshMascii(delims, strlen(delims));
  return until_mascii_FildeshX(in, &mascii);
}

/** Like strspn but returns a slice.**/
  FildeshX
while_chars_FildeshX(FildeshX* in, const char* span)
{
  FildeshMascii mascii = charnot_FildeshMascii(span, strlen(span));
  return until_mascii_FildeshX(in, &mascii);
}

/** Like strstr but returns a slice.**/
  FildeshX
slicestr_FildeshX(FildeshX* in, const char* delim)
{
  FildeshX slice = DEFAULT_FildeshX;
  size_t delim_length = strlen(delim);
  char* s = NULL;
  size_t ret_off;

  assert(delim_length > 0);

  maybe_flush_FildeshX(in);
  ret_off = in->off;

  while (in->off + delim_length > in->size) {
    if (0 == read_FildeshX(in)) {
      break;
    }
  }
  if (in->off + delim_length <= in->size) {
    s = strstr(cstr_of_FildeshX(in), delim);
    while (!s) {
      in->off = in->size + 1 - delim_length;
      if (0 == read_FildeshX(in)) {
        break;
      }
      s = strstr(cstr_of_FildeshX(in), delim);
    }
  }

  if (ret_off == in->size) {
    return slice;  /* Empty.*/
  }
  assert(in->at[in->size] == '\0');

  if (s) {
    in->off = delim_length + (s - in->at);
    slice = slice_FildeshX(in, ret_off, in->off-delim_length);
    slice.at[slice.size] = '\0';
  } else {
    in->off = in->size;
    slice = slice_FildeshX(in, ret_off, in->size);
  }
  assert(slice.at[slice.size] == '\0');
  return slice;
}

/** Get line, return string.**/
  char*
getline_FildeshX(FildeshX* in)
{
  FildeshX slice = sliceline_FildeshX(in);
  return slice.at;
}

/** Get string up to a delimiter. Push offset past delimiter.**/
  char*
gets_FildeshX(FildeshX* in, const char* delim)
{
  FildeshX slice = slicestr_FildeshX(in, delim);
  return slice.at;
}

  bool
skipchrs_FildeshX(FildeshX* in, const char* span)
{
  FildeshX slice = while_chars_FildeshX(in, span);
  maybe_flush_FildeshX(in);
  return (slice.size > 0);
}

  bool
skipstr_FildeshX(FildeshX* in, const char* s)
{
  const size_t n = strlen(s);
  while (in->size - in->off < n) {
    if (0 == read_FildeshX(in)) {
      return false;
    }
  }
  if (0 == memcmp(&in->at[in->off], s, n)) {
    in->off += n;
    maybe_flush_FildeshX(in);
    return true;
  }
  return false;
}

  bool
parse_int_FildeshX(FildeshX* in, int* ret)
{
  FildeshX slice;
  char* end = NULL;
  skipchrs_FildeshX(in, fildesh_compat_string_blank_bytes);
  slice = while_chars_FildeshX(in, "+-0123456789");
  if (slice.size > 0) {
    end = fildesh_parse_int(ret, slice.at);
  }
  return !!end;
}

  bool
parse_double_FildeshX(FildeshX* in, double* ret)
{
  FildeshX slice;
  char* end = NULL;
  skipchrs_FildeshX(in, fildesh_compat_string_blank_bytes);
  slice = while_chars_FildeshX(in, "+-.0123456789Ee");
  if (slice.size > 0) {
    end = fildesh_parse_double(ret, slice.at);
  }
  return !!end;
}
