#include "fildesh_compat_random.h"
#include <assert.h>
#include <string.h>

int main() {
  char buf1[256];
  char buf2[256];
  const size_t capacity = 256;
  size_t n;
  n = lace_compat_random_bytes(buf1, capacity);
  assert(n == capacity);
  do {
    n = lace_compat_random_bytes(buf2, capacity);
    assert(n == capacity);
    /* Much like randomness is used to break symmetry and make progress,
     * this test will run forever if bytes never differ.
     */
  } while (0 == memcmp(buf1, buf2, capacity));
  return 0;
}

