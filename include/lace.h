#ifndef LACE_H_
#define LACE_H_
#include <stddef.h>
#include <stdint.h>
#include <limits.h>

typedef struct LaceX_VTable LaceX_VTable;
typedef struct LaceX LaceX;

struct LaceX_VTable
{
  size_t (*read_fn) (LaceX*);
  void (*close_fn) (LaceX*);
  void (*free_fn) (LaceX*);
};

struct LaceX {
  struct LaceX_Buffer {
    char* at;
    size_t sz;
    uint8_t alloc_lgsz;
  } buf;
  size_t off;
  const LaceX_VTable* vt;
};

typedef struct LaceXF LaceXF;
struct LaceXF {
  LaceX base;
  int fd;
};

#if __STDC_VERSION__ < 199901L && !defined(inline)
#define inline __inline
#endif

size_t read_LaceX(LaceX*);
void close_LaceX(LaceX*);

void open_LaceXF(LaceXF* f, const char* filename);
void close_LaceXF(LaceXF* f);

char* lace_parse_int(int* ret, const char* in);


static inline
  void
grow_LaceA_(void** at, size_t* count, uint8_t* allocated_lgcount,
            size_t element_size, size_t capac,
            void* (*realloc_fn) (void*, size_t))
{
  *count += capac;
  if (*allocated_lgcount == UCHAR_MAX) {
    /* This allocation is fixed. */
    return;
  }
  if ((*count << 1) > ((size_t)1 << *allocated_lgcount)) {
    if (*allocated_lgcount == 0) {
      *at = 0;
      *allocated_lgcount = 1;
    }
    while (*count > ((size_t)1 << *allocated_lgcount)) {
      *allocated_lgcount += 1;
    }

    *allocated_lgcount += 1;
    *at = realloc_fn(*at, element_size << (*allocated_lgcount - 1));
  }
}

static inline
  void
mpop_LaceA_(void** at, size_t* count, uint8_t* allocated_lgcount,
            size_t element_size, size_t capac,
            void* (*realloc_fn) (void*, size_t))
{
  *count -= capac;
  if (*allocated_lgcount == UCHAR_MAX) {
    /* This allocation is fixed. */
    return;
  }
  if ((*allocated_lgcount >= 3) && ((*count >> (*allocated_lgcount - 3)) == 0)) {
    while ((*allocated_lgcount >= 4) && ((*count >> (*allocated_lgcount - 4)) == 0))
      *allocated_lgcount -= 1;
    *allocated_lgcount -= 1;

    if (*allocated_lgcount > 0) {
      *at = realloc_fn(*at, element_size << (*allocated_lgcount - 1));
    } else {
      *at = realloc_fn(*at, 0);
    }
  }
}

#endif
