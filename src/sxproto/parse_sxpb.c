#include "src/sxproto/parse_sxpb.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>


static const char sxpb_delim_bytes[] = " \t\n\v\f\r\"();";
static const char sxpb_blank_bytes[] = " \t\n\v\f\r";

static void
syntax_error(const FildeshSxpbInfo* info, const char* msg)
{
  FildeshO* const out = info->err_out;
  if (!out) {return;}
  putstrlit_FildeshO(out, "Line ");
  print_size_FildeshO(out, info->line_count+1);
  putstrlit_FildeshO(out, " column ");
  print_size_FildeshO(out, info->column_count+1);
  putstrlit_FildeshO(out, ": ");
  putstr_FildeshO(out, msg);
  putc_FildeshO(out, '\n');
  flush_FildeshO(out);
}

static void
progress_FildeshSxpbInfo(FildeshSxpbInfo* info, const FildeshX* slice)
{
  const char* s = (char*)memchr(slice->at, '\n', slice->size);
  info->column_count += slice->size;
  while (s) {
    size_t n = &slice->at[slice->size] - s;
    info->line_count += 1;
    info->column_count = n;
    s = (char*)memchr(&s[1], '\n', n-1);
  }
}

static inline FildeshX
until_chars_FildeshSxpbInfo(FildeshSxpbInfo* info, FildeshX* in, const char* s)
{
  const FildeshX slice = until_chars_FildeshX(in, s);
  progress_FildeshSxpbInfo(info, &slice);
  return slice;
}

static inline FildeshX
while_chars_FildeshSxpbInfo(FildeshSxpbInfo* info, FildeshX* in, const char* s)
{
  const FildeshX slice = while_chars_FildeshX(in, s);
  progress_FildeshSxpbInfo(info, &slice);
  return slice;
}

static inline
  bool
skipstr_FildeshSxpbInfo(FildeshSxpbInfo* info, FildeshX* in, const char* s)
{
  const size_t n = strlen(s);
  if (skip_bytestring_FildeshX(in, (const unsigned char*)s, n)) {
    info->column_count += n;
    return true;
  }
  return false;
}

static
  void
skip_separation(FildeshX* in, FildeshSxpbInfo* info)
{
  FildeshX slice;
  do {
    /* Line comment.*/
    if (peek_char_FildeshX(in, ';')) {
      until_chars_FildeshSxpbInfo(info, in, "\n");
    }
    slice = while_chars_FildeshSxpbInfo(info, in, sxpb_blank_bytes);
  } while (slice.size > 0);
}

static
  bool
parse_quoted_string_FildeshSxpbInfo(
    FildeshSxpbInfo* info,
    FildeshX* in,
    FildeshO* oslice)
{
  FildeshX slice;
  bool multiline_on = false;

  assert(in->off < in->size && in->at[in->off] == '"');
  skipstr_FildeshSxpbInfo(info, in, "\"");
  multiline_on = skipstr_FildeshSxpbInfo(info, in, "\"\"");

  for (slice = until_chars_FildeshSxpbInfo(info, in, "\\\"\r");
       avail_FildeshX(in);
       slice = until_chars_FildeshSxpbInfo(info, in, "\\\"\r"))
  {
    putslice_FildeshO(oslice, slice);
    if (skipstr_FildeshSxpbInfo(info, in, "\"")) {
      if (!multiline_on || skipstr_FildeshSxpbInfo(info, in, "\"\"")) {
        info->unquoted_value_separation_on = false;
        return true;
      }
      putc_FildeshO(oslice, '"');
    }
    else if (skipstr_FildeshSxpbInfo(info, in, "\\\"")) {
      putc_FildeshO(oslice, '"');
    }
    else if (skipstr_FildeshSxpbInfo(info, in, "\\\\")) {
      putc_FildeshO(oslice, '\\');
    }
    else if (skipstr_FildeshSxpbInfo(info, in, "\\t")) {
      putc_FildeshO(oslice, '\t');
    }
    else if (skipstr_FildeshSxpbInfo(info, in, "\\n")) {
      putc_FildeshO(oslice, '\n');
    }
    else if (skipstr_FildeshSxpbInfo(info, in, "\\v")) {
      putc_FildeshO(oslice, '\v');
    }
    else if (skipstr_FildeshSxpbInfo(info, in, "\\f")) {
      putc_FildeshO(oslice, '\f');
    }
    else if (skipstr_FildeshSxpbInfo(info, in, "\\r")) {
      putc_FildeshO(oslice, '\r');
    }
    else if (skipstr_FildeshSxpbInfo(info, in, "\\\n") ||
             skipstr_FildeshSxpbInfo(info, in, "\\\r\n")) {
      /* Ignore escaped newline.*/
    }
    else if (skipstr_FildeshSxpbInfo(info, in, "\r")) {
      /* Ignore carriage return.*/
    }
    else {
      syntax_error(info, "Unknown escape sequence. Only very basic ones are supported.");
      return false;
    }
  }

  truncate_FildeshO(oslice);
  syntax_error(info, "Expected closing double quote.");
  return false;
}

