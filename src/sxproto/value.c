#include "src/sxproto/value.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

static
  const FildeshSxprotoValue*
value_at_FildeshSxpb(const FildeshSxpb* sxpb, FildeshSxpbIT it)
{
  const FildeshSxprotoValue* const e = (
      fildesh_nullid(it.elem_id)
      ? &(*sxpb->values)[it.cons_id]
      : &(*sxpb->values)[it.elem_id]);
  if (fildesh_nullid(e->elem)) {
    return e;
  }
  assert(e->field_kind == FildeshSxprotoFieldKind_LITERAL);
  return &(*sxpb->values)[e->elem];
}

  bool
bool_value_at_FildeshSxpb(const FildeshSxpb* sxpb, FildeshSxpbIT it)
{
  const FildeshSxprotoValue* const v = value_at_FildeshSxpb(sxpb, it);
  if (v->field_kind == FildeshSxprotoFieldKind_LITERAL_INT) {
    unsigned x = UINT_MAX;
    const char* s = v->text;
    s = fildesh_parse_unsigned(&x, s);
    assert(s);
    assert(x <= 1);
    return (x == 1);
  }
  assert(v->field_kind == FildeshSxprotoFieldKind_LITERAL_BOOL);
  return v->text[0] == 't';
}

  unsigned
unsigned_value_at_FildeshSxpb(const FildeshSxpb* sxpb, FildeshSxpbIT it)
{
  const FildeshSxprotoValue* const v = value_at_FildeshSxpb(sxpb, it);
  unsigned x = UINT_MAX;
  const char* s = v->text;
  assert(v->field_kind == FildeshSxprotoFieldKind_LITERAL_INT);
  s = fildesh_parse_unsigned(&x, s);
  assert(s);
  return x;
}

  float
float_value_at_FildeshSxpb(const FildeshSxpb* sxpb, FildeshSxpbIT it)
{
  const FildeshSxprotoValue* const v = value_at_FildeshSxpb(sxpb, it);
  double x = 0.0;
  const char* s = v->text;
  assert(v->field_kind == FildeshSxprotoFieldKind_LITERAL_FLOAT ||
         v->field_kind == FildeshSxprotoFieldKind_LITERAL_INT);
  s = fildesh_parse_double(&x, s);
  assert(s);
  return (float)x;
}

  const char*
str_value_at_FildeshSxpb(const FildeshSxpb* sxpb, FildeshSxpbIT it)
{
  const FildeshSxprotoValue* const v = value_at_FildeshSxpb(sxpb, it);
  assert(v->field_kind == FildeshSxprotoFieldKind_LITERAL_STRING);
  return v->text;
}

  bool
lone_subfield_at_FildeshSxpb_to_bool(
    bool* dst, const FildeshSxpb* sxpb, FildeshSxpbIT it, const char* name)
{
  it = lookup_subfield_at_FildeshSxpb(sxpb, it, name);
  if (nullish_FildeshSxpbIT(it)) {return false;}
  *dst = bool_value_at_FildeshSxpb(sxpb, it);
  return true;
}
  bool
lone_subfield_at_FildeshSxpb_to_unsigned(
    unsigned* dst, const FildeshSxpb* sxpb, FildeshSxpbIT it, const char* name)
{
  it = lookup_subfield_at_FildeshSxpb(sxpb, it, name);
  if (nullish_FildeshSxpbIT(it)) {return false;}
  *dst = unsigned_value_at_FildeshSxpb(sxpb, it);
  return true;
}
  bool
lone_subfield_at_FildeshSxpb_to_float(
    float* dst, const FildeshSxpb* sxpb, FildeshSxpbIT it, const char* name)
{
  it = lookup_subfield_at_FildeshSxpb(sxpb, it, name);
  if (nullish_FildeshSxpbIT(it)) {return false;}
  *dst = float_value_at_FildeshSxpb(sxpb, it);
  return true;
}
  bool
lone_subfield_at_FildeshSxpb_to_str(
    const char** dst, const FildeshSxpb* sxpb, FildeshSxpbIT it, const char* name)
{
  it = lookup_subfield_at_FildeshSxpb(sxpb, it, name);
  if (nullish_FildeshSxpbIT(it)) {return false;}
  *dst = str_value_at_FildeshSxpb(sxpb, it);
  return true;
}

  FildeshSxpbIT
