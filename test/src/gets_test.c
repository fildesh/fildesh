
#include "lace.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

typedef struct MockFildeshXF MockFildeshXF;
struct MockFildeshXF {
  FildeshX x;
  unsigned index;
  char* content;
  size_t chunk_size;
};

static void read_MockFildeshXF(MockFildeshXF* xf);
static void close_MockFildeshXF(MockFildeshXF* xf) {(void) xf;}
static void free_MockFildeshXF(MockFildeshXF* xf) {(void) xf;}
DEFINE_FildeshX_VTable(MockFildeshXF, x);
#define DEFAULT_MockFildeshXF { \
  DEFAULT1_FildeshX(DEFAULT_MockFildeshXF_FildeshX_VTable), \
  0, NULL, 0, \
}

  void
read_MockFildeshXF(MockFildeshXF* xf) {
  LaceX* x = &xf->x;
  char c;
  unsigned i;

  for (i = 0; i < xf->chunk_size; ++i) {
    c = xf->content[xf->index];
    if (c == '\0') {
      return;
    }
    xf->index += 1;
    *grow_LaceX(x, 1) = c;
  }
}

  void
param3_test_gets(unsigned chunk_size, fildesh_lgsize_t flush_lgsize, const char* delim) {
  static const char* const lines[] = {
    "this is the first line",
    "this is the second line",
    "this is third line",
    "",
    "this is the fifth line",
  };
  MockFildeshXF xf[1] = { DEFAULT_MockFildeshXF };
  char* line;
  xf->chunk_size = chunk_size;
  xf->x.flush_lgsize = flush_lgsize;
  xf->content = (char*)malloc((30+strlen(delim))*5+1);
  sprintf(xf->content, "%s%s%s%s%s%s%s%s%s",
          lines[0], delim,
          lines[1], delim,
          lines[2], delim,
          lines[3], delim,
          lines[4]);

  line = gets_LaceX(&xf->x, delim);
  assert(0 == strcmp(lines[0], line));

  line = gets_LaceX(&xf->x, delim);
  assert(0 == strcmp(lines[1], line));

  line = gets_LaceX(&xf->x, delim);
  assert(0 == strcmp(lines[2], line));

  line = gets_LaceX(&xf->x, delim);
  assert(0 == strcmp(lines[3], line));

  line = gets_LaceX(&xf->x, delim);
  assert(0 == strcmp(lines[4], line));

  line = gets_LaceX(&xf->x, delim);
  assert(!line);

  close_LaceX(&xf->x);
  free(xf->content);
}

int main() {
  unsigned chunk_size;
  fildesh_lgsize_t flush_lgsize;
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
        param3_test_gets(chunk_size, flush_lgsize, delims[delim_index]);
      }
    }
  }

  return 0;
}