static
  bool
parse_bare_string_FildeshSxpbInfo(
    FildeshSxpbInfo* info,
    FildeshX* in,
    FildeshO* oslice)
{
  FildeshX xslice = until_chars_FildeshSxpbInfo(info, in, sxpb_delim_bytes);
  assert(xslice.size > 0);
  if (info->unquoted_value_separation_on) {
    putc_FildeshO(oslice, ' ');
  }
  putslice_FildeshO(oslice, xslice);
  info->unquoted_value_separation_on = true;
  return true;
}

  bool
parse_concat_string_FildeshSxpbInfo(
    FildeshSxpbInfo* info,
    FildeshX* in,
    FildeshO* oslice)
{
  if (oslice->size == 0) {
    info->unquoted_value_separation_on = false;
  }
  if (peek_char_FildeshX(in, '"')) {
    return parse_quoted_string_FildeshSxpbInfo(info, in, oslice);
  }
  return parse_bare_string_FildeshSxpbInfo(info, in, oslice);
}

static
  bool
parse_string_field_content_FildeshSxpbInfo(
    FildeshSxpbInfo* info,
    FildeshX* in,
    FildeshO* oslice)
{
  truncate_FildeshO(oslice);
  if (!parse_concat_string_FildeshSxpbInfo(info, in, oslice)) {
    return false;
  }
  for (skip_separation(in, info);
       avail_FildeshX(in) && !peek_char_FildeshX(in, ')');
       skip_separation(in, info))
  {
    if (peek_char_FildeshX(in, '(')) {
      syntax_error(info, "Unexpected open paren in string.");
      return false;
    }
    if (!parse_concat_string_FildeshSxpbInfo(info, in, oslice)) {
      return false;
    }
  }
  return true;
}

static void skip_leading_zeroes(FildeshX* in) {
  while (in->off < in->size && in->at[in->off] == '0') {
    in->off += 1;
  }
}

  bool
parse_number_FildeshSxpbInfo(
    FildeshSxpbInfo* info,
    FildeshX* in,
    FildeshO* oslice)
{
  FildeshX slice;
  int exponent = 0;
  bool fractional = false;
  truncate_FildeshO(oslice);
  if (skipstr_FildeshSxpbInfo(info, in, "-")) {
    putc_FildeshO(oslice, '-');
  }
  else {
    skipstr_FildeshSxpbInfo(info, in, "+");
    putc_FildeshO(oslice, '+');
  }
  assert(oslice->size == 1);
  slice = while_chars_FildeshSxpbInfo(info, in, "0123456789");
  skip_leading_zeroes(&slice);
  putslice_FildeshO(oslice, slice);
  if (skipstr_FildeshSxpbInfo(info, in, ".")) {
    fractional = true;
    slice = while_chars_FildeshSxpbInfo(info, in, "0123456789");
    exponent = -(int)slice.size;
    if (oslice->size == 1) {
      skip_leading_zeroes(&slice);
    }
    putslice_FildeshO(oslice, slice);
  }
  if (skipstr_FildeshSxpbInfo(info, in, "e") ||
      skipstr_FildeshSxpbInfo(info, in, "E"))
  {
    int exponent_diff = 0;
    fractional = true;
    slice = while_chars_FildeshSxpbInfo(info, in, "+-0123456789");
    if (!parse_int_FildeshX(&slice, &exponent_diff)) {
      syntax_error(info, "Cannot parse exponent.");
      return false;
    }
    exponent += exponent_diff;
  }
  if (oslice->size == 1) {
    fractional = false;
    putc_FildeshO(oslice, '0');
  }
  if (fractional) {
    size_t i = oslice->size;
    bool zeroes_trailing = true;
    assert(oslice->size >= 2);
    putc_FildeshO(oslice, '.');
    while (i > 2) {
      oslice->at[i] = oslice->at[i-1];
      oslice->at[i-1] = '.';
      exponent += 1;
      if (zeroes_trailing && oslice->at[i] == '0') {
        oslice->size -= 1;
      }
      else {
        zeroes_trailing = false;
      }
      i -= 1;
    }
    putc_FildeshO(oslice, 'e');
    if (exponent < 0) {
      exponent = -exponent;
      putc_FildeshO(oslice, '-');
    }
    else {
      putc_FildeshO(oslice, '+');
    }
    print_int_FildeshO(oslice, exponent);
  }
  return true;
}

static
  bool
on_array_content(FildeshX* in)
{
  if (!peek_char_FildeshX(in, '(')) {
    return true;
  }
  if (peek_bytestring_FildeshX(in, fildesh_bytestrlit("(()"))) {
    return true;
  }
  if (peek_bytestring_FildeshX(in, fildesh_bytestrlit("(\"\" "))) {
    return true;
  }
  return false;
}

  bool
