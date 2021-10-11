#include "fildesh_compat_string.h"
#include <string.h>
#include <stdlib.h>

  char*
lace_compat_string_byte_translate(
    const char* haystack,
    const char* needles,
    const char* const* replacements,
    const char* lhs, const char* rhs)
{
  unsigned char replacement_length_lut[256];
  unsigned char replacement_index_lut[256];
  size_t lhs_length, rhs_length;
  char* dst;
  size_t offset;

  { /* Preprocess.*/
    unsigned i;
    for (i = 0; needles[i] != '\0'; ++i) {
      size_t n = strlen(replacements[i]);
      const unsigned needle = (unsigned)needles[i];
      if (n >= 256) {return NULL;}
      replacement_length_lut[needle] = (unsigned char) n;
      replacement_index_lut[needle] = (unsigned char) i;
    }

    if (!lhs) {lhs = "";}
    if (!rhs) {rhs = "";}
    lhs_length = strlen(lhs);
    rhs_length = strlen(rhs);
  }

  { /* Count.*/
    size_t n;
    size_t i = 0;
    offset = lhs_length;
    for (n = strcspn(haystack, needles);
         haystack[i+n] != '\0';
         n = strcspn(&haystack[i], needles))
    {
      const unsigned needle = (unsigned) haystack[i+n];
      offset += n + replacement_length_lut[needle];
      i += n+1;
    }
    offset += n + rhs_length;
  }

  /* Allocate.*/
  dst = (char*) malloc(offset+1);

  { /* Copy.*/
    size_t n;
    size_t i = 0;
    offset = lhs_length;
    memcpy(dst, lhs, lhs_length);
    for (n = strcspn(haystack, needles);
         haystack[i+n] != '\0';
         n = strcspn(&haystack[i], needles))
    {
      const unsigned needle = (unsigned) haystack[i+n];
      memcpy(&dst[offset], &haystack[i], n);
      memcpy(&dst[offset+n],
             replacements[replacement_index_lut[needle]],
             replacement_length_lut[needle]);
      i += n+1;
      offset += n + replacement_length_lut[needle];
    }
    memcpy(&dst[offset], &haystack[i], n);
    offset += n;
    memcpy(&dst[offset], rhs, rhs_length);
    offset += rhs_length;
    dst[offset] = '\0';
  }
  return dst;
}

  char*
lace_compat_string_duplicate(const char* s)
{
  char* p;
  size_t size;
  if (!s) {return NULL;}
  size = strlen(s)+1;
  p = (char*) malloc(size);
  if (!p) {return NULL;}
  memcpy(p, s, size);
  return p;
}

