
#include "lace.h"
#include <assert.h>

int main()
{
  /* Always true.*/
  assert(sizeof(char) == 1);
  /* POSIX says a char is a 8 bits.*/
  assert(UCHAR_MAX == 255);
  /* Need enough bits to hold the base-2 log of a size.*/
  assert(LACE_LGSIZE_MAX > 8*sizeof(size_t));
  return 0;
}
