
#include "mascii.h"
#include <assert.h>
#include <string.h>

#if defined(FILDESH_MASCII_INTRINSIC_MM)
#include <x86intrin.h>
#ifdef _OPENMP
/* #define FILDESH_MASCII_AUTOVECTORIZE_FAST */
#endif

#else
#define FILDESH_MASCII_AUTOVECTORIZE_FAST
#define FILDESH_MASCII_AUTOVECTORIZE_OKAY
#define FILDESH_MASCII_AUTOVECTORIZE_SLOW
#endif

static inline void
set0_FildeshMascii(FildeshMascii* mascii, char c) {
  assert(!(c & 128));
  mascii->at[c>>3] &= ~((unsigned char)1 << (unsigned char)(c & 7));
}

static inline void
set1_FildeshMascii(FildeshMascii* mascii, char c) {
  assert(!(c & 128));
  mascii->at[c>>3] |= ((unsigned char)1 << (unsigned char)(c & 7));
}

static inline unsigned char
get_FildeshMascii(const FildeshMascii* mascii, unsigned char c) {
  return (mascii->at[(c>>3) & 15] & (1 << (c & 7))) && (c < 128);
}

static inline FildeshMascii and_FildeshMascii(FildeshMascii a, FildeshMascii b) {
  FildeshMascii mascii;
#if defined(FILDESH_MASCII_AUTOVECTORIZE_FAST)
  unsigned i;
#ifdef _OPENMP
#pragma omp simd
#endif
  for (i = 0; i < 16; ++i) {
    mascii.at[i] = a.at[i] & b.at[i];
  }
#elif defined(FILDESH_MASCII_INTRINSIC_MM)
  mascii.mm = _mm_and_si128(a.mm, b.mm);
#else
  assert(false);
#endif
  return mascii;
}

static inline FildeshMascii mod16_FildeshMascii(FildeshMascii a) {
  FildeshMascii mascii;
#if defined(FILDESH_MASCII_AUTOVECTORIZE_FAST)
  unsigned i;
#ifdef _OPENMP
#pragma omp simd
#endif
  for (i = 0; i < 16; ++i) {
    mascii.at[i] = a.at[i] & 15;
  }
#elif defined(FILDESH_MASCII_INTRINSIC_MM)
  mascii.mm = _mm_and_si128(a.mm, _mm_set1_epi8(15));
#else
  assert(false);
#endif
  return mascii;
}

static inline FildeshMascii shuffle_FildeshMascii(FildeshMascii a, FildeshMascii b) {
  FildeshMascii mascii;
#if defined(FILDESH_MASCII_AUTOVECTORIZE_SLOW)
  unsigned i;
  b = mod16_FildeshMascii(b);
  /* GCC v10.2.1 does not auto-vectorize this.
   * Well, it does vectorize the first loop with OpenMP,
   * but the second loop is the important part!
   */
#ifdef _OPENMP
#pragma omp simd
#endif
  for (i = 0; i < 16; ++i) {
    mascii.at[i] = a.at[b.at[i]];
  }
#elif defined(FILDESH_MASCII_INTRINSIC_MM)
  b = mod16_FildeshMascii(b);
  mascii.mm = _mm_shuffle_epi8(a.mm, b.mm);
#else
  assert(false);
#endif
  return mascii;
}

static inline FildeshMascii lshift_1by_3bits_FildeshMascii(FildeshMascii a) {
#if 0
  /* No chance of auto-vectorizing because AVX
   * doesn't have shift instructions for 8-bit elements.
   */
  FildeshMascii mascii;
  unsigned i;
  for (i = 0; i < 16; ++i) {
    mascii.at[i] = 1 << (a.at[i] & 7);
  }
  return mascii;
#else
  static const FildeshMascii lut = {{
    1, 2, 4, 8, 16, 32, 64, 128,
    1, 2, 4, 8, 16, 32, 64, 128,
  }};
  return shuffle_FildeshMascii(lut, a);
#endif
}

