
#include <fildesh/fildesh.h>
#include <assert.h>

int main()
{
  /* Always true.*/
  assert(sizeof(char) == 1);
  /* POSIX says a char is a 8 bits.*/
  assert(CHAR_BIT == 8);
  assert(UCHAR_MAX == 255);
  /* Need enough bits to hold the base-2 log of a size.*/
  assert(FILDESH_LGSIZE_MAX > 8*sizeof(size_t));
  /* Pointers are at least as big as the sizes of things.*/
  assert(sizeof(size_t) <= sizeof(uintptr_t));

  /* Alignment should be accurate.*/
  assert(sizeof(int) == fildesh_alignof(int));
  return 0;
}
