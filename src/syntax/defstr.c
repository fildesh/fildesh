#include "defstr.h"

#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#include "symval.h"
#include "include/fildesh/fildesh_compat_string.h"

static inline
  bool
pfxeq_cstr(const char* pfx, const char* s)
{
  if (!s)  return false;
  return (0 == strncmp(pfx, s, strlen(pfx)));
}

static
  bool
skip_blank_bytes(FildeshX* in, size_t* text_nlines)
{
  size_t i;
  FildeshX slice = while_chars_FildeshX(in, fildesh_compat_string_blank_bytes);
  for (i = 0; i < slice.size; ++i) {
    if (slice.at[i] == '\n') {
      *text_nlines += 1;
    }
  }
  return (slice.size > 0);
}


static
  SymVal*
lookup_SymVal(FildeshKV* map, const char* s)
{
  if ((s[0] == '#' || isdigit(s[0])) && s[1] == '\0') {
    /* TODO(#99): Remove in v0.2.0.*/
    fildesh_log_warningf("For forward compatibility, please read positional arg %s via a flag.", s);
  }
  return (SymVal*) lookup_value_FildeshKV(map, s, strlen(s)+1);
}

static
  const char*
lookup_string_variable(FildeshKV* map, FildeshX* in, FildeshO* tmp_out)
{
  const char* result = NULL;
  truncate_FildeshO(tmp_out);
  putslice_FildeshO(tmp_out, *in);
  putc_FildeshO(tmp_out, '\0');
  if (pfxeq_cstr(".self.env.", tmp_out->at)) {
    tmp_out->off = 10;
    result = getenv(&tmp_out->at[tmp_out->off]);
  }
  else {
    const SymVal* sym = lookup_SymVal(map, tmp_out->at);
    if (sym) {
      result = sym->as.here_doc;
    }
  }
  truncate_FildeshO(tmp_out);
  return result;
}

  const char*
parse_double_quoted_fildesh_string(FildeshX* in, FildeshO* out, FildeshKV* map)
{
  const char* end_of_string = "\"";
  if (skipstr_FildeshX(in, "\"\"")) {
    end_of_string = "\"\"\"";  /* Multiline.*/
  }
  while (!skipstr_FildeshX(in, end_of_string)) {
    if (in->off >= in->size) {
      return "Unterminated double quote.";
    }
    else if (skipstr_FildeshX(in, "\\")) {
      bool have_char = true;
      char c;
      if (!peek_bytestring_FildeshX(in, NULL, 1)) {
        return "Unterminated double quote.";
      }
      c = in->at[in->off];
      switch(c) {
        case '0': {c = '\0'; break;}
        case 'n': {c = '\n'; break;}
        case 't': {c = '\t'; break;}
        case '\r':
        case '\n': {
          have_char = false;
          skipstr_FildeshX(in, "\r");
          skipstr_FildeshX(in, "\n");
          break;
        }
        case '"':
        case '$':
        case '\\':
        default: {break;}
      }
      if (have_char) {
        putc_FildeshO(out, c);
        in->off += 1;
      }
    }
    else if (map && skipstr_FildeshX(in, "$")) {
      FildeshX slice;
      SymVal* sym;
      if (!skipstr_FildeshX(in, "{")) {
        return "Please wrap varible name in curly braces when it is part of a string.";
      }
      slice = until_char_FildeshX(in, '}');
      if (in->off >= in->size) {
        return "Missing closing curly brace.";
      }
      in->at[in->off] = '\0';
      in->off += 1;
      sym = lookup_SymVal(map, slice.at);
      if (!sym || sym->kind != HereDocVal) {
        return "Variable not known at parse time.";
      }
      putstr_FildeshO(out, sym->as.here_doc);
    }
    else if (skipstr_FildeshX(in, "\r\n")) {
      putc_FildeshO(out, '\n');
    }
    else {
      putc_FildeshO(out, in->at[in->off]);
      in->off += 1;
    }
  }
  return NULL;
}


static
  const char*
