
#include <fildesh/fildesh.h>

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

  size_t
write_FildeshO(FildeshO* o)
{
  if (o->vt && o->vt->write_fn) {
    size_t old_off = o->off;
    o->vt->write_fn(o);
    return o->off - old_off;
  }
  return 0;
}

  void
close_FildeshO(FildeshO* o)
{
  if (!o) { return; }
  flush_FildeshO(o);
  if (o->vt && o->vt->close_fn) {
    o->vt->close_fn(o);
  }
  if (o->at && o->alloc_lgsize > 0) {
    free(o->at);
    o->at = NULL;
    o->size = 0;
    o->alloc_lgsize = 0;
  }
  o->off = 0;
  if (o->vt && o->vt->free_fn) {
    o->vt->free_fn(o);
  }
}

  char*
grow_FildeshO(FildeshO* o, size_t capac)
{
  return (char*) grow_FildeshA_(
      (void**)&o->at, &o->size, &o->alloc_lgsize,
      1, capac);
}

  void
flush_FildeshO(FildeshO* o)
{
  assert(o->off <= o->size);
  while (o->off < o->size) {
    if (0 == write_FildeshO(o)) {
      break;
    }
  }
  if (o->off > 0) {
    if (o->off == o->size) {
      truncate_FildeshO(o);
    }
    else {
      o->size -= o->off;
      memmove(o->at, &o->at[o->off], o->size);
      o->off = 0;
    }
  }
}

/** If the buffer offset is too big, then shift data to the front.
 * Maybe write some data as well.
 **/
  void
maybe_flush_FildeshO(FildeshO* o)
{
  if (o->flush_lgsize == 0) {
    return;
  }
  if (o->off < o->size &&
      (o->size >> o->flush_lgsize) == 0) {
    return;
  }
  flush_FildeshO(o);
}

  void
put_bytestring_FildeshO(FildeshO* out, const unsigned char* s, size_t n)
{
  char* buf;
  if (n == 0) {return;}
  buf = grow_FildeshO(out, n);
  memcpy(buf, s, n);
  maybe_flush_FildeshO(out);
}

  void
putc_FildeshO(FildeshO* o, char c)
{
  *grow_FildeshO(o, 1) = c;
  maybe_flush_FildeshO(o);
}

  void
puts_FildeshO(FildeshO* o, const char* s)
{
  const size_t length = strlen(s);
  char* buf = grow_FildeshO(o, length);
  memcpy(buf, s, length);
  maybe_flush_FildeshO(o);
}

  void
print_int_FildeshO(FildeshO* out, int q)
{
  unsigned n = fildesh_encode_int_base10(
      grow_FildeshO(out, FILDESH_INT_BASE10_SIZE_MAX),
      q);
  out->size -= FILDESH_INT_BASE10_SIZE_MAX - n;
  maybe_flush_FildeshO(out);
}

  void
print_double_FildeshO(FildeshO* out, double q)
{
  char buf[50];
  unsigned n = sprintf(buf, "%.17g", q);
  assert(n > 0);
  memcpy(grow_FildeshO(out, n), buf, n);
  maybe_flush_FildeshO(out);
}

  void
repeat_byte_FildeshO(FildeshO* out, unsigned char b, size_t n)
{
  memset(grow_FildeshO(out, n), b, n);
  maybe_flush_FildeshO(out);
}
