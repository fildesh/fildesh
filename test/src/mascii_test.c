#include "fildesh.h"
#include "src/lib/mascii.h"
#include <assert.h>
#include <string.h>
#include <stdio.h>

static void alignment_test() {
  assert(sizeof(FildeshMascii) == 16);
#if defined(__TINYC__)
  if (fildesh_alignof(FildeshMascii) != 16) {
    fildesh_log_warningf(
        "Alignment of FildeshMascii is %u, not 16, on TinyCC.",
        (unsigned)fildesh_alignof(FildeshMascii));
  }
#else
  assert(fildesh_alignof(FildeshMascii) == 16);
#endif
}

static void charset_short_test() {
  size_t got;
  FildeshMascii mascii;
  mascii = charset_FildeshMascii(NULL, 0);
  got = find_FildeshMascii(&mascii, "abcd", 4);
  assert(got == 4);
  mascii = charset_FildeshMascii("d", 1);
  got = find_FildeshMascii(&mascii, "abcd", 4);
  assert(got == 3);
  mascii = charset_FildeshMascii("cd", 2);
  got = find_FildeshMascii(&mascii, "abcd", 4);
  assert(got == 2);
  mascii = charset_FildeshMascii("bcd", 3);
  got = find_FildeshMascii(&mascii, "abcd", 4);
  assert(got == 1);
  mascii = charset_FildeshMascii("abcd", 4);
  got = find_FildeshMascii(&mascii, "abcd", 4);
  assert(got == 0);
  mascii = charset_FildeshMascii("1234efgh", 8);
  got = find_FildeshMascii(&mascii, "abcd", 4);
  assert(got == 4);
}

static void charnot_short_test() {
  size_t got;
  FildeshMascii mascii;
  mascii = charnot_FildeshMascii(NULL, 0);
  got = find_FildeshMascii(&mascii, "abcd", 4);
  assert(got == 0);
  mascii = charnot_FildeshMascii("a", 1);
  got = find_FildeshMascii(&mascii, "abcd", 4);
  assert(got == 1);
  mascii = charnot_FildeshMascii("ab", 2);
  got = find_FildeshMascii(&mascii, "abcd", 4);
  assert(got == 2);
  mascii = charnot_FildeshMascii("abc", 3);
  got = find_FildeshMascii(&mascii, "abcd", 4);
  assert(got == 3);
  mascii = charnot_FildeshMascii("abcd", 4);
  got = find_FildeshMascii(&mascii, "abcd", 4);
  assert(got == 4);
  mascii = charnot_FildeshMascii("bcd", 3);
  got = find_FildeshMascii(&mascii, "abcd", 4);
  assert(got == 0);
}

static void no_simd_test() {
  size_t got;
  FildeshMascii mascii;
  FildeshAlloc* alloc = open_FildeshAlloc();
  char* const size15_aligned16 = (char*)
    reserve_FildeshAlloc(alloc, 15, 16);
  char* const size15_aligned16plus1 = 1 + (char*)
    reserve_FildeshAlloc(alloc, 16, 16);
  char* const size16_aligned16plus1 = 1 + (char*)
    reserve_FildeshAlloc(alloc, 17, 16);
  char* const size30_aligned16plus1 = 1 + (char*)
    reserve_FildeshAlloc(alloc, 31, 16);

  assert(((uintptr_t)size15_aligned16 & 15) == 0);
  assert(((uintptr_t)size15_aligned16plus1 & 15) == 1);
  assert(((uintptr_t)size16_aligned16plus1 & 15) == 1);
  assert(((uintptr_t)size30_aligned16plus1 & 15) == 1);

  mascii = charset_FildeshMascii("abc9de", 6);
  memcpy(size15_aligned16, "0123456789abcde", 15);
  got = find_FildeshMascii(&mascii, size15_aligned16, 15);
  assert(got < 15);
  assert(got == 9);

  mascii = charset_FildeshMascii("abc9de", 6);
  memcpy(size15_aligned16plus1, "123456789abcdef", 15);
  got = find_FildeshMascii(&mascii, size15_aligned16plus1, 15);
  assert(got < 15);
  assert(got == 8);

  mascii = charset_FildeshMascii("gh0st", 6);
  memcpy(size16_aligned16plus1, "123456789abcdef0", 16);
  got = find_FildeshMascii(&mascii, size16_aligned16plus1, 16);
  assert(got < 16);
  assert(got == 15);

  mascii = charset_FildeshMascii("tuls", 4);
  memcpy(size30_aligned16plus1, "0123456789abcdefghijklmnopqrst", 30);
  got = find_FildeshMascii(&mascii, size30_aligned16plus1, 30);
  assert(got < 30);
  assert(got == 21);

  close_FildeshAlloc(alloc);
}