parse_double_quoted_fildesh_string_or_variable(FildeshX* in, FildeshO* out, size_t* text_nlines, FildeshKV* map)
{
  const char* emsg = NULL;
  FildeshO tmp_out[1] = {DEFAULT_FildeshO};
  FildeshX slice;
  if (skipstr_FildeshX(in, "\"")) {
    emsg = parse_double_quoted_fildesh_string(in, out, NULL);
  }
  else if (!skipstr_FildeshX(in, "(")) {
    const char* v;
    slice = until_chars_FildeshX(in, ") \t\v\r\n");
    if (slice.size == 0) {return "No token.";}
    if (in->off >= in->size) {return "Unexpected end of file.";}
    v = lookup_string_variable(map, &slice, tmp_out);
    if (!v) {
      return "Unknown string variable.";
    }
    putstr_FildeshO(out, v);
  }
  else if (skipstr_FildeshX(in, "??")) {
    const char* v;
    if (!skip_blank_bytes(in, text_nlines)) {
      return "Need space after \"(??\".";
    }
    slice = until_chars_FildeshX(in, fildesh_compat_string_blank_bytes);
    if (slice.size == 0) {return "Need a second arg for \"(??\".";}
    v = lookup_string_variable(map, &slice, tmp_out);
    if (!skip_blank_bytes(in, text_nlines)) {return "Thought there was space here.";}
    emsg = parse_double_quoted_fildesh_string_or_variable(in, tmp_out, text_nlines, map);
    skip_blank_bytes(in, text_nlines);
    if (emsg) {
      /* Nothing.*/
    }
    else if (!skipstr_FildeshX(in, ")")) {
      emsg = "Need closing paren for \"(??\".";
    }
    else if (v) {
      putstr_FildeshO(out, v);
    }
    else {
      putslice_FildeshO(out, getslice_FildeshO(tmp_out));
    }
  }
  else if (skipstr_FildeshX(in, "++")) {
    if (!skip_blank_bytes(in, text_nlines)) {
      return "Need space after \"(++\".";
    }
    while (!peek_char_FildeshX(in, ')') && in->off < in->size) {
      emsg = parse_double_quoted_fildesh_string_or_variable(in, tmp_out, text_nlines, map);
      if (emsg) {break;}
      putslice_FildeshO(out, getslice_FildeshO(tmp_out));
      skip_blank_bytes(in, text_nlines);
      truncate_FildeshO(tmp_out);
    }
    if (!emsg && !skipstr_FildeshX(in, ")")) {emsg = "Need closing paren for \"(++\".";}
  }
  else {
    emsg = "";
  }
  close_FildeshO(tmp_out);
  return emsg;
}

  const char*
parse_fildesh_string_definition(
    FildeshX* in,
    size_t* text_nlines,
    FildeshKV* map,
    FildeshAlloc* alloc,
    FildeshO* tmp_out)
{
  FildeshX slice;
  const char* emsg;
  char* sym_name;
  char* sym_value = NULL;
  SymVal* sym;
  SymValKind sym_kind = NSymValKinds;

  truncate_FildeshO(tmp_out);

  for (skipchrs_FildeshX(in, " \t\v\r");
       !skipstr_FildeshX(in, "(: ");
       skipchrs_FildeshX(in, " \t\v\r"))
  {
    if (skipstr_FildeshX(in, "#")) {
      until_char_FildeshX(in, '\n');
    }
    if (skipstr_FildeshX(in, "\n")) {
      *text_nlines += 1;
    }
    else {
      return "";
    }
  }

  skip_blank_bytes(in, text_nlines);
  slice = until_chars_FildeshX(in, fildesh_compat_string_blank_bytes);
  sym_name = strdup_FildeshX(&slice, alloc);
  skip_blank_bytes(in, text_nlines);

  if (skipstr_FildeshX(in, "Filename")) {
    sym_kind = IOFileVal;
  }
  else if (skipstr_FildeshX(in, "Str")) {
    sym_kind = HereDocVal;
  }

  if (sym_kind == NSymValKinds || !skip_blank_bytes(in, text_nlines)) {
    return "Expected Filename or Str type.";
  }
  emsg = parse_double_quoted_fildesh_string_or_variable(
      in, tmp_out, text_nlines, map);
  if (emsg) {
    if (!emsg[0]) {return "idk";}
    return emsg;
  }
  if (tmp_out->size == 0) {
    return "Expected a closing paren.";
  }
  skip_blank_bytes(in, text_nlines);
  if (!skipstr_FildeshX(in, ")")) {
    return "Expected a closing paren.";
  }
  sym = declare_fildesh_SymVal(map, sym_kind, sym_name);
  if (!sym) {
    return "Cannot create new variable.";
  }
  sym_value = strdup_FildeshO(tmp_out, alloc);
  if (sym_kind == HereDocVal) {
    sym->as.here_doc = sym_value;
  }
  else {
    sym->as.iofilename = sym_value;
  }
  return NULL;
}