parse_name_FildeshSxpbInfo(
    FildeshSxpbInfo* info,
    FildeshX* in,
    FildeshO* oslice,
    unsigned* ret_nesting_depth)
{
  FildeshX slice;
  unsigned nesting_depth = *ret_nesting_depth;
  if (nesting_depth != 3) {
    if (skipstr_FildeshSxpbInfo(info, in, "(\"\")")) {
      /* Nest discriminator with empty name.*/
      skip_separation(in, info);
      truncate_FildeshO(oslice);
      *ret_nesting_depth = 4;
      return true;
    }
    if (peek_bytestring_FildeshX(in, fildesh_bytestrlit("()"))) {
      /* Will parse this later.*/
      nesting_depth = 0;
    }
    else if (skipstr_FildeshSxpbInfo(info, in, "(")) {
      nesting_depth = 1;
      skip_separation(in, info);
    }
  }
  slice = until_chars_FildeshSxpbInfo(info, in, sxpb_delim_bytes);
  truncate_FildeshO(oslice);
  if (slice.size == 0) {
    if (peek_char_FildeshX(in, '"')) {
      if (!parse_quoted_string_FildeshSxpbInfo(info, in, oslice)) {
        return false;
      }
    }
  }
  else {
    putslice_FildeshO(oslice, slice);
  }

  if (nesting_depth == 3) {
    nesting_depth = 0;
    skip_separation(in, info);
    if (!skipstr_FildeshSxpbInfo(info, in, ")")) {
      syntax_error(info, "Expected closing paren after loneof selection name.");
      return false;
    }
    if (oslice->size == 0) {
      syntax_error(info, "Expected loneof selection name.");
      return false;
    }
  }

  skip_separation(in, info);
  if (nesting_depth == 0) {
    if (skipstr_FildeshSxpbInfo(info, in, "()")) {
      skip_separation(in, info);
      if (oslice->size == 0) {
        nesting_depth = 0;
      }
      else {
        nesting_depth = 5;
      }
    }
    else if (skipstr_FildeshSxpbInfo(info, in, "(())")) {
      skip_separation(in, info);
      if (oslice->size == 0) {
        nesting_depth = 1;
      }
      else if (on_array_content(in)) {
        nesting_depth = 1;
      }
      else {
        nesting_depth = 2;
      }
    }
    else if (skipstr_FildeshSxpbInfo(info, in, "(\"\")")) {
      nesting_depth = 4;
    }
  }
  else {
    assert(nesting_depth == 1);
    if (skipstr_FildeshSxpbInfo(info, in, ")")) {
      nesting_depth = 2;
    }
    else {
      nesting_depth = 3;
    }
    if (oslice->size == 0) {
      if (nesting_depth == 2) {
        syntax_error(info, "Unexpected space between opening and closing parentheses.");
      }
      else {
        syntax_error(info, "Expected loneof field name.");
      }
      return false;
    }
  }
  *ret_nesting_depth = nesting_depth;
  return true;
}

static
  FildeshSxpbIT
insert_next_FildeshSxpb(
    FildeshSxpb* sxpb,
    FildeshSxpbIT p_it,
    FildeshSxprotoFieldKind kind,
    const FildeshO* text_oslice,
    const FildeshSxpbInfo* info)
{
  const FildeshX text_slice = getslice_FildeshO(text_oslice);
  FildeshSxprotoValue* const m = &(*sxpb->values)[p_it.cons_id];
  const FildeshSxprotoFieldKind cons_kind = m->field_kind;
  const char* text;
  FildeshSxpbIT m_it = DEFAULT_FildeshSxpbIT;

  if (cons_kind == FildeshSxprotoFieldKind_ARRAY &&
      text_slice.size > 0 &&
      (kind == FildeshSxprotoFieldKind_UNKNOWN ||
       kind == FildeshSxprotoFieldKind_MESSAGE ||
       kind == FildeshSxprotoFieldKind_DICT ||
       kind == FildeshSxprotoFieldKind_LITERAL ||
       kind == FildeshSxprotoFieldKind_LONEOF ||
       kind == FildeshSxprotoFieldKind_ARRAY ||
       kind == FildeshSxprotoFieldKind_MANYOF))
  {
    syntax_error(info, "Array cannot hold fields.");
    return FildeshSxpbIT_of_NULL();
  }

  text = ensure_name_FildeshSxpb(sxpb, text_slice.at, text_slice.size);
  m_it.cons_id = p_it.cons_id;

  if (cons_kind == FildeshSxprotoFieldKind_MESSAGE ||
      cons_kind == FildeshSxprotoFieldKind_DICT) {
    FildeshSxpbIT e_it = direct_ensure_subfield_FildeshSxpb(
        sxpb, m_it, text, text_slice.size);
    if (e_it.field_kind == kind && kind == FildeshSxprotoFieldKind_ARRAY) {
      return e_it;
    }
    if (e_it.field_kind == kind && kind == FildeshSxprotoFieldKind_MANYOF) {
      return e_it;
    }
    if (e_it.field_kind != FildeshSxprotoFieldKind_UNKNOWN) {
      syntax_error(info, "Duplicate field name. Use array syntax for repeated fields.");
      return FildeshSxpbIT_of_NULL();
    }
    e_it.field_kind = kind;
    (*sxpb->values)[e_it.elem_id].field_kind = kind;
    return e_it;
  }
  if (fildesh_nullid(m->elem)) {
    assert(fildesh_nullid(p_it.elem_id));
    return direct_insert_first_FildeshSxpb(sxpb, m_it, text, kind);
  }
  assert(!fildesh_nullid(p_it.elem_id));
  return direct_insert_next_FildeshSxpb(sxpb, p_it, text, kind);
}

