
#include "lace.h"

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

  size_t
write_LaceO(LaceO* o)
{
  if (o->vt && o->vt->write_fn) {
    size_t old_off = o->off;
    o->vt->write_fn(o);
    return o->off - old_off;
  }
  return 0;
}

  void
close_LaceO(LaceO* o)
{
  if (!o) { return; }
  flush_LaceO(o);
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
grow_LaceO(LaceO* o, size_t capac)
{
  return (char*) grow_FildeshA_(
      (void**)&o->at, &o->size, &o->alloc_lgsize,
      1, capac, realloc);
}

  void
flush_LaceO(LaceO* o)
{
  assert(o->off <= o->size);
  while (o->off < o->size) {
    if (0 == write_LaceO(o)) {
      break;
    }
  }
  if (o->off == 0)  return;
  o->size -= o->off;
  if (o->size > 0) {
    memmove(o->at, &o->at[o->off], o->size);
  }
  o->off = 0;
}

/** If the buffer offset is too big, then shift data to the front.
 * Maybe write some data as well.
 **/
  void
maybe_flush_LaceO(LaceO* o)
{
  if (o->flush_lgsize == 0) {
    return;
  }
  if (o->off < o->size &&
      (o->size >> o->flush_lgsize) == 0) {
    return;
  }
  flush_LaceO(o);
}

  void
putc_LaceO(LaceO* o, char c)
{
  *grow_LaceO(o, 1) = c;
  maybe_flush_LaceO(o);
}

  void
puts_LaceO(LaceO* o, const char* s)
{
  const size_t length = strlen(s);
  char* buf = grow_LaceO(o, length);
  memcpy(buf, s, length);
  maybe_flush_LaceO(o);
}

  void
print_int_LaceO(LaceO* out, int q)
{
  unsigned n = fildesh_encode_int_base10(
      grow_LaceO(out, FILDESH_INT_BASE10_SIZE_MAX),
      q);
  out->size -= FILDESH_INT_BASE10_SIZE_MAX - n;
  maybe_flush_LaceO(out);
}

  void
print_double_LaceO(LaceO* out, double q)
{
  char buf[50];
  unsigned n = sprintf(buf, "%.17g", q);
  assert(n > 0);
  memcpy(grow_LaceO(out, n), buf, n);
  maybe_flush_LaceO(out);
}