assign_bool_subfield_at_FildeshSxpb(
    FildeshSxpb* sxpb, FildeshSxpbIT it, const char* k, bool v)
{
  FildeshSxprotoValue* e;
  it = ensure_bool_subfield_at_FildeshSxpb(sxpb, it, k);
  e = &(*sxpb->values)[(*sxpb->values)[it.elem_id].elem];
  assert(e->field_kind == FildeshSxprotoFieldKind_LITERAL_BOOL);
  if (v) {
    e->text = ensure_name_FildeshSxpb(sxpb, "true", 4);
  }
  else {
    e->text = ensure_name_FildeshSxpb(sxpb, "false", 5);
  }
  return it;
}

  FildeshSxpbIT
assign_str_subfield_at_FildeshSxpb(
    FildeshSxpb* sxpb, FildeshSxpbIT it, const char* k, const char* v)
{
  FildeshSxprotoValue* e;
  it = ensure_string_subfield_at_FildeshSxpb(sxpb, it, k);
  e = &(*sxpb->values)[(*sxpb->values)[it.elem_id].elem];
  assert(e->field_kind == FildeshSxprotoFieldKind_LITERAL_STRING);
  e->text = ensure_name_FildeshSxpb(sxpb, v, strlen(v));
  return it;
}

static
  void
direct_assign_literal(
    FildeshSxpb* sxpb, FildeshSxpbIT it, FildeshSxprotoValue* e,
    const FildeshSxpb* src_sxpb, FildeshSxpbIT src_it)
{
  const FildeshSxprotoValue* src_e = &(*src_sxpb->values)[src_it.elem_id];
  const char* v_text;
  if (src_e->field_kind == FildeshSxprotoFieldKind_LITERAL) {
    src_e = &(*src_sxpb->values)[src_e->elem];
  }
  v_text = ensure_name_FildeshSxpb(sxpb, src_e->text, strlen(src_e->text));
  if (e->field_kind == FildeshSxprotoFieldKind_LITERAL) {
    if (fildesh_nullid(e->elem)) {
      direct_insert_first_FildeshSxpb(sxpb, it, v_text, src_e->field_kind);
    }
    else {
      e = &(*sxpb->values)[e->elem];
      assert(e->field_kind == src_e->field_kind);
      e->text = v_text;
    }
  }
  else {
    assert(default_value_text_FildeshSxpb(NULL, e->field_kind));
    e->text = v_text;
  }
}

  void
assign_at_FildeshSxpb(
    FildeshSxpb* sxpb, FildeshSxpbIT it,
    const char* optional_field_name,
    const FildeshSxpb* src_sxpb, FildeshSxpbIT src_it)
{
  FildeshSxprotoValue* e = &(*sxpb->values)[it.elem_id];
  FildeshSxprotoFieldKind kind = (*src_sxpb->values)[src_it.elem_id].field_kind;
  if (optional_field_name) {
    size_t n = strlen(optional_field_name);
    assert(e->field_kind == FildeshSxprotoFieldKind_MESSAGE);
    optional_field_name = ensure_name_FildeshSxpb(sxpb, optional_field_name, n);
    it = direct_ensure_subfield_FildeshSxpb(sxpb, it, optional_field_name, n);
    e = &(*sxpb->values)[it.elem_id];
    e->field_kind = kind;
  }
  if (kind == FildeshSxprotoFieldKind_LITERAL ||
      default_value_text_FildeshSxpb(NULL, kind))
  {
    direct_assign_literal(sxpb, it, e, src_sxpb, src_it);
    return;
  }
  assert(e->field_kind == kind);
  while (!nullish_FildeshSxpbIT(first_at_FildeshSxpb(sxpb, it))) {
    remove_at_FildeshSxpb(sxpb, first_at_FildeshSxpb(sxpb, it));
  }
  for (src_it = first_at_FildeshSxpb(src_sxpb, src_it);
       !nullish_FildeshSxpbIT(src_it);
       src_it = next_at_FildeshSxpb(src_sxpb, src_it))
  {
    optional_field_name = (*src_sxpb->values)[src_it.elem_id].text;
    if (kind == FildeshSxprotoFieldKind_MESSAGE) {
      assign_at_FildeshSxpb(sxpb, it, optional_field_name, src_sxpb, src_it);
    }
    else if (kind == FildeshSxprotoFieldKind_ARRAY) {
      append_at_FildeshSxpb(sxpb, it, NULL, src_sxpb, src_it);
    }
    else {
      append_at_FildeshSxpb(sxpb, it, optional_field_name, src_sxpb, src_it);
    }
  }
}

