#include "fildesh.h"

#include <assert.h>
#include <stdlib.h>


static
  bool
testbit_FildeshA_(const char data[], size_t bitpos)
{
  return 0 != (1 & (data[bitpos/CHAR_BIT] >> (bitpos % CHAR_BIT)));
}

extern
  int
LLVMFuzzerTestOneInput(const uint8_t data[], size_t size) {
  size_t i;
  bool growing = true;
  size_t difference = 0;

  int* at = NULL;
  size_t count = 0;
  fildesh_lgsize_t allocated_lgcount = 0;

  for (i = 0; i < size * CHAR_BIT; ++i) {
    size_t j;
    if (testbit_FildeshA_((char*) data, i)) {
      difference += 1;
      continue;
    }

    if (difference > 0) {
      if (growing) {
        int* dst = (int*)
          grow_FildeshA_((void**)&at, &count, &allocated_lgcount,
                      sizeof(int), difference, realloc);
        for (j = 0; j < difference; ++j) {
          dst[j] = (int) j;
          assert(at[count-difference+j] == (int)j);
        }
      } else {
        if (difference > count) {
          difference = count;
        }
        mpop_FildeshA_((void**)&at, &count, &allocated_lgcount,
                    sizeof(int), difference, realloc);
      }
      difference = 0;
    }
    /* Ensure that we can write to all positions in the array.*/
    for (j = 0; j < count; ++j) {
      at[j] = count;
    }
    growing = !growing;
  }

  if (at) {
    free(at);
    at = NULL;
  }
  return 0;
}