static inline FildeshMascii shard_index_FildeshMascii(FildeshMascii a) {
  FildeshMascii mascii;
  /* We use 32-bit ints for this right shift because AVX
   * doesn't have shift instructions for 8-bit elements.
   * This is safe because only lowest 4 bits of the result will be used.
   */
#if defined(FILDESH_MASCII_AUTOVECTORIZE_FAST)
  unsigned i;
#ifdef _OPENMP
#pragma omp simd
#endif
  for (i = 0; i < 4; ++i) {
    mascii.at4x[i] = a.at4x[i] >> 3;
  }
#elif defined(FILDESH_MASCII_INTRINSIC_MM)
  mascii.mm = _mm_srli_epi32(a.mm, 3);
#else
  assert(false);
#endif
  return mascii;
}

static inline unsigned span0_ascii_FildeshMascii(FildeshMascii a, FildeshMascii b) {
  FildeshMascii mascii;
#if defined(FILDESH_MASCII_AUTOVECTORIZE_FAST)
  unsigned char i;
#ifdef _OPENMP
#pragma omp simd
#endif
  for (i = 0; i < 16; ++i) {
    a.at[i] = a.at[i] == 0 ? 255 : 0;
    b.at[i] = b.at[i] >= 128 ? 255 : 0;
    mascii.at[i] = ~(a.at[i] | b.at[i]);
  }
#elif defined(FILDESH_MASCII_INTRINSIC_MM)
  mascii.mm = _mm_setzero_si128();
  a.mm = _mm_cmpeq_epi8(a.mm, mascii.mm);
  b.mm = _mm_cmplt_epi8(b.mm, mascii.mm);
  mascii.mm = _mm_cmpeq_epi8(mascii.mm, _mm_or_si128(a.mm, b.mm));
#else
  assert(false);
#endif

#if defined(FILDESH_MASCII_AUTOVECTORIZE_OKAY)
  /* This test is relatively fast,
   * but I couldn't coax OpenMP to do a movemask.
   */
  if (mascii.at4x[0] || mascii.at4x[1] || mascii.at4x[2] || mascii.at4x[3]) {
    for (i = 0; i < 16; ++i) {
      if (mascii.at[i]) {
        return i;
      }
    }
  }
  return 16;
#elif defined(FILDESH_MASCII_INTRINSIC_MM)
  {
    int bits = _mm_movemask_epi8(mascii.mm);
    if (bits == 0) {return 16;}
    return _bit_scan_forward(bits);
  }
#else
  assert(false);
#endif
}


FildeshMascii charset_FildeshMascii(const char* needles, size_t n) {
  FildeshMascii mascii;
  size_t i;
  memset(mascii.at, 0, sizeof(mascii.at));
  for (i = 0; i < n; ++i) {
    set1_FildeshMascii(&mascii, needles[i]);
  }
  return mascii;
}

FildeshMascii charnot_FildeshMascii(const char* needles, size_t n) {
  FildeshMascii mascii;
  size_t i;
  memset(mascii.at, 255, sizeof(mascii.at));
  for (i = 0; i < n; ++i) {
    set0_FildeshMascii(&mascii, needles[i]);
  }
  return mascii;
}

static inline unsigned
get16_FildeshMascii(const FildeshMascii mascii, const FildeshMascii data) {
  FildeshMascii shard = shuffle_FildeshMascii(
      mascii,
      shard_index_FildeshMascii(data));
  FildeshMascii shard_mask = lshift_1by_3bits_FildeshMascii(data);
  return span0_ascii_FildeshMascii(and_FildeshMascii(shard, shard_mask), data);
}

  size_t
find_FildeshMascii(const FildeshMascii* mascii, const char* s, size_t n)
{
  size_t i;
  const FildeshMascii* t = (const FildeshMascii*) (
      ((uintptr_t)s + 15) & ~(uintptr_t)15);
  const size_t t_offset = (
      (uintptr_t)t - (uintptr_t)s >= n
      ? n
      : (uintptr_t)t - (uintptr_t)s);
  const size_t t_count = (
      (n - t_offset) / 16);

  for (i = 0; i < t_offset; ++i) {
    if (get_FildeshMascii(mascii, (unsigned char) s[i])) {
      return i;
    }
  }
  for (i = 0; i < t_count; ++i) {
    unsigned idx = get16_FildeshMascii(*mascii, t[i]);
    if (idx < 16) {
      return t_offset + 16*i + (size_t)idx;
    }
  }
  for (i = t_offset + 16 * t_count; i < n; ++i) {
    if (get_FildeshMascii(mascii, (unsigned char) s[i])) {
      return i;
    }
  }
  return n;
}

