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
  double x = UINT_MAX;
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
ensure_message_subfield_at_FildeshSxpb(
    FildeshSxpb* sxpb, FildeshSxpbIT it, const char* k)
{
  FildeshSxprotoValue* e;
  const size_t n = strlen(k);
  k = ensure_name_FildeshSxpb(sxpb, k, n);
  it = direct_ensure_subfield_FildeshSxpb(sxpb, it, k, n);
  e = &(*sxpb->values)[it.elem_id];
  if (it.field_kind == FildeshSxprotoFieldKind_UNKNOWN) {
    it.field_kind = FildeshSxprotoFieldKind_MESSAGE;
    e->field_kind = FildeshSxprotoFieldKind_MESSAGE;
  }
  assert(e->field_kind == FildeshSxprotoFieldKind_MESSAGE);
  return it;
}

  FildeshSxpbIT
assign_bool_subfield_at_FildeshSxpb(
    FildeshSxpb* sxpb, FildeshSxpbIT it, const char* k, bool v)
{
  FildeshSxprotoValue* e;
  const size_t n = strlen(k);
  const char* v_text = v ? "true" : "false";
  k = ensure_name_FildeshSxpb(sxpb, k, n);
  v_text = ensure_name_FildeshSxpb(sxpb, v_text, strlen(v_text));
  it = direct_ensure_subfield_FildeshSxpb(sxpb, it, k, n);
  e = &(*sxpb->values)[it.elem_id];
  if (it.field_kind == FildeshSxprotoFieldKind_UNKNOWN) {
    it.field_kind = FildeshSxprotoFieldKind_LITERAL;
    e->field_kind = FildeshSxprotoFieldKind_LITERAL;
    direct_insert_first_FildeshSxpb(
        sxpb, it, v_text, FildeshSxprotoFieldKind_LITERAL_BOOL);
  }
  else {
    assert(!fildesh_nullid(e->elem));
    e = &(*sxpb->values)[e->elem];
    assert(e->field_kind == FildeshSxprotoFieldKind_LITERAL_BOOL);
    e->text = v_text;
  }
  return it;
}

  FildeshSxpbIT
assign_str_subfield_at_FildeshSxpb(
    FildeshSxpb* sxpb, FildeshSxpbIT it, const char* k, const char* v)
{
  FildeshSxprotoValue* e;
  const size_t n = strlen(k);
  k = ensure_name_FildeshSxpb(sxpb, k, n);
  v = ensure_name_FildeshSxpb(sxpb, v, strlen(v));
  it = direct_ensure_subfield_FildeshSxpb(sxpb, it, k, n);
  e = &(*sxpb->values)[it.elem_id];
  if (it.field_kind == FildeshSxprotoFieldKind_UNKNOWN) {
    it.field_kind = FildeshSxprotoFieldKind_LITERAL;
    e->field_kind = FildeshSxprotoFieldKind_LITERAL;
    direct_insert_first_FildeshSxpb(
        sxpb, it, v, FildeshSxprotoFieldKind_LITERAL_STRING);
  }
  else {
    assert(!fildesh_nullid(e->elem));
    e = &(*sxpb->values)[e->elem];
    assert(e->field_kind == FildeshSxprotoFieldKind_LITERAL_STRING);
    e->text = v;
  }
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