static FildeshSxpbIT freshtail_FildeshSxpb(
    const FildeshSxpb* sxpb,
    FildeshSxpbIT it)
{
  if (fildesh_nullid(it.elem_id)) {
    it.elem_id = (*sxpb->values)[it.cons_id].elem;
  }
  else {
    it.elem_id = (*sxpb->values)[it.elem_id].next;
  }
  assert(!fildesh_nullid(it.elem_id));
  assert(fildesh_nullid((*sxpb->values)[it.elem_id].next));
  it.field_kind = (*sxpb->values)[it.elem_id].field_kind;
  return it;
}

static
  bool
reconcile_elem_kind_FildeshSxpbInfo(
    FildeshSxpbInfo* info,
    FildeshO* oslice,
    FildeshSxprotoFieldKind* elem_kind,
    FildeshSxprotoFieldKind* tmp_kind,
    bool schema_on)
{
  if (*elem_kind == FildeshSxprotoFieldKind_UNKNOWN) {
    *elem_kind = *tmp_kind;
  }
  else if (*elem_kind == FildeshSxprotoFieldKind_LITERAL_FLOAT &&
           *tmp_kind == FildeshSxprotoFieldKind_LITERAL_INT)
  {
    /* This is fine. */
  }
  else if (!schema_on &&
           *elem_kind == FildeshSxprotoFieldKind_LITERAL_INT &&
           *tmp_kind == FildeshSxprotoFieldKind_LITERAL_FLOAT)
  {
    /* Upgrade type. */
    *elem_kind = FildeshSxprotoFieldKind_LITERAL_FLOAT;
  }
  else if (*elem_kind == FildeshSxprotoFieldKind_LITERAL_BOOL &&
           *tmp_kind == FildeshSxprotoFieldKind_LITERAL_INT)
  {
    /* Upgrade type. */
    if (oslice->size == 2 && oslice->at[0] == '+' && oslice->at[1] == '1') {
      truncate_FildeshO(oslice);
      putstrlit_FildeshO(oslice, "+true");
    }
    else if (oslice->size == 2 && oslice->at[0] == '+' && oslice->at[1] == '0') {
      truncate_FildeshO(oslice);
      putstrlit_FildeshO(oslice, "+false");
    }
    else {
      syntax_error(info, "Expected a bool, not an int.");
      return false;
    }
    *tmp_kind = FildeshSxprotoFieldKind_LITERAL_BOOL;
  }
  else if (*elem_kind != *tmp_kind) {
    syntax_error(info, "Unexpected literal type.");
    return false;
  }
  return true;
}

static
  FildeshSxprotoValue*
literal_value_at_FildeshSxpb(FildeshSxpb* sxpb, FildeshSxpbIT it)
{
  FildeshSxprotoValue* e = &(*sxpb->values)[it.elem_id];
  assert(e->field_kind == FildeshSxprotoFieldKind_LITERAL);
  assert(!fildesh_nullid(e->elem));
  e = &(*sxpb->values)[e->elem];
  assert(e->field_kind == FildeshSxprotoFieldKind_LITERAL_STRING ||
         e->field_kind == FildeshSxprotoFieldKind_LITERAL_BOOL ||
         e->field_kind == FildeshSxprotoFieldKind_LITERAL_INT ||
         e->field_kind == FildeshSxprotoFieldKind_LITERAL_FLOAT);
  return e;
}

static
  bool
reconcile_dict_value_kind_FildeshSxpbInfo(
    FildeshSxpbInfo* info,
    FildeshSxpb* sxpb,
    FildeshSxpbIT it,
    FildeshO* oslice,
    FildeshSxprotoFieldKind* elem_kind,
    bool schema_on)
{
  FildeshSxprotoFieldKind tmp_kind = it.field_kind;
  FildeshSxprotoValue* e = NULL;
  if (tmp_kind == FildeshSxprotoFieldKind_LITERAL) {
    e = literal_value_at_FildeshSxpb(sxpb, it);
    tmp_kind = e->field_kind;
    truncate_FildeshO(oslice);
    putstr_FildeshO(oslice, e->text);
  }
  if (!reconcile_elem_kind_FildeshSxpbInfo(
          info, oslice, elem_kind, &tmp_kind, schema_on)) {
    return false;
  }
  if (e && *elem_kind == FildeshSxprotoFieldKind_LITERAL_FLOAT &&
      tmp_kind == FildeshSxprotoFieldKind_LITERAL_INT) {
    tmp_kind = FildeshSxprotoFieldKind_LITERAL_FLOAT;
  }
  if (e && e->field_kind != tmp_kind) {
    const FildeshX xslice = getslice_FildeshO(oslice);
    e->field_kind = tmp_kind;
    e->text = ensure_name_FildeshSxpb(sxpb, xslice.at, xslice.size);
  }
  return true;
}

  bool
