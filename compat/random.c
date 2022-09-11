#include "fildesh_compat_random.h"
#include "fildesh_compat_string.h"


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

  size_t
fildesh_compat_random_hex(char* buf, size_t capacity)
{
  size_t n = fildesh_compat_random_bytes(buf, (capacity+1)/2);
  size_t i;
  if (2*n < capacity) {
    capacity = 2*n;
    buf[capacity] = '\0';
  }
  for (i = n; i < capacity; ++i) {
    /* High 4 bytes to hex char.*/
    buf[i] = fildesh_compat_string_hexchar((unsigned)buf[i-n] >> 4);
  }
  for (i = 0; i < n; ++i) {
    /* Low 4 bytes to hex char.*/
    buf[i] = fildesh_compat_string_hexchar((unsigned)buf[i]);
  }
  return capacity;
}

