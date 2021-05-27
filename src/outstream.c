
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
  flush_LaceO(o);
  if (o->vt && o->vt->close_fn) {
    o->vt->close_fn(o);
  }
  if (o->at) {
    free(o->at);
    o->at = NULL;
    o->size = 0;
    o->alloc_lgsize = 0;
  }
  o->off = 0;
}

  char*
grow_LaceO(LaceO* o, size_t capac)
{
  return (char*) grow_LaceA_(
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
  char buf[1 + (CHAR_BIT * sizeof(int)) / 3];
  unsigned i = sizeof(buf);

  if (q == 0) {
    putc_LaceO(out, '0');
    return;
  } else if (q < 0) {
    q = -q;
    buf[0] = '-';
  } else {
    buf[0] = '\0';
  }

  while (q > 0) {
    buf[--i] = '0' + (char)(q % 10);
    q /= 10;
  }

  if (buf[0] == '-') {
    buf[--i] = '-';
  }

  memcpy(grow_LaceO(out, sizeof(buf)-i),
         &buf[i],
         sizeof(buf)-i);
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