parse_field_FildeshSxpbInfo(
    FildeshSxpbInfo* info,
    const FildeshSxprotoField* schema,
    FildeshX* in,
    FildeshSxpb* sxpb,
    FildeshSxpbIT p_it,
    FildeshO* oslice)
{
  unsigned nesting_depth = 0;
  const FildeshSxprotoField* field = NULL;
  FildeshSxprotoFieldKind field_kind = FildeshSxprotoFieldKind_UNKNOWN;
  FildeshSxprotoFieldKind elem_kind = FildeshSxprotoFieldKind_UNKNOWN;
  FildeshSxprotoFieldKind parent_kind = FildeshSxprotoFieldKind_UNKNOWN;
  bool loneof_value_on = false;
  truncate_FildeshO(oslice);

  assert(in->off < in->size && in->at[in->off] == '(');
  skipstr_FildeshSxpbInfo(info, in, "(");

  skip_separation(in, info);
  if (!parse_name_FildeshSxpbInfo(info, in, oslice, &nesting_depth)) {
    return false;
  }
  skip_separation(in, info);
  parent_kind = (*sxpb->values)[p_it.cons_id].field_kind;

  if (nesting_depth == 0) {
    if (oslice->size == 0) {
      field_kind = FildeshSxprotoFieldKind_MESSAGE;
      field = schema;
    }
    else if (parent_kind == FildeshSxprotoFieldKind_NEST) {
      /* Fields within NEST are always NEST kind, even with "" prefix. */
      field_kind = FildeshSxprotoFieldKind_NEST;
    }
    else if (peek_bytestring_FildeshX(in, fildesh_bytestrlit("\"\""))) {
      field_kind = FildeshSxprotoFieldKind_LITERAL;
    }
    else if (peek_chars_FildeshX(in, "()")) {
      field_kind = FildeshSxprotoFieldKind_MESSAGE;
    }
    else {
      field_kind = FildeshSxprotoFieldKind_LITERAL;
    }
  }
  else if (nesting_depth == 1) {
    field_kind = FildeshSxprotoFieldKind_ARRAY;
  }
  else if (nesting_depth == 2) {
    field_kind = FildeshSxprotoFieldKind_MANYOF;
  }
  else if (nesting_depth == 4) {
    field_kind = FildeshSxprotoFieldKind_NEST;
  }
  else if (nesting_depth == 5) {
    field_kind = FildeshSxprotoFieldKind_DICT;
  }
  else {
    assert(nesting_depth == 3);
    field_kind = FildeshSxprotoFieldKind_LONEOF;
    if (schema) {
      assert(!field);
      putc_FildeshO(oslice, '\0');
      oslice->size -= 1;
      field = subfield_of_FildeshSxprotoField(schema, oslice->at);
      if (!field) {
        syntax_error(info, "Unrecognized loneof name.");
        return false;
      }
      schema = field;
      field = NULL;
    }
    p_it = insert_next_FildeshSxpb(sxpb, p_it, field_kind, oslice, info);
    if (nullish_FildeshSxpbIT(p_it)) {
      return false;
    }
    loneof_value_on = true;
    p_it.cons_id = p_it.elem_id;
    p_it.elem_id = ~(FildeshSxpb_id)0;
    if (!parse_name_FildeshSxpbInfo(info, in, oslice, &nesting_depth)) {
      return false;
    }
    skip_separation(in, info);
    if (nesting_depth == 1) {
      field_kind = FildeshSxprotoFieldKind_ARRAY;
    }
    else if (nesting_depth == 5) {
      field_kind = FildeshSxprotoFieldKind_DICT;
    }
    else if (peek_chars_FildeshX(in, "()")) {
      field_kind = FildeshSxprotoFieldKind_MESSAGE;
    }
    else {
      field_kind = FildeshSxprotoFieldKind_LITERAL;
    }
  }
  assert(field_kind != FildeshSxprotoFieldKind_UNKNOWN);

  if ((*sxpb->values)[p_it.cons_id].field_kind == FildeshSxprotoFieldKind_NEST &&
      field_kind != FildeshSxprotoFieldKind_NEST) {
    syntax_error(info, "Nest can only hold nests and strings.");
    return false;
  }
  if (parent_kind == FildeshSxprotoFieldKind_DICT &&
      !loneof_value_on &&
      is_protobuf_repeated_FildeshSxprotoFieldKind(field_kind)) {
    syntax_error(info, "Dict cannot hold list-like or dict values.");
    return false;
  }

  if (schema && !field) {
    if (parent_kind == FildeshSxprotoFieldKind_DICT &&
        schema->kind == FildeshSxprotoFieldKind_DICT) {
      field = schema;
    }
  }
  if (schema && !field) {
    putc_FildeshO(oslice, '\0');
    field = subfield_of_FildeshSxprotoField(schema, oslice->at);
    if (!field) {
      syntax_error(info, "Unrecognized field name.");
      return false;
    }
    truncate_FildeshO(oslice);
    putstr_FildeshO(oslice, field->name);
  }

  if (schema) {
    if (field->kind == FildeshSxprotoFieldKind_MESSAGE) {
      if (field_kind == FildeshSxprotoFieldKind_MANYOF) {
        field_kind = FildeshSxprotoFieldKind_MESSAGE;
      }
      if (field_kind != FildeshSxprotoFieldKind_MESSAGE) {
        syntax_error(info, "Expected field to be a message.");
        return false;
      }
    }
    else if (field->kind == FildeshSxprotoFieldKind_DICT) {
      if (parent_kind == FildeshSxprotoFieldKind_DICT) {
        if (field->subfields) {
          if (field_kind != FildeshSxprotoFieldKind_MESSAGE) {
            syntax_error(info, "Expected dict value to be a message.");
            return false;
          }
        }
        else {
          elem_kind = (FildeshSxprotoFieldKind)field->hi;
          if (elem_kind != FildeshSxprotoFieldKind_UNKNOWN &&
              field_kind != FildeshSxprotoFieldKind_LITERAL) {
            syntax_error(info, "Expected dict value to be a literal.");
            return false;
          }
        }
      }
      else if (field_kind != FildeshSxprotoFieldKind_DICT) {
        syntax_error(info, "Expected field to be a dict.");
        return false;
      }
      else if (!field->subfields && field->hi == 0) {
        field = NULL;
      }
    }
    else if (field == schema && field->kind == FildeshSxprotoFieldKind_ARRAY) {
      if (field_kind != FildeshSxprotoFieldKind_MESSAGE) {
        syntax_error(info, "Expected array element to be a message.");
        return false;
      }
    }
    else if (field->kind == FildeshSxprotoFieldKind_ARRAY) {
      if (field_kind == FildeshSxprotoFieldKind_MANYOF) {
        field_kind = FildeshSxprotoFieldKind_ARRAY;
      }
      if (field_kind != FildeshSxprotoFieldKind_ARRAY) {
        syntax_error(info, "Expected field to be an array.");
        return false;
      }
      if (field->subfields) {
        elem_kind = FildeshSxprotoFieldKind_MESSAGE;
      }
      else {
        elem_kind = (FildeshSxprotoFieldKind)field->hi;
      }
    }
    else if (field->kind == FildeshSxprotoFieldKind_NEST) {
      if (field_kind != FildeshSxprotoFieldKind_NEST) {
        syntax_error(info, "Expected field to be a nest.");
        return false;
      }
      field = NULL;
    }
    else if (field == schema && field->kind == FildeshSxprotoFieldKind_MANYOF) {
      if (field_kind != FildeshSxprotoFieldKind_MESSAGE) {
        syntax_error(info, "Expected manyof element to be a message.");
        return false;
      }
    }
    else if (field->kind == FildeshSxprotoFieldKind_MANYOF) {
      field_kind = FildeshSxprotoFieldKind_MANYOF;
    }
    else {
      if (field_kind == FildeshSxprotoFieldKind_MANYOF) {
        field_kind = FildeshSxprotoFieldKind_LITERAL;
      }
      if (field_kind != FildeshSxprotoFieldKind_LITERAL) {
        syntax_error(info, "Expected field to be a literal.");
        return false;
      }
      elem_kind = field->kind;
    }
  }

  p_it = insert_next_FildeshSxpb(sxpb, p_it, field_kind, oslice, info);
  if (nullish_FildeshSxpbIT(p_it)) {
    return false;
  }
  p_it.cons_id = p_it.elem_id;
  p_it.elem_id = ~(FildeshSxpb_id)0;

  /* Prepare to append more elements if field already a non-empty array.*/
  if (field_kind == FildeshSxprotoFieldKind_ARRAY ||
      field_kind == FildeshSxprotoFieldKind_MANYOF)
  {
    FildeshSxpbIT e_it;
    for (e_it = first_at_FildeshSxpb(sxpb, p_it);
         !nullish_FildeshSxpbIT(e_it);
         e_it = next_at_FildeshSxpb(sxpb, p_it))
    {
      p_it = e_it;
    }
  }

  if (!parse_field_content_FildeshSxpbInfo(
          info, in, sxpb, p_it, field, elem_kind,
          field_kind == FildeshSxprotoFieldKind_NEST && nesting_depth == 4,
          oslice)) {
    return false;
  }

  if (avail_FildeshX(in)) {
    if (skipstr_FildeshSxpbInfo(info, in, ")")) {
      return true;
    }
  }
  syntax_error(info, "Expected a literal or closing paren.");
  return false;
}

  bool
