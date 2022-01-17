#include "fildesh_compat_random.h"


#if defined(__linux__)
#include <sys/random.h>

#elif defined(_MSC_VER)
#include <windows.h>
#include <ntsecapi.h>

#else
#include <stdlib.h>
#endif


  size_t
fildesh_compat_random_bytes(void* buf, size_t capacity)
{
#if defined(__linux__)
  ssize_t n = getrandom(buf, capacity, 0);
  if (n <= 0) {return 0;}
  return n;
#elif defined(_MSC_VER)
  if (RtlGenRandom(buf, capacity)) {
    return capacity;
  } else {
    return 0;
  }
#else
  arc4random_buf(buf, capacity);
  return capacity;
#endif
}

