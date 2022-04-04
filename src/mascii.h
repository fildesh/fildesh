#ifndef FILDESH_MASCII_H_
#define FILDESH_MASCII_H_

#include "fildesh.h"

#if defined(__SSSE3__) || defined(__AVX__) || defined(__AVX2__)
#include <tmmintrin.h>
#define FILDESH_MASCII_INTRINSIC_MM
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef union FildeshMascii FildeshMascii;

#if defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 201112L)
#define fildesh_alignas(n)  _Alignas(n)
#elif defined(__cplusplus) && (__cplusplus >= 201103L)
#define fildesh_alignas(n)  alignas(n)
#elif defined(_MSC_VER)
#define fildesh_alignas(n)  __declspec(align(n))
#else
#define fildesh_alignas(n)  __attribute__((aligned(n)))
#endif

union FildeshMascii {
  fildesh_alignas(16) unsigned char at[16];
  fildesh_alignas(16) uint32_t at4x[4];
#ifdef FILDESH_MASCII_INTRINSIC_MM
  __m128i mm;
#endif
};

FildeshMascii charset_FildeshMascii(const char*, size_t);
FildeshMascii charnot_FildeshMascii(const char*, size_t);
size_t find_FildeshMascii(const FildeshMascii*, const char*, size_t);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
