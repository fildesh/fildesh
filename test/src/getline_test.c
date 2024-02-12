
#include <fildesh/fildesh.h>

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

typedef struct MockFildeshXF MockFildeshXF;
struct MockFildeshXF {
  FildeshX x;
  unsigned row;
  unsigned col;
  const char* const* lines;
  size_t chunk_size;
};

static void read_MockFildeshXF(MockFildeshXF* xf);
static void close_MockFildeshXF(MockFildeshXF* xf) {(void) xf;}
static void free_MockFildeshXF(MockFildeshXF* xf) {(void) xf;}
DEFINE_FildeshX_VTable(MockFildeshXF, x);
#define DEFAULT_MockFildeshXF { \
  DEFAULT1_FildeshX(DEFAULT_MockFildeshXF_FildeshX_VTable), \
  0, 0, NULL, 0, \
}


  void
read_MockFildeshXF(MockFildeshXF* xf) {
  FildeshX* x = &xf->x;
  char c;
  unsigned i;

  for (i = 0; i < xf->chunk_size; ++i) {
    if (!xf->lines[xf->row]) {
      return;
    }
    c = xf->lines[xf->row][xf->col];
    if (c == '\0') {
      xf->row += 1;
      xf->col = 0;
      if (!xf->lines[xf->row]) {
        return;
      }
      c = '\n';
    } else {
      c = xf->lines[xf->row][xf->col];
      xf->col += 1;
    }
    *grow_FildeshX(x, 1) = c;
  }
}

  void
param2_test_getline(unsigned chunk_size, Fildesh_lgsize flush_lgsize) {
  static const char* const lines[] = {
    "this is the first line",
    "this is the second line",
    "this is third line",
    "",
    "this is the fifth line",
    NULL,
  };
  MockFildeshXF xf[1] = { DEFAULT_MockFildeshXF };
  FildeshX* const in = &xf->x;
  FildeshX slice;
  xf->chunk_size = chunk_size;
  xf->x.flush_lgsize = flush_lgsize;
  xf->lines = lines;

  slice = sliceline_FildeshX(in);
  assert(skipstr_FildeshX(&slice, lines[0]));
  assert(!avail_FildeshX(&slice));

  slice = sliceline_FildeshX(in);
  assert(skipstr_FildeshX(&slice, lines[1]));
  assert(!avail_FildeshX(&slice));

  slice = sliceline_FildeshX(in);
  assert(skipstr_FildeshX(&slice, lines[2]));
  assert(!avail_FildeshX(&slice));

  slice = sliceline_FildeshX(in);
  assert(skipstr_FildeshX(&slice, lines[3]));
  assert(!avail_FildeshX(&slice));

  slice = sliceline_FildeshX(in);
  assert(skipstr_FildeshX(&slice, lines[4]));
  assert(!avail_FildeshX(&slice));

  slice = sliceline_FildeshX(in);
  assert(!slice.at);
  assert(!avail_FildeshX(in));

  close_FildeshX(in);
}

int main() {
  unsigned chunk_size;
  Fildesh_lgsize flush_lgsize;

  for (chunk_size = 1; chunk_size < 20; ++chunk_size) {
    for (flush_lgsize = 0; flush_lgsize < 4; ++flush_lgsize) {
      fprintf(stderr, "chunk_size:%u  flush_lgsize:%u\n",
              chunk_size, (unsigned)flush_lgsize);
      param2_test_getline(chunk_size, flush_lgsize);
    }
  }

  return 0;
}

