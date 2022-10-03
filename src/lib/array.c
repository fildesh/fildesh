#include "fildesh.h"
#include <stdlib.h>

  void*
realloc_more_FildeshA_(void* at, fildesh_lgsize_t* p_allocated_lgcount,
                       const size_t element_size, const size_t count)
{
  fildesh_lgsize_t allocated_lgcount = *p_allocated_lgcount;

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
realloc_less_FildeshA_(void* at, fildesh_lgsize_t* p_allocated_lgcount,
                       const size_t element_size, const size_t count)
{
  fildesh_lgsize_t allocated_lgcount = *p_allocated_lgcount;
  do {
    allocated_lgcount -= 1;
  } while ((allocated_lgcount >= 3) && ((count >> (allocated_lgcount - 3)) == 0));
  at = realloc(at, fildesh_size_of_lgcount(element_size, allocated_lgcount));
  if (at) {
    *p_allocated_lgcount = allocated_lgcount;
  }
  return at;
}
