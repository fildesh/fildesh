
#include "lace.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

typedef struct MockFildeshOF MockFildeshOF;
struct MockFildeshOF {
  FildeshO o;
  unsigned index;
  char* content;
  size_t chunk_size;
};

static void write_MockFildeshOF(MockFildeshOF* of);
static void close_MockFildeshOF(MockFildeshOF* of) {(void) of;}
static void free_MockFildeshOF(MockFildeshOF* of) {(void) of;}
DEFINE_FildeshO_VTable(MockFildeshOF, o);
#define DEFAULT_MockFildeshOF { \
  DEFAULT1_FildeshO(DEFAULT_MockFildeshOF_FildeshO_VTable), \
  0, NULL, 0, \
}

  void
write_MockFildeshOF(MockFildeshOF* of) {
  LaceO* o = &of->o;
  unsigned i;

  for (i = 0; i < of->chunk_size && o->off < o->size; ++i) {
    assert(of->content[of->index] == o->at[o->off]);
    of->index += 1;
    o->off += 1;
  }
}


  void
param3_test_puts(unsigned chunk_size, lace_lgsize_t flush_lgsize, const char* delim) {
  static const char* const lines[] = {
    "this is the first line",
    "this is the second line",
    "this is third line",
    "",
    "this is the fifth line",
  };
  MockFildeshOF of[1] = { DEFAULT_MockFildeshOF };
  LaceO* o = &of->o;
  of->chunk_size = chunk_size;
  of->o.flush_lgsize = flush_lgsize;
  of->content = (char*)malloc((30+strlen(delim))*5+1);
  sprintf(of->content, "%s%s%s%s%s%s%s%s%s",
          lines[0], delim,
          lines[1], delim,
          lines[2], delim,
          lines[3], delim,
          lines[4]);

  puts_LaceO(o, lines[0]);
  puts_LaceO(o, delim);
  assert(flush_lgsize == 0 || (o->size >> flush_lgsize) == 0);

  puts_LaceO(o, lines[1]);
  puts_LaceO(o, delim);
  assert(flush_lgsize == 0 || (o->size >> flush_lgsize) == 0);

  puts_LaceO(o, lines[2]);
  puts_LaceO(o, delim);
  assert(flush_lgsize == 0 || (o->size >> flush_lgsize) == 0);

  puts_LaceO(o, lines[3]);
  puts_LaceO(o, delim);
  assert(flush_lgsize == 0 || (o->size >> flush_lgsize) == 0);

  puts_LaceO(o, lines[4]);
  assert(flush_lgsize == 0 || (o->size >> flush_lgsize) == 0);

  flush_LaceO(o);
  assert(o->off == 0);
  assert(o->size == 0);
  assert(of->content[of->index] == '\0');

  close_LaceO(&of->o);
  free(of->content);
}

int main() {
  unsigned chunk_size;
  lace_lgsize_t flush_lgsize;
  unsigned delim_index;
  static const char* const delims[] = {
    "IAMA delimiter AMA",
    "z",
    "22",
    "333",
    NULL,
  };

  for (chunk_size = 1; chunk_size < 20; ++chunk_size) {
    for (flush_lgsize = 0; flush_lgsize < 4; ++flush_lgsize) {
      for (delim_index = 0; delims[delim_index]; ++delim_index) {
        fprintf(stderr, "chunk_size:%u  flush_lgsize:%u delim:%s\n",
                chunk_size, (unsigned)flush_lgsize, delims[delim_index]);
        param3_test_puts(chunk_size, flush_lgsize, delims[delim_index]);
      }
    }
  }

  return 0;
}

