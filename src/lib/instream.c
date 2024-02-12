
#include <fildesh/fildesh.h>
#include "include/fildesh/fildesh_compat_string.h"
#include "mascii.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

static inline bool sliced_FildeshX(const FildeshX* slice) {
  return (slice->at && (slice->alloc_lgsize == 0));
}

static inline
  FildeshX
slice_FildeshX(const FildeshX* in, size_t beg_off, size_t end_off)
{
  FildeshX slice = *in;
  slice.off = beg_off;
  slice.size = end_off;
  assert(slice.off <= slice.size);
  return getslice_FildeshX(&slice);
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
      1, capac);
}

  void
flush_FildeshX(FildeshX* x)
{
  assert(x->off <= x->size);
  assert(!sliced_FildeshX(x));
  if (x->off > 0) {
    if (x->off == x->size) {
      truncate_FildeshX(x);
    }
    else {
      x->size -= x->off;
      memmove(x->at, &x->at[x->off], x->size);
      x->off = 0;
    }
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

  void
slurp_FildeshX(FildeshX* in)
{
  if (!in) {return;}
  maybe_flush_FildeshX(in);
  while (read_FildeshX(in) > 0) {/* Yummy.*/}
  if (in->vt && in->vt->close_fn) {
    in->vt->close_fn(in);
  }
  in->flush_lgsize = 0;
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

  FildeshX
until_char_FildeshX(FildeshX* in, char delim)
{
  FildeshX slice = DEFAULT_FildeshX;
  size_t ret_off;
  char* s = NULL;

  maybe_flush_FildeshX(in);
  ret_off = in->off;
  if (in->size > 0) {
    s = (char*) memchr(&in->at[in->off], delim, in->size - in->off);
  }

  while (!s) {
    in->off = in->size;
    if (0 == read_FildeshX(in)) {
      break;
    }
    s = (char*) memchr(&in->at[in->off], delim, in->size - in->off);
  }

  if (ret_off == in->size) {
    assert(in->off == in->size);
    return slice;  /* Empty.*/
  }

  if (s) {
    in->off = (s - in->at);
    slice = slice_FildeshX(in, ret_off, in->off);
  } else {
    assert(in->off == in->size);
    slice = slice_FildeshX(in, ret_off, in->size);
  }
  return slice;
}

static inline
  const char*
discount_memmem(const char* haystack, size_t haystack_size,
                const unsigned char* needle, size_t needle_size)
{
  while (needle_size <= haystack_size) {
    const char* const s = (const char*) memchr(
        haystack, needle[0], haystack_size - needle_size + 1);
    if (!s) {
      break;
    }
    if (0 == memcmp(&s[1], &needle[1], needle_size-1)) {
      return s;
    }
    haystack_size -= (size_t) (&s[1] - haystack);
    haystack = &s[1];
  }
  return NULL;
}

  FildeshX
until_bytestring_FildeshX(
    FildeshX* in,
    const unsigned char* delim,
    size_t delim_length)
{
  FildeshX slice = DEFAULT_FildeshX;
  const char* s = NULL;
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
    s = discount_memmem(&in->at[in->off], in->size - in->off, delim, delim_length);
    while (!s) {
      in->off = in->size + 1 - delim_length;
      if (0 == read_FildeshX(in)) {
        break;
      }
      s = discount_memmem(&in->at[in->off], in->size - in->off, delim, delim_length);
    }
  }

  if (ret_off == in->size) {
    return slice;  /* Empty.*/
  }

  if (s) {
    in->off = (s - in->at);
    slice = slice_FildeshX(in, ret_off, in->off);
  } else {
    in->off = in->size;
    slice = slice_FildeshX(in, ret_off, in->size);
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

  bool
peek_chars_FildeshX(FildeshX* in, const char* needles)
{
  if (peek_bytestring_FildeshX(in, NULL, 1)) {
    if (in->at[in->off] != '\0') {
      return !!strchr(needles, in->at[in->off]);
    }
  }
  return false;
}

  bool
peek_bytestring_FildeshX(FildeshX* in, const unsigned char* s, size_t n)
{
  while (in->size - in->off < n) {
    if (0 == read_FildeshX(in)) {
      return false;
    }
  }
  if (!s) {
    return true;
  }
  return 0 == memcmp(&in->at[in->off], s, n);
}

  bool
skip_bytestring_FildeshX(FildeshX* in, const unsigned char* s, size_t n)
{
  if (peek_bytestring_FildeshX(in, s, n)) {
    in->off += n;
    maybe_flush_FildeshX(in);
    return true;
  }
  return false;
}

/** Like strcspn or strtok but returns a slice.**/
  FildeshX
slicechr_FildeshX(FildeshX* in, char delim)
{
  FildeshX slice;
  slice = until_char_FildeshX(in, delim);
  if (slice.at) {
    if (in->off < in->size) {
      in->off += 1;
    }
    else {
      size_t slice_offset = (size_t) (slice.at - in->at);
      slice.at = &in->at[slice_offset];
    }
  }
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
  }
  return slice;
}

/** Like strstr but returns a slice.**/
  FildeshX
slicestr_FildeshX(FildeshX* in, const char* delim)
{
  const size_t delim_size = strlen(delim);
  FildeshX slice = until_bytestring_FildeshX(
      in, (const unsigned char*)delim, delim_size);
  if (avail_FildeshX(in)) {
    in->off += delim_size;
    assert(in->off <= in->size);
  }
  return slice;
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
  return skip_bytestring_FildeshX(
      in,
      (const unsigned char*)s,
      strlen(s));
}

  bool
parse_int_FildeshX(FildeshX* in, int* ret)
{
  char buf[101];
  FildeshX slice;
  char* end = NULL;
  skipchrs_FildeshX(in, fildesh_compat_string_blank_bytes);
  slice = while_chars_FildeshX(in, "+-0123456789");
  if (slice.size > 0) {
    if (slice.size > sizeof(buf)-1) {return false;}
    memcpy(buf, slice.at, slice.size);
    buf[slice.size] = '\0';
    end = fildesh_parse_int(ret, buf);
  }
  return !!end;
}

  bool
parse_unsigned_FildeshX(FildeshX* in, unsigned* ret)
{
  char buf[101];
  FildeshX slice;
  char* end = NULL;
  skipchrs_FildeshX(in, fildesh_compat_string_blank_bytes);
  slice = while_chars_FildeshX(in, "+0123456789");
  if (slice.size > 0) {
    if (slice.size > sizeof(buf)-1) {return false;}
    memcpy(buf, slice.at, slice.size);
    buf[slice.size] = '\0';
    end = fildesh_parse_unsigned(ret, buf);
  }
  return !!end;
}

  bool
parse_double_FildeshX(FildeshX* in, double* ret)
{
  char buf[101];
  FildeshX slice;
  char* end = NULL;
  skipchrs_FildeshX(in, fildesh_compat_string_blank_bytes);
  slice = while_chars_FildeshX(in, "+-.0123456789Ee");
  if (slice.size > 0) {
    if (slice.size > sizeof(buf)-1) {return false;}
    memcpy(buf, slice.at, slice.size);
    buf[slice.size] = '\0';
    end = fildesh_parse_double(ret, buf);
  }
  return !!end;
}