static
  FildeshSxpbIT
append_element_at_FildeshSxpb(
    FildeshSxpb* sxpb, FildeshSxpbIT it,
    const char* optional_field_name,
    FildeshSxprotoFieldKind kind)
{
  FildeshSxprotoValue* e = &(*sxpb->values)[it.elem_id];
  const char* v_text = default_value_text_FildeshSxpb(sxpb, kind);
  if (optional_field_name) {
    size_t n = strlen(optional_field_name);
    if (e->field_kind == FildeshSxprotoFieldKind_MESSAGE) {
      it = ensure_message_subfield_at_FildeshSxpb(
          sxpb, it, ensure_name_FildeshSxpb(sxpb, optional_field_name, n));
      optional_field_name = NULL;
    }
    else {
      assert(e->field_kind == FildeshSxprotoFieldKind_MANYOF);
      optional_field_name = ensure_name_FildeshSxpb(sxpb, optional_field_name, n);
    }
  }
  else {
    assert(v_text || e->field_kind == FildeshSxprotoFieldKind_ARRAY);
  }
  e = NULL;
  if (nullish_FildeshSxpbIT(first_at_FildeshSxpb(sxpb, it))) {
    if (optional_field_name) {
      if (v_text) {
        it = direct_insert_first_FildeshSxpb(
            sxpb, it, optional_field_name, FildeshSxprotoFieldKind_LITERAL);
        direct_insert_first_FildeshSxpb(sxpb, it, v_text, kind);
        return it;
      }
      v_text = optional_field_name;
    }
    return direct_insert_first_FildeshSxpb(sxpb, it, v_text, kind);
  }
  it = first_at_FildeshSxpb(sxpb, it);
  while (!nullish_FildeshSxpbIT(next_at_FildeshSxpb(sxpb, it))) {
    it = next_at_FildeshSxpb(sxpb, it);
  }
  if (optional_field_name) {
    if (v_text) {
      it = direct_insert_next_FildeshSxpb(
          sxpb, it, optional_field_name, FildeshSxprotoFieldKind_LITERAL);
      direct_insert_first_FildeshSxpb(sxpb, it, v_text, kind);
      return it;
    }
    v_text = optional_field_name;
  }
  return direct_insert_next_FildeshSxpb(sxpb, it, v_text, kind);
}

  FildeshSxpbIT
append_at_FildeshSxpb(
    FildeshSxpb* sxpb, FildeshSxpbIT it,
    const char* optional_field_name,
    const FildeshSxpb* src_sxpb, FildeshSxpbIT src_it)
{
  FildeshSxprotoValue* src_e = &(*src_sxpb->values)[src_it.elem_id];
  FildeshSxprotoFieldKind kind = src_e->field_kind;
  it = append_element_at_FildeshSxpb(sxpb, it, optional_field_name, kind);
  assign_at_FildeshSxpb(sxpb, it, NULL, src_sxpb, src_it);
  return it;
}

  void
print_quoted_sxpb_str_FildeshO(FildeshO* out, const char* s)
{
  size_t i;
  putc_FildeshO(out, '"');
  for (i = 0; s[i] != '\0'; ++i) {
    switch (s[i]) {
      case '"':   putstrlit_FildeshO(out, "\\\"");  break;
      case '\\':  putstrlit_FildeshO(out, "\\\\");  break;
      case '\n':  putstrlit_FildeshO(out, "\\n");  break;
      default:    putc_FildeshO(out, s[i]);  break;
    }
  }
  putc_FildeshO(out, '"');
}

  void
print_sxpb_literal_value_FildeshO(FildeshO* out, const FildeshSxprotoValue* e)
{
  if (e->field_kind == FildeshSxprotoFieldKind_LITERAL_STRING) {
    print_quoted_sxpb_str_FildeshO(out, e->text);
  }
  else if (e->text[0] == '+') {
    putstr_FildeshO(out, &e->text[1]);
  }
  else {
    putstr_FildeshO(out, e->text);
  }
}

  const char*