parse_field_content_FildeshSxpbInfo(
    FildeshSxpbInfo* info,
    FildeshX* in,
    FildeshSxpb* sxpb,
    FildeshSxpbIT p_it,
    const FildeshSxprotoField* schema,
    FildeshSxprotoFieldKind elem_kind,
    bool nest_discriminator_on,
    FildeshO* oslice)
{
  const FildeshSxprotoFieldKind field_kind = p_it.field_kind;
  size_t elem_count = 0;
  bool in_avail;
  if (field_kind == FildeshSxprotoFieldKind_NEST &&
      !nest_discriminator_on &&
      peek_bytestring_FildeshX(in, fildesh_bytestrlit("\"\" "))) {
    /* Consume "" discriminator, then parse the content that follows
     * as one concatenated LITERAL_STRING in the nest.
     */
    skipstr_FildeshSxpbInfo(info, in, "\"\"");
    skip_separation(in, info);
    if (!avail_FildeshX(in) || peek_char_FildeshX(in, ')')) {
      truncate_FildeshO(oslice);
    }
    else if (!parse_string_field_content_FildeshSxpbInfo(info, in, oslice)) {
      return false;
    }
    p_it = insert_next_FildeshSxpb(
        sxpb, p_it, FildeshSxprotoFieldKind_LITERAL_STRING, oslice, info);
    assert(!fildesh_nullid(p_it.elem_id));
    elem_count += 1;
    skip_separation(in, info);
  }
  for (in_avail = peek_bytestring_FildeshX(in, NULL, 2);
       in_avail && in->at[in->off] != ')';
       in_avail = peek_bytestring_FildeshX(in, NULL, 2))
  {
    const char c0 = in->at[in->off];
    const char c1 = in->at[in->off+1];
    if (elem_count > 0 && field_kind == FildeshSxprotoFieldKind_LITERAL) {
      syntax_error(info, "Literal field can only hold 1 value.");
      return false;
    }
    if (c0 == '(') {
      bool is_anonymous_discriminated_string_element = false;
      assert(field_kind != FildeshSxprotoFieldKind_LITERAL);
      if (field_kind == FildeshSxprotoFieldKind_ARRAY) {
        if (peek_bytestring_FildeshX(in, fildesh_bytestrlit("(())"))) {
          syntax_error(info, "Arrays cannot be nested.");
          return false;
        }
        if (peek_bytestring_FildeshX(in, fildesh_bytestrlit("(\"\" "))) {
          is_anonymous_discriminated_string_element = true;
          if (elem_kind == FildeshSxprotoFieldKind_UNKNOWN) {
            elem_kind = FildeshSxprotoFieldKind_LITERAL_STRING;
          }
          else if (elem_kind != FildeshSxprotoFieldKind_LITERAL_STRING) {
            syntax_error(info, "Unexpected string array element.");
            return false;
          }
        }
        else if (elem_kind == FildeshSxprotoFieldKind_UNKNOWN) {
          elem_kind = FildeshSxprotoFieldKind_MESSAGE;
        }
        else if (elem_kind != FildeshSxprotoFieldKind_MESSAGE) {
          syntax_error(info, "Unexpected message array element.");
          return false;
        }
      }
      else if (field_kind == FildeshSxprotoFieldKind_NEST) {
        if (peek_bytestring_FildeshX(in, fildesh_bytestrlit("(\"\" "))) {
          /* Check if content after anonymous name is a nest discriminator.
           * By itself, ("") means anonymous nest, not anonymous string.
           */
          if (!peek_bytestring_FildeshX(in, fildesh_bytestrlit("(\"\" (\"\")"))) {
            is_anonymous_discriminated_string_element = true;
            elem_kind = FildeshSxprotoFieldKind_LITERAL_STRING;
          }
        }
      }

      if (is_anonymous_discriminated_string_element) {
        skipstr_FildeshSxpbInfo(info, in, "(");
        if (!parse_string_field_content_FildeshSxpbInfo(info, in, oslice)) {
          return false;
        }
        if (!skipstr_FildeshSxpbInfo(info, in, ")")) {
          syntax_error(info, "Expected closing paren.");
          return false;
        }
        p_it = insert_next_FildeshSxpb(sxpb, p_it, elem_kind, oslice, info);
        assert(!fildesh_nullid(p_it.elem_id));
      }
      else {
        if (!parse_field_FildeshSxpbInfo(info, schema, in, sxpb, p_it, oslice)) {
          return false;
        }
        if (field_kind != FildeshSxprotoFieldKind_MESSAGE) {
          p_it = freshtail_FildeshSxpb(sxpb, p_it);
        }
        if (field_kind == FildeshSxprotoFieldKind_DICT) {
          if (!reconcile_dict_value_kind_FildeshSxpbInfo(
                  info, sxpb, p_it, oslice, &elem_kind, schema != NULL)) {
            return false;
          }
        }
      }
    }
    else {
      FildeshSxprotoFieldKind tmp_kind = FildeshSxprotoFieldKind_UNKNOWN;
      if (field_kind == FildeshSxprotoFieldKind_MESSAGE) {
        syntax_error(info, "Message can only hold fields.");
        return false;
      }
      if (field_kind == FildeshSxprotoFieldKind_DICT) {
        syntax_error(info, "Dict items must be key-value pairs even when the value is empty.");
        return false;
      }
      if (elem_kind == FildeshSxprotoFieldKind_LITERAL_STRING ||
          field_kind == FildeshSxprotoFieldKind_NEST ||
          (c0 != '+' && c0 != '-' && c0 != '.' && !('0' <= c0 && c0 <= '9')) ||
          (c0 == '-' && c1 != '.' && c1 != '+' && !('0' <= c1 && c1 <= '9')) ||
          (c0 == '.' && c1 != '-' && c1 != '+' && !('0' <= c1 && c1 <= '9')))
      {
        tmp_kind = FildeshSxprotoFieldKind_LITERAL_STRING;
        if (field_kind == FildeshSxprotoFieldKind_LITERAL) {
          if (!parse_string_field_content_FildeshSxpbInfo(info, in, oslice)) {
            return false;
          }
        }
        else {
          truncate_FildeshO(oslice);
          if (!parse_concat_string_FildeshSxpbInfo(info, in, oslice)) {
            return false;
          }
        }
      }
      else if (('0' <= c0 && c0 <= '9') ||  c1 == '.' ||
               ('0' <= c1 && c1 <= '9'))
      {
        if (!parse_number_FildeshSxpbInfo(info, in, oslice)) {
          return false;
        }
        tmp_kind = FildeshSxprotoFieldKind_LITERAL_INT;
        if (oslice->size > 2 && oslice->at[2] == '.') {
          tmp_kind = FildeshSxprotoFieldKind_LITERAL_FLOAT;
        }
      }
      else if (skipstr_FildeshSxpbInfo(info, in, "+true")) {
        truncate_FildeshO(oslice);
        putstrlit_FildeshO(oslice, "+true");
        tmp_kind = FildeshSxprotoFieldKind_LITERAL_BOOL;
      }
      else if (skipstr_FildeshSxpbInfo(info, in, "+false")) {
        truncate_FildeshO(oslice);
        putstrlit_FildeshO(oslice, "+false");
        tmp_kind = FildeshSxprotoFieldKind_LITERAL_BOOL;
      }
      else {
        parse_number_FildeshSxpbInfo(info, in, oslice);
        return false;
      }

      if (!reconcile_elem_kind_FildeshSxpbInfo(
              info, oslice, &elem_kind, &tmp_kind, schema != NULL)) {
        return false;
      }

      p_it = insert_next_FildeshSxpb(sxpb, p_it, elem_kind, oslice, info);
      assert(!fildesh_nullid(p_it.elem_id));
    }
    elem_count += 1;
    skip_separation(in, info);
  }
  return true;
}

  FildeshSxpb*
