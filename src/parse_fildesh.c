
#include "parse_fildesh.h"

#include <string.h>
#include "fildesh_compat_string.h"


  static unsigned
count_ws(const char* s)
{
  return strspn (s, fildesh_compat_string_blank_bytes);
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

  /* Check for the single-line case.*/
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

