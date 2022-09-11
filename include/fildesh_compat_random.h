#ifndef FILDESH_COMPAT_RANDOM_H_
#define FILDESH_COMPAT_RANDOM_H_
#include <stddef.h>

size_t
fildesh_compat_random_bytes(void* buf, size_t capacity);
size_t
fildesh_compat_random_hex(char* buf, size_t capacity);

#endif
