
#include <fildesh/fildesh.h>
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

static
  void
create_block_FildeshAlloc(FildeshAlloc* alloc)
{
  const size_t size =
    (size_t)1 << (FILDESH_ALLOC_MIN_BLOCK_LGSIZE + alloc->block_count);
  alloc->sizes[alloc->block_count] = size;
  alloc->blocks[alloc->block_count] = (char*) malloc(size);
  alloc->block_count += 1;
}

  void*
reserve_FildeshAlloc(FildeshAlloc* alloc, size_t size, size_t alignment)
{
  uintptr_t p;
  const size_t mask = ~(alignment-1);
  fildesh_lgsize_t i = alloc->block_count-1;
  while (alloc->sizes[i] <= size) {
    create_block_FildeshAlloc(alloc);
    i = alloc->block_count-1;
  }

  p = (uintptr_t)&alloc->blocks[i][alloc->sizes[i] - size] & mask;
  while (p < (uintptr_t)alloc->blocks[i]) {
    create_block_FildeshAlloc(alloc);
    i = alloc->block_count-1;
    p = (uintptr_t)&alloc->blocks[i][alloc->sizes[i] - size] & mask;
  }
  alloc->sizes[i] = p - (uintptr_t)alloc->blocks[i];
  return (void*)p;
}

  char*
strdup_FildeshAlloc(FildeshAlloc* alloc, const char* s)
{
  size_t size = 1+strlen(s);
  char* buf = fildesh_allocate(char, size, alloc);
  memcpy(buf, s, size);
  return buf;
}

  char*
strdup_FildeshX(const FildeshX* in, FildeshAlloc* alloc)
{
  size_t n = in->size - in->off;
  char* buf = fildesh_allocate(char, n+1, alloc);
  memcpy(buf, &in->at[in->off], n);
  buf[n] = '\0';
  return buf;
}

  char*
strdup_FildeshO(const FildeshO* out, FildeshAlloc* alloc)
{
  size_t n = out->size - out->off;
  char* buf = fildesh_allocate(char, n+1, alloc);
  memcpy(buf, &out->at[out->off], n);
  buf[n] = '\0';
  return buf;
}
