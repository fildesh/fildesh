#include "src/sxproto/parse_sxpb.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>


static const char sxpb_delim_bytes[] = " \n\t\r\v\"();";
static const char sxpb_blank_bytes[] = " \n\t\r\v";

static void
syntax_error(const FildeshSxpbInfo* info, const char* msg)
{
  FildeshO* const out = info->err_out;
  if (!out) {return;}
  putstr_FildeshO(out, "Line ");
  print_int_FildeshO(out, (int)(info->line_count+1));
  putstr_FildeshO(out, " column ");
  print_int_FildeshO(out, (int)(info->column_count+1));
  putstr_FildeshO(out, ": ");
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

  bool
parse_string_FildeshSxpbInfo(
    FildeshSxpbInfo* info,
    FildeshX* in,
    FildeshO* oslice)
{
  FildeshX slice;

  truncate_FildeshO(oslice);
  assert(in->off < in->size && in->at[in->off] == '"');
  skipstr_FildeshSxpbInfo(info, in, "\"");

  for (slice = until_chars_FildeshSxpbInfo(info, in, "\\\"");
       in->off < in->size && in->at[in->off] != '"';
       slice = until_chars_FildeshSxpbInfo(info, in, "\\\""))
  {
    putslice_FildeshO(oslice, slice);
    if (skipstr_FildeshSxpbInfo(info, in, "\\\"")) {
      putc_FildeshO(oslice, '"');
    }
    else if (skipstr_FildeshSxpbInfo(info, in, "\\\\")) {
      putc_FildeshO(oslice, '\\');
    }
    else if (skipstr_FildeshSxpbInfo(info, in, "\\n")) {
      putc_FildeshO(oslice, '\n');
    }
    else {
      syntax_error(info, "Unknown escape sequence. Only very basic ones are supported.");
      return false;
    }
  }
  putslice_FildeshO(oslice, slice);

  if (!skipstr_FildeshSxpbInfo(info, in, "\"")) {
    truncate_FildeshO(oslice);
    syntax_error(info, "Expected closing double quote.");
    return false;
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

  bool
parse_name_FildeshSxpbInfo(
    FildeshSxpbInfo* info,
    FildeshX* in,
    FildeshO* oslice,
    unsigned* ret_nesting_depth)
{
  FildeshX slice;
  unsigned nesting_depth = 0;
  if (skipstr_FildeshSxpbInfo(info, in, "(")) {
    nesting_depth = 1;
    skip_separation(in, info);
    if (skipstr_FildeshSxpbInfo(info, in, "(")) {
      nesting_depth = 2;
      skip_separation(in, info);
    }
  }
  slice = until_chars_FildeshSxpbInfo(info, in, sxpb_delim_bytes);
  truncate_FildeshO(oslice);
  if (slice.size == 0) {
    if (peek_char_FildeshX(in, '"')) {
      if (!parse_string_FildeshSxpbInfo(info, in, oslice)) {
        return false;
      }
      info->quoted_names_on = true;
    }
  }
  else if (info->quoted_names_on) {
    syntax_error(info, "Expected subfield name to be quoted too.");
    return false;
  }
  else {
    putslice_FildeshO(oslice, slice);
  }

  if (nesting_depth > 0) {
    skip_separation(in, info);
    if (skipstr_FildeshSxpbInfo(info, in, ")")) {
      if (nesting_depth > 1) {
        skip_separation(in, info);
        if (!skipstr_FildeshSxpbInfo(info, in, ")")) {
          syntax_error(info, "Expected 2 closing parens after manyof name; got 1.");
          return false;
        }
      }
    }
    else {
      if (nesting_depth == 1) {
        syntax_error(info, "Expected closing paren after array name.");
        return false;
      }
      else {
        syntax_error(info, "Expected 2 closing parens after manyof name; got 0.");
        return false;
      }
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

  if (cons_kind == FildeshSxprotoFieldKind_MESSAGE && text_slice.size == 0) {
    syntax_error(info, "Message expects named fields inside it.");
    return FildeshSxpbIT_of_NULL();
  }
  if (cons_kind == FildeshSxprotoFieldKind_ARRAY &&
      text_slice.size > 0 &&
      (kind == FildeshSxprotoFieldKind_UNKNOWN ||
       kind == FildeshSxprotoFieldKind_MESSAGE ||
       kind == FildeshSxprotoFieldKind_LITERAL ||
       kind == FildeshSxprotoFieldKind_ARRAY ||
       kind == FildeshSxprotoFieldKind_MANYOF))
  {
    syntax_error(info, "Array cannot hold fields.");
    return FildeshSxpbIT_of_NULL();
  }
  if (cons_kind == FildeshSxprotoFieldKind_MANYOF && text_slice.size == 0) {
    syntax_error(info, "Manyof cannot hold nameless message values yet.");
    return FildeshSxpbIT_of_NULL();
  }

  text = ensure_name_FildeshSxpb(sxpb, text_slice.at, text_slice.size);
  m_it.cons_id = p_it.cons_id;

  if (cons_kind == FildeshSxprotoFieldKind_MESSAGE) {
    FildeshSxpbIT e_it = direct_ensure_subfield_FildeshSxpb(
        sxpb, m_it, text, text_slice.size);
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
  return it;
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
  const bool info_quoted_names_on = info->quoted_names_on;
  unsigned nesting_depth = 0;
  size_t elem_count = 0;
  const FildeshSxprotoField* field = NULL;
  FildeshSxprotoFieldKind field_kind = FildeshSxprotoFieldKind_UNKNOWN;
  FildeshSxprotoFieldKind elem_kind = FildeshSxprotoFieldKind_UNKNOWN;
  truncate_FildeshO(oslice);

  assert(in->off < in->size && in->at[in->off] == '(');
  skipstr_FildeshSxpbInfo(info, in, "(");

  skip_separation(in, info);
  if (!parse_name_FildeshSxpbInfo(info, in, oslice, &nesting_depth)) {
    return false;
  }
  skip_separation(in, info);

  if (nesting_depth == 0) {
    if (oslice->size == 0) {
      field_kind = FildeshSxprotoFieldKind_MESSAGE;
      field = schema;
    }
    else if (peek_chars_FildeshX(in, "()")) {
      field_kind = FildeshSxprotoFieldKind_MESSAGE;
    }
    else {
      field_kind = FildeshSxprotoFieldKind_LITERAL;
    }
  }
  else if (nesting_depth == 1) {
    if (oslice->size == 0) {
      if (peek_chars_FildeshX(in, ")")) {
        syntax_error(info, "Denote empty message in array as (), not (()).");
        return false;
      }
      field_kind = FildeshSxprotoFieldKind_MESSAGE;
      field = schema;
    }
    else {
      field_kind = FildeshSxprotoFieldKind_ARRAY;
    }
  }
  else {
    assert(nesting_depth == 2);
    field_kind = FildeshSxprotoFieldKind_MANYOF;
  }
  assert(field_kind != FildeshSxprotoFieldKind_UNKNOWN);

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

  p_it = insert_next_FildeshSxpb(sxpb, p_it, field_kind, oslice, info);
  if (nullish_FildeshSxpbIT(p_it)) {
    return false;
  }
  p_it.cons_id = p_it.elem_id;
  p_it.elem_id = ~(FildeshSxpb_id)0;

  if (schema) {
    if (field->kind == FildeshSxprotoFieldKind_MESSAGE) {
      if (field_kind != FildeshSxprotoFieldKind_MESSAGE) {
        syntax_error(info, "Expected field to be a message.");
        return false;
      }
    }
    else if (field->kind == FildeshSxprotoFieldKind_ARRAY) {
      if (field != schema) {
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
      else {
        if (field_kind != FildeshSxprotoFieldKind_MESSAGE) {
          syntax_error(info, "Expected array element to be a message.");
          return false;
        }
      }
    }
    else if (field->kind == FildeshSxprotoFieldKind_MANYOF) {
      if (field_kind != FildeshSxprotoFieldKind_MANYOF) {
        syntax_error(info, "Expected field to be a manyof.");
        return false;
      }
    }
    else {
      if (field_kind != FildeshSxprotoFieldKind_LITERAL) {
        syntax_error(info, "Expected field to be a literal.");
        return false;
      }
      elem_kind = field->kind;
    }
  }

  while (!skipstr_FildeshSxpbInfo(info, in, ")")) {
    if (elem_count > 0 && field_kind == FildeshSxprotoFieldKind_LITERAL) {
      syntax_error(info, "Literal field can only hold 1 value.");
      return false;
    }
    if (peek_char_FildeshX(in, '(')) {
      assert(field_kind != FildeshSxprotoFieldKind_LITERAL);
      if (field_kind == FildeshSxprotoFieldKind_ARRAY) {
        if (elem_kind == FildeshSxprotoFieldKind_UNKNOWN) {
          elem_kind = FildeshSxprotoFieldKind_MESSAGE;
        }
        else if (elem_kind != FildeshSxprotoFieldKind_MESSAGE) {
          syntax_error(info, "Unexpected message.");
          return false;
        }
      }
      if (!parse_field_FildeshSxpbInfo(info, field, in, sxpb, p_it, oslice)) {
        return false;
      }
      if (field_kind != FildeshSxprotoFieldKind_MESSAGE) {
        p_it = freshtail_FildeshSxpb(sxpb, p_it);
        assert((*sxpb->values)[p_it.elem_id].field_kind != FildeshSxprotoFieldKind_ARRAY);
      }
    }
    else {
      FildeshSxprotoFieldKind tmp_kind = FildeshSxprotoFieldKind_UNKNOWN;
      if (field_kind == FildeshSxprotoFieldKind_MESSAGE) {
        syntax_error(info, "Message can only hold fields.");
        return false;
      }
      if (peek_char_FildeshX(in, '"')) {
        if (!parse_string_FildeshSxpbInfo(info, in, oslice)) {
          return false;
        }
        tmp_kind = FildeshSxprotoFieldKind_LITERAL_STRING;
      }
      else if (skipstr_FildeshX(in, "true")) {
        truncate_FildeshO(oslice);
        putstr_FildeshO(oslice, "true");
        tmp_kind = FildeshSxprotoFieldKind_LITERAL_BOOL;
      }
      else if (skipstr_FildeshX(in, "false")) {
        truncate_FildeshO(oslice);
        putstr_FildeshO(oslice, "false");
        tmp_kind = FildeshSxprotoFieldKind_LITERAL_BOOL;
      }
      else if (peek_chars_FildeshX(in, "0123456789+-.")) {
        if (!parse_number_FildeshSxpbInfo(info, in, oslice)) {
          return false;
        }
        tmp_kind = FildeshSxprotoFieldKind_LITERAL_INT;
        if (oslice->size > 2 && oslice->at[2] == '.') {
          tmp_kind = FildeshSxprotoFieldKind_LITERAL_FLOAT;
        }
      }
      else {
        syntax_error(info, "Expected a literal or closing paren.");
        return false;
      }

      if (elem_kind == FildeshSxprotoFieldKind_UNKNOWN) {
        elem_kind = tmp_kind;
      }
      else if (elem_kind == FildeshSxprotoFieldKind_LITERAL_FLOAT &&
               tmp_kind == FildeshSxprotoFieldKind_LITERAL_INT)
      {
        /* This is fine.*/
      }
      else if (!schema &&
               elem_kind == FildeshSxprotoFieldKind_LITERAL_INT &&
               tmp_kind == FildeshSxprotoFieldKind_LITERAL_FLOAT)
      {
        /* Upgrade type.*/
        elem_kind = FildeshSxprotoFieldKind_LITERAL_FLOAT;
      }
      else if (elem_kind == FildeshSxprotoFieldKind_LITERAL_BOOL &&
               tmp_kind == FildeshSxprotoFieldKind_LITERAL_INT)
      {
        /* Upgrade type.*/
        if (oslice->size == 2 && oslice->at[0] == '+' && oslice->at[1] == '1') {
          truncate_FildeshO(oslice);
          putstr_FildeshO(oslice, "true");
        }
        else if (oslice->size == 2 && oslice->at[0] == '+' && oslice->at[1] == '0') {
          truncate_FildeshO(oslice);
          putstr_FildeshO(oslice, "false");
        }
        else {
          syntax_error(info, "Expected a bool, not an int.");
          return false;
        }
      }
      else if (elem_kind != tmp_kind) {
        syntax_error(info, "Unexpected literal type.");
        return false;
      }

      p_it = insert_next_FildeshSxpb(sxpb, p_it, elem_kind, oslice, info);
      assert(!fildesh_nullid(p_it.elem_id));
    }
    elem_count += 1;
    skip_separation(in, info);
  }

  info->quoted_names_on = info_quoted_names_on;
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

  info->err_out = err_out;

  for (skip_separation(in, info);
       peek_char_FildeshX(in, '(');
       skip_separation(in, info))
  {
    if (!parse_field_FildeshSxpbInfo(info, schema, in, sxpb, p_it, oslice)) {
      close_FildeshSxpb(sxpb);
      sxpb = NULL;
      break;
    }
  }

  if (sxpb) {
    FildeshX slice = until_char_FildeshX(in, '\n');
    if (slice.size > 0) {
      syntax_error(info, "Expected open paren to start field.");
      close_FildeshSxpb(sxpb);
      sxpb = NULL;
    }
  }

  close_FildeshX(in);
  close_FildeshO(oslice);
  return sxpb;
}

