#ifndef FILDESH_SXPROTO_SYNTAX_H_
#define FILDESH_SXPROTO_SYNTAX_H_
#include <string.h>

#include <fildesh/fildesh.h>

static const char sxpb_blank_chars[] = " \t\n\v\f\r";
static const char sxpb_delim_chars[] = " \t\n\v\f\r\"();";

static inline
  bool
is_sxpb_blank(char c)
{
  return NULL != memchr(
      sxpb_blank_chars, c, sizeof(sxpb_blank_chars)-1);
}

static inline
  bool
is_sxpb_delim(char c)
{
  return NULL != memchr(
      sxpb_delim_chars, c, sizeof(sxpb_delim_chars)-1);
}

static inline
  bool
has_sxpb_special_prefix(const char* s, size_t n)
{
  char c = (n > 0 ? s[0] : '\0');
  if (c == '+') {return true;}
  if (c == '-' || c == '.') {
    if (n == 1 || c == s[1]) {return false;}
    c = s[1];
    if (c == '+' || c == '-' || c == '.') {return true;}
  }
  return false;
}

static inline
  bool
has_sxpb_bare_prefix(const char* s, size_t n)
{
  static const char sxpb_bad_bare_prefix_chars[] =
    " \t\n\v\f\r\"();0123456789-+.";
  char c = (n > 0 ? s[0] : '\0');
  if (c == '\0') {return false;}
  if (c == '-' || c == '.') {
    if (n == 1 || c == s[1]) {return true;}
    c = s[1];
  }
  return NULL == memchr(
      sxpb_bad_bare_prefix_chars,
      c,
      sizeof(sxpb_bad_bare_prefix_chars)-1);
}

static inline
  bool
is_sxpb_bare_atom(const char* s, size_t n)
{
  size_t i;
  if (!has_sxpb_bare_prefix(s, n)) {return false;}
  for (i = 0; i < n; ++i) {
    if (is_sxpb_delim(s[i])) {return false;}
  }
  return true;
}

#endif
