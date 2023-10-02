#include "src/sxproto/value.h"

#include <assert.h>
#include <stdlib.h>

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

  FildeshSxpbIT
first_at_FildeshSxpb(const FildeshSxpb* sxpb, FildeshSxpbIT it)
{
  if (fildesh_nullid(it.elem_id)) {
    it.elem_id = (*sxpb->values)[it.cons_id].elem;
  }
  else {
    it.cons_id = it.elem_id;
    it.elem_id = (*sxpb->values)[it.elem_id].elem;
  }
  if (fildesh_nullid(it.elem_id)) {
    const FildeshSxpbIT end = DEFAULT_FildeshSxpbIT;
    return end;
  }
  return it;
}

  FildeshSxpbIT
next_at_FildeshSxpb(const FildeshSxpb* sxpb, FildeshSxpbIT it)
{
  assert(!fildesh_nullid(it.cons_id));
  assert(!fildesh_nullid(it.elem_id));

  it.elem_id = (*sxpb->values)[it.elem_id].next;
  if (fildesh_nullid(it.elem_id)) {
    const FildeshSxpbIT end = DEFAULT_FildeshSxpbIT;
    return end;
  }
  return it;
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