default_value_text_FildeshSxpb(FildeshSxpb* sxpb, FildeshSxprotoFieldKind kind)
{
  const char* s = NULL;
  unsigned n = 0;
  switch (kind) {
    case FildeshSxprotoFieldKind_LITERAL_BOOL:
      s = "false";  n = 5;  break;
    case FildeshSxprotoFieldKind_LITERAL_INT:
      s = "+0";  n = 2;  break;
    case FildeshSxprotoFieldKind_LITERAL_FLOAT:
      s = "+0.e+0";  n = 6;  break;
    case FildeshSxprotoFieldKind_LITERAL_STRING:
      s = "";  break;
    default:
      break;
  }
  if (s && sxpb) {
    s = ensure_name_FildeshSxpb(sxpb, s, n);
  }
  return s;
}

  FildeshSxpbIT
direct_insert_first_FildeshSxpb(
    FildeshSxpb* sxpb,
    FildeshSxpbIT m_it,
    const char* text,
    FildeshSxprotoFieldKind kind)
{
  FildeshSxprotoValue* const m = (
      fildesh_nullid(m_it.elem_id)
      ? &(*sxpb->values)[m_it.cons_id]
      : &(*sxpb->values)[m_it.elem_id]);
  FildeshSxprotoValue v = DEFAULT_FildeshSxprotoValue;
  FildeshSxpbIT it = DEFAULT_FildeshSxpbIT;
  v.text = text;
  v.next = m->elem;
  v.field_kind = kind;
  it.field_kind = kind;
  it.cons_id = m - (*sxpb->values);
  it.elem_id = count_of_FildeshAT(sxpb->values);
  m->elem = it.elem_id;
  /* Wait until end to modify array to avoid invalidating `m` pointer.*/
  push_FildeshAT(sxpb->values, v);
  return it;
}

  FildeshSxpbIT
direct_insert_next_FildeshSxpb(
    FildeshSxpb* sxpb,
    FildeshSxpbIT p_it,
    const char* text,
    FildeshSxprotoFieldKind kind)
{
  FildeshSxprotoValue* const p = (
      fildesh_nullid(p_it.elem_id)
      ? &(*sxpb->values)[p_it.cons_id]
      : &(*sxpb->values)[p_it.elem_id]);
  FildeshSxprotoValue v = DEFAULT_FildeshSxprotoValue;
  FildeshSxpbIT it = DEFAULT_FildeshSxpbIT;
  v.text = text;
  v.next = p->next;
  v.field_kind = kind;
  it.field_kind = kind;
  it.cons_id = p_it.cons_id;
  it.elem_id = count_of_FildeshAT(sxpb->values);
  p->next = it.elem_id;
  /* Wait until end to modify array to avoid invalidating `p` pointer.*/
  push_FildeshAT(sxpb->values, v);
  return it;
}

  FildeshSxpbIT
direct_ensure_subfield_FildeshSxpb(
    FildeshSxpb* sxpb,
    FildeshSxpbIT m_it,
    const char* key,
    size_t key_size)
{
  FildeshSxprotoValue* const m = (
      fildesh_nullid(m_it.elem_id)
      ? &(*sxpb->values)[m_it.cons_id]
      : &(*sxpb->values)[m_it.elem_id]);
  FildeshSxpbIT p_it = DEFAULT_FildeshSxpbIT;
  FildeshSxprotoValue* p = NULL;

  assert(m->field_kind == FildeshSxprotoFieldKind_MESSAGE);
  p_it.cons_id = m - *sxpb->values;

  if (!fildesh_nullid(m->elem)) {
    FildeshSxprotoValue* x;
    for (x = &(*sxpb->values)[m->elem];
         x;
         x = (!fildesh_nullid(x->next) ? &(*sxpb->values)[x->next] : NULL))
    {
      const size_t n = strlen(x->text);
      if (key_size < n) {
        break;
      }
      if (key_size == n) {
        int sign = memcmp(key, x->text, key_size);
        if (sign < 0) {
          break;
        }
        if (sign == 0) {
          p_it.elem_id = x - *sxpb->values;
          p_it.field_kind = x->field_kind;
          return p_it;
        }
      }
      p = x;
    }
  }

  if (!p) {
    return direct_insert_first_FildeshSxpb(
        sxpb, m_it, key, FildeshSxprotoFieldKind_UNKNOWN);
  }

  p_it.elem_id = p - *sxpb->values;
  return direct_insert_next_FildeshSxpb(
      sxpb, p_it, key, FildeshSxprotoFieldKind_UNKNOWN);
}

