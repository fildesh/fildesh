
#include "fildesh.h"

#include <assert.h>
#include <string.h>

static
  void
alloc_test()
{
  FildeshAlloc* alloc = open_FildeshAlloc();
  char* bufs[3];
  size_t size = ((size_t)1 << FILDESH_ALLOC_MIN_BLOCK_LGSIZE);
  bufs[0] = fildesh_allocate(char, size+1, alloc);
  memset(bufs[0], 'a', size+1);

  size *= 2;
  bufs[1] = fildesh_allocate(char, size+1, alloc);
  memset(bufs[1], 'b', size+1);

  size *= 2;
  bufs[2] = fildesh_allocate(char, size+1, alloc);
  memset(bufs[2], 'c', size+1);

  assert(bufs[0][1] == 'a');
  assert(bufs[1][0] == 'b');
  assert(bufs[2][0] == 'c');
  close_FildeshAlloc(alloc);
}


int main() {
  alloc_test();
  return 0;
}