static void only_simd_test() {
  size_t got;
  FildeshMascii mascii;
  FildeshAlloc* alloc = open_FildeshAlloc();
  char* const size16_aligned16 = (char*)
    reserve_FildeshAlloc(alloc, 16, 16);
  char* const size32_aligned32 = (char*)
    reserve_FildeshAlloc(alloc, 32, 32);
  char* const size32_aligned32plus16 = 16 + (char*)
    reserve_FildeshAlloc(alloc, 64, 32);

  assert(((uintptr_t)size16_aligned16 & 15) == 0);
  assert(((uintptr_t)size32_aligned32 & 31) == 0);
  assert(((uintptr_t)size32_aligned32plus16 & 31) == 16);

  mascii = charset_FildeshMascii("abc9de", 6);
  memcpy(size16_aligned16, "0123456789abcdef", 16);
  got = find_FildeshMascii(&mascii, size16_aligned16, 16);
  assert(got < 16);
  assert(got == 9);

  mascii = charset_FildeshMascii("tuls", 4);
  memcpy(size16_aligned16, "01234lmnopqrstuv", 16);
  got = find_FildeshMascii(&mascii, size16_aligned16, 16);
  assert(got < 16);
  assert(got == 5);

  mascii = charset_FildeshMascii("tuls", 4);
  memcpy(size32_aligned32, "0123456789abcdefghijklmnopqrstuv", 32);
  got = find_FildeshMascii(&mascii, size32_aligned32, 32);
  assert(got < 32);
  assert(got == 21);

  mascii = charset_FildeshMascii("tuls", 4);
  memcpy(size32_aligned32plus16, "0123456789abcdefghijklmnopqrstuv", 32);
  got = find_FildeshMascii(&mascii, size32_aligned32plus16, 32);
  assert(got < 32);
  assert(got == 21);

  close_FildeshAlloc(alloc);
}

static void historically_problematic_test() {
  static const unsigned char needle0[] = {0x00};
  static const unsigned char haystack0[] = {
    0x16, 0x16, 0x01, 0x80, 0x16, 0x16, 0x00, 0x16,
    0x00, 0x16, 0x16, 0x9C, 0x9C, 0x26, 0x00, 0xCD,
  };
  size_t got;
  FildeshMascii mascii;
  FildeshAlloc* alloc = open_FildeshAlloc();
  char* haystack = reserve_FildeshAlloc(alloc, 1000, 16);

  mascii = charset_FildeshMascii((char*)needle0, sizeof(needle0));
  memcpy(haystack, haystack0, sizeof(haystack0));
  got = find_FildeshMascii(&mascii, haystack, sizeof(haystack0));
  assert(got < sizeof(haystack0));
  /* 0x80 at index 3 matched when abs() was used to determine validity.
   * There is no abs(-128) defined for char!
   */
  assert(got != 3);
  assert(got == 6);

  close_FildeshAlloc(alloc);
}

int main() {
  alignment_test();
  charset_short_test();
  charnot_short_test();
  no_simd_test();
  only_simd_test();
  historically_problematic_test();
  return 0;
}
