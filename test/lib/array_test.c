#include <assert.h>
#include "fildesh.h"

int main() {
  FildeshA a[1] = {{NULL, 0, 0}};
  assert((void*)a == &a->at);
  assert(count_of_FildeshAT(a) == 0);
  return 0;
}