slurp_sxpb_close_FildeshX(
    FildeshX* in,
    const FildeshSxprotoField* schema,
    FildeshO* err_out)
{
  FildeshSxpbInfo info[1] = {DEFAULT_FildeshSxpbInfo};
  FildeshSxpb* sxpb = open_FildeshSxpb();
  FildeshSxpbIT p_it = top_of_FildeshSxpb(sxpb);
  FildeshO oslice[1] = {DEFAULT_FildeshO};
  bool nest_discriminator_on = false;

  info->err_out = err_out;
  skip_separation(in, info);
  if (skipstr_FildeshSxpbInfo(info, in, "(())")) {
    skip_separation(in, info);
    if (on_array_content(in)) {
      p_it.field_kind = FildeshSxprotoFieldKind_ARRAY;
    }
    else {
      p_it.field_kind = FildeshSxprotoFieldKind_MANYOF;
    }
    (*sxpb->values)[p_it.cons_id].field_kind = p_it.field_kind;
  }
  else if (skipstr_FildeshSxpbInfo(info, in, "()")) {
    skip_separation(in, info);
    p_it.field_kind = FildeshSxprotoFieldKind_DICT;
    (*sxpb->values)[p_it.cons_id].field_kind = p_it.field_kind;
    schema = NULL;
  }
  else if (skipstr_FildeshSxpbInfo(info, in, "(\"\")")) {
    skip_separation(in, info);
    p_it.field_kind = FildeshSxprotoFieldKind_NEST;
    (*sxpb->values)[p_it.cons_id].field_kind = p_it.field_kind;
    nest_discriminator_on = true;
  }

  if (!parse_field_content_FildeshSxpbInfo(
          info, in, sxpb, p_it, schema,
          FildeshSxprotoFieldKind_UNKNOWN,
          nest_discriminator_on,
          oslice))
  {
    close_FildeshSxpb(sxpb);
    sxpb = NULL;
  }

  if (sxpb && avail_FildeshX(in)) {
    syntax_error(info, "Expected open paren to start field.");
    close_FildeshSxpb(sxpb);
    sxpb = NULL;
  }

  close_FildeshX(in);
  close_FildeshO(oslice);
  return sxpb;
}

