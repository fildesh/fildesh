
#include <fildesh/fildesh.h>

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
  FildeshO* o = &of->o;
  unsigned i;

  for (i = 0; i < of->chunk_size && o->off < o->size; ++i) {
    assert(of->content[of->index] == o->at[o->off]);
    of->index += 1;
    o->off += 1;
  }
}


  void
param3_test_putstr(
    unsigned chunk_size,
    Fildesh_lgsize flush_lgsize,
    const char* delim)
{
  static const char* const lines[] = {
    "this is the first line",
    "this is the second line",
    "this is third line",
    "",
    "this is the fifth line",
  };
  MockFildeshOF of[1] = { DEFAULT_MockFildeshOF };
  FildeshO* o = &of->o;
  of->chunk_size = chunk_size;
  of->o.flush_lgsize = flush_lgsize;
  of->content = (char*)malloc((30+strlen(delim))*5+1);
  assert(of->content);
  sprintf(of->content, "%s%s%s%s%s%s%s%s%s",
          lines[0], delim,
          lines[1], delim,
          lines[2], delim,
          lines[3], delim,
          lines[4]);

  putstr_FildeshO(o, lines[0]);
  putstr_FildeshO(o, delim);
  assert(flush_lgsize == 0 || (o->size >> flush_lgsize) == 0);
  assert(allocated_size_of_FildeshO(o) >= o->size);

  putstr_FildeshO(o, lines[1]);
  putstr_FildeshO(o, delim);
  assert(flush_lgsize == 0 || (o->size >> flush_lgsize) == 0);

  putstr_FildeshO(o, lines[2]);
  putstr_FildeshO(o, delim);
  assert(flush_lgsize == 0 || (o->size >> flush_lgsize) == 0);

  putstr_FildeshO(o, lines[3]);
  putstr_FildeshO(o, delim);
  assert(flush_lgsize == 0 || (o->size >> flush_lgsize) == 0);

  putstr_FildeshO(o, lines[4]);
  assert(flush_lgsize == 0 || (o->size >> flush_lgsize) == 0);

  flush_FildeshO(o);
  assert(o->off == 0);
  assert(o->size == 0);
  assert(of->content[of->index] == '\0');

  close_FildeshO(&of->o);
  free(of->content);
}

int main() {
  unsigned chunk_size;
  Fildesh_lgsize flush_lgsize;
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
        param3_test_putstr(chunk_size, flush_lgsize, delims[delim_index]);
      }
    }
  }

  return 0;
}

