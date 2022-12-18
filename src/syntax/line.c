
#include "line.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "defstr.h"
#include "include/fildesh/fildesh_compat_string.h"


static
  bool
pfxeq_cstr(const char* pfx, const char* s)
{
  if (!s)  return false;
  return (0 == strncmp(pfx, s, strlen (pfx)));
}
  static unsigned
count_ws(const char* s)
{
  return strspn (s, fildesh_compat_string_blank_bytes);
}
  static unsigned
count_non_ws (const char* s)
{
  return strcspn (s, fildesh_compat_string_blank_bytes);
}
  static unsigned
trim_trailing_ws (char* s)
{
  unsigned n = strlen (s);
  while (0 < n && strchr (fildesh_compat_string_blank_bytes, s[n-1]))  --n;
  s[n] = '\0';
  return n;
}


  char*
fildesh_syntax_parse_line(
    FildeshX* xf, size_t* text_nlines,
    FildeshAlloc* alloc, FildeshO* tmp_out)
{
  char* s;

  truncate_FildeshO(tmp_out);
  while ((s = getline_FildeshX(xf)))
  {
    unsigned n;
    bool multiline = false;
    *text_nlines += 1;

    s = &s[count_ws(s)];
    if (s[0] == '#' || s[0] == '\0')  continue;

    n = trim_trailing_ws (s);

    multiline = s[n-1] == '\\';
    if (multiline)
      --n;

    put_bytestring_FildeshO(tmp_out, (unsigned char*)s, n);

    if (!multiline)  break;
  }
  if (tmp_out->size == 0) {
    return NULL;
  }
  return strdup_FildeshO(tmp_out, alloc);
}


  char*
fildesh_syntax_maybe_concatenate_args(
    unsigned argc, const char* const* argv,
    FildeshAlloc* alloc)
{
  size_t n = 0;
  unsigned i;
  char* s;
  for (i = 0; i < argc; ++i) {
    if (argv[i][0] == '$' && argv[i][1] == '(') {
      return NULL;
    }
    n += strlen(argv[i]);
  }
  s = fildesh_allocate(char, n+1, alloc);
  s[n] = '\0';
  for (i = argc; i > 0; --i) {
    size_t size = strlen(argv[i-1]);
    n -= size;
    memcpy(&s[n], argv[i-1], size);
  }
  return s;
}


/** HERE document is created by
 * $(H var_name) Optional identifying stuff.
 * Line 1 in here.
 * Line 2 in here.
 * ...
 * Line n in here.
 * $(H var_name) Optional identifying stuff.
 *
 * OR it could look like:
 * $(H: var_name) value
 **/
  char*
fildesh_syntax_parse_here_doc(
    FildeshX* in, const char* term, size_t* text_nlines,
    FildeshAlloc* alloc, FildeshO* tmp_out)
{
  const size_t term_length = strlen(term);
  FildeshX slice;
  truncate_FildeshO(tmp_out);

  /* Check for the single-line case.
   * TODO(#94): Disallow this in the future.
   */
  if (term[3] == ':')
  {
    char* s;
    term = strchr(term, ')');
    if (!term) {
      return strdup_FildeshAlloc(alloc, "");
    }
    term = &term[1];
    term = &term[count_ws(term)];
    s = strdup_FildeshAlloc(alloc, term);
    trim_trailing_ws(s);
    return s;
  }

  for (slice = sliceline_FildeshX(in);
       slice.at;
       slice = sliceline_FildeshX(in))
  {
    *text_nlines += 1;
    if (skip_bytestring_FildeshX(&slice, (unsigned char*)term, term_length)) {
      break;
    }
    put_bytestring_FildeshO(tmp_out, (unsigned char*)slice.at, slice.size);
    putc_FildeshO(tmp_out, '\n');
  }
  if (tmp_out->size > 0) {tmp_out->size -= 1;}

  return strdup_FildeshO(tmp_out, alloc);
}

  const char*
fildesh_syntax_sep_line(
    char*** args, char* s, FildeshKV* map,
    FildeshAlloc* alloc, FildeshO* tmp_out)
{
  while (1) {
    s = &s[count_ws (s)];
    if (s[0] == '\0')  break;

    if (s[0] == '\'') {
      unsigned i;
      s = &s[1];
      push_FildeshAT(args, s);
      i = strcspn (s, "'");
      if (s[i] == '\0') {
        return "Unterminated single quote.";
      }
      s = &s[i];
    }
    else if (s[0] == '"') {
      FildeshX slice = DEFAULT_FildeshX;
      const char* emsg;
      slice.size = strlen(s);
      slice.at = s;
      slice.off = 1;
      truncate_FildeshO(tmp_out);
      emsg = parse_double_quoted_fildesh_string(&slice, tmp_out, map);
      if (emsg) {return emsg;}
      s = &s[slice.off];
      push_FildeshAT(args, strdup_FildeshO(tmp_out, alloc));
      truncate_FildeshO(tmp_out);
    }
    else if (pfxeq_cstr("$(getenv ", s)) {
      FildeshX in[1] = {DEFAULT_FildeshX};
      FildeshX slice;
      in->at = s;
      in->size = strlen(s);
      in->off = strlen("$(getenv ");
      while_chars_FildeshX(in, " ");
      slice = until_char_FildeshX(in, ')');
      if (slice.at) {
        const char* v;
        in->at[in->off++] = '\0';
        s = &in->at[in->off];
        v = getenv(slice.at);
        if (!v) {v = "";}
        push_FildeshAT(args, strdup_FildeshAlloc(alloc, v));
      }
      else {
        return "Unterminated environment variable.";
      }
    }
    else if (s[0] == '$' && s[1] == '(') {
      unsigned i;
      push_FildeshAT(args, s);
      s = &s[2];
      i = strcspn (s, ")");
      if (s[i] == '\0') {
        return "Unterminated variable.";
      }
      s = &s[i+1];
    }
    else {
      push_FildeshAT(args, s);
      s = &s[count_non_ws (s)];
    }
    if (s[0] == '\0')  break;
    s[0] = '\0';
    s = &s[1];
  }
  return NULL;
}

