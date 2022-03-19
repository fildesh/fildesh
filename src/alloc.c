
#include "fildesh.h"
#include <stdlib.h>
#include <string.h>

  FildeshAlloc*
open_FildeshAlloc()
{
  char* block = (char*) malloc((size_t)1 << FILDESH_ALLOC_MIN_BLOCK_LGSIZE);
  const size_t mask = ~(fildesh_alignof(FildeshAlloc)-1);
  const size_t block_size = mask &
    (((size_t)1 << FILDESH_ALLOC_MIN_BLOCK_LGSIZE) - sizeof(FildeshAlloc));
  FildeshAlloc* alloc = (FildeshAlloc*) &block[block_size];
  alloc->block_count = 1;
  alloc->blocks[0] = block;
  alloc->sizes[0] = block_size;
  return alloc;
}

  void
close_FildeshAlloc(FildeshAlloc* alloc)
{
  unsigned i;
  if (!alloc) {return;}
  for (i = alloc->block_count; i > 0; --i) {
    free(alloc->blocks[i-1]);
  }
}

  void
create_block_FildeshAlloc(FildeshAlloc* alloc)
{
  const size_t size =
    (size_t)1 << (FILDESH_ALLOC_MIN_BLOCK_LGSIZE + alloc->block_count);
  alloc->sizes[alloc->block_count] = size;
  alloc->blocks[alloc->block_count] = (char*) malloc(size);
  alloc->block_count += 1;
}

  char*
strdup_FildeshAlloc(FildeshAlloc* alloc, const char* s)
{
  size_t size = 1+strlen(s);
  char* buf = fildesh_allocate(char, size, alloc);
  memcpy(buf, s, size);
  return buf;
}
