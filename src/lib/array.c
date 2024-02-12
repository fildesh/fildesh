#include <fildesh/fildesh.h>
#include <stdlib.h>

  void
close_FildeshAT(void* p)
{
  FildeshA* a = (FildeshA*) p;
  if (a->at) {
    free(a->at);
    a->at = NULL;
  }
  a->count = 0;
  a->allocated_lgcount = 0;
}

  void
clear_FildeshAT(void* p)
{
  close_FildeshAT(p);
}

  void*
realloc_more_FildeshA_(
    void* at, Fildesh_lgsize*
    p_allocated_lgcount,
    const size_t element_size,
    const size_t count)
{
  Fildesh_lgsize allocated_lgcount = *p_allocated_lgcount;
  if (allocated_lgcount == 0) {
    allocated_lgcount = 1;
  }
  do {
    allocated_lgcount += 1;
  } while ((count << 1) > ((size_t)1 << allocated_lgcount));
  at = realloc(at, fildesh_size_of_lgcount(element_size, allocated_lgcount));
  if (at) {
    *p_allocated_lgcount = allocated_lgcount;
  }
  return at;
}

  void*
realloc_less_FildeshA_(
    void* at,
    Fildesh_lgsize* p_allocated_lgcount,
    const size_t element_size,
    const size_t count)
{
  Fildesh_lgsize allocated_lgcount = *p_allocated_lgcount;
  do {
    allocated_lgcount -= 1;
  } while ((allocated_lgcount >= 3) && ((count >> (allocated_lgcount - 3)) == 0));
  at = realloc(at, fildesh_size_of_lgcount(element_size, allocated_lgcount));
  if (at) {
    *p_allocated_lgcount = allocated_lgcount;
  }
  return at;
}
