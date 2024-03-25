#include <assert.h>
#include <string.h>

#include "src/sxproto/value.h"

FildeshSxpb* open_FildeshSxpb() {
  const FildeshSxprotoValue default_toplevel_cons = {
    "",
    FildeshSxprotoFieldKind_MESSAGE,
    ~(FildeshSxpb_id)0,
    ~(FildeshSxpb_id)0,
  };
  FildeshAlloc* alloc = open_FildeshAlloc();
  FildeshSxpb* sxpb = fildesh_allocate(FildeshSxpb, 1, alloc);
  const FildeshKV default_map = DEFAULT_FildeshKV;
  *sxpb->name_by_name = default_map;
  sxpb->name_by_name->alloc = alloc;
  init_FildeshAT(sxpb->values);
  push_FildeshAT(sxpb->values, default_toplevel_cons);
  return sxpb;
}

void close_FildeshSxpb(FildeshSxpb* sxpb) {
  close_FildeshAT(sxpb->values);
  close_FildeshKV(sxpb->name_by_name);
  close_FildeshAlloc(sxpb->name_by_name->alloc);
}

  const char*
ensure_name_FildeshSxpb(FildeshSxpb* sxpb, const char* s, size_t n)
{
  FildeshKV_id id = lookup_FildeshKV(sxpb->name_by_name, s, n);
  if (fildesh_nullid(id)) {
    char* v = fildesh_allocate(char, n+1, sxpb->name_by_name->alloc);
    memcpy(v, s, n);
    v[n] = '\0';
    id = ensuref_FildeshKV(sxpb->name_by_name, v, n);
    assign_memref_at_FildeshKV(sxpb->name_by_name, id, v);
    return v;
  }
  return (const char*) value_at_FildeshKV(sxpb->name_by_name, id);
}

static
  FildeshSxpbIT
ensure_subfield_at_FildeshSxpb(
    FildeshSxpb* sxpb, FildeshSxpbIT it, const char* k,
    FildeshSxprotoFieldKind kind)
{
  FildeshSxprotoValue* e;
  const size_t n = strlen(k);
  const char* v_text;
  k = ensure_name_FildeshSxpb(sxpb, k, n);
  it = direct_ensure_subfield_FildeshSxpb(sxpb, it, k, n);
  e = &(*sxpb->values)[it.elem_id];
  if (e->field_kind == kind) {return it;}
  if (e->field_kind == FildeshSxprotoFieldKind_LITERAL) {
    e = &(*sxpb->values)[e->elem];
    assert(e->field_kind == kind);
    return it;
  }
  assert(e->field_kind == FildeshSxprotoFieldKind_UNKNOWN);
  v_text = default_value_text_FildeshSxpb(sxpb, kind);
  if (v_text) {
    it.field_kind = FildeshSxprotoFieldKind_LITERAL;
    e->field_kind = FildeshSxprotoFieldKind_LITERAL;
    direct_insert_first_FildeshSxpb(sxpb, it, v_text, kind);
  }
  else {
    it.field_kind = kind;
    e->field_kind = kind;
  }
  return it;
}

  FildeshSxpbIT
ensure_message_subfield_at_FildeshSxpb(FildeshSxpb* sxpb, FildeshSxpbIT it, const char* k) {
  return ensure_subfield_at_FildeshSxpb(sxpb, it, k, FildeshSxprotoFieldKind_MESSAGE);
}
  FildeshSxpbIT
ensure_loneof_subfield_at_FildeshSxpb(FildeshSxpb* sxpb, FildeshSxpbIT it, const char* k) {
  return ensure_subfield_at_FildeshSxpb( sxpb, it, k, FildeshSxprotoFieldKind_LONEOF);
}
  FildeshSxpbIT
ensure_array_subfield_at_FildeshSxpb(FildeshSxpb* sxpb, FildeshSxpbIT it, const char* k) {
  return ensure_subfield_at_FildeshSxpb(sxpb, it, k, FildeshSxprotoFieldKind_ARRAY);
}
  FildeshSxpbIT
ensure_manyof_subfield_at_FildeshSxpb(FildeshSxpb* sxpb, FildeshSxpbIT it, const char* k) {
  return ensure_subfield_at_FildeshSxpb( sxpb, it, k, FildeshSxprotoFieldKind_MANYOF);
}
  FildeshSxpbIT
ensure_bool_subfield_at_FildeshSxpb(FildeshSxpb* sxpb, FildeshSxpbIT it, const char* k) {
  return ensure_subfield_at_FildeshSxpb(sxpb, it, k, FildeshSxprotoFieldKind_LITERAL_BOOL);
}
  FildeshSxpbIT
ensure_int_subfield_at_FildeshSxpb(FildeshSxpb* sxpb, FildeshSxpbIT it, const char* k) {
  return ensure_subfield_at_FildeshSxpb(sxpb, it, k, FildeshSxprotoFieldKind_LITERAL_INT);
}
  FildeshSxpbIT
ensure_float_subfield_at_FildeshSxpb(FildeshSxpb* sxpb, FildeshSxpbIT it, const char* k) {
  return ensure_subfield_at_FildeshSxpb(sxpb, it, k, FildeshSxprotoFieldKind_LITERAL_FLOAT);
}
  FildeshSxpbIT
ensure_string_subfield_at_FildeshSxpb(FildeshSxpb* sxpb, FildeshSxpbIT it, const char* k) {
  return ensure_subfield_at_FildeshSxpb(sxpb, it, k, FildeshSxprotoFieldKind_LITERAL_STRING);
}

  FildeshSxpbIT
lookup_subfield_at_FildeshSxpb(
    const FildeshSxpb* sxpb,
    FildeshSxpbIT m,
    const char* k)
{
  FildeshSxpb_id eid;
  const FildeshSxprotoValue* e;
  FildeshSxpbIT pos = DEFAULT_FildeshSxpbIT;

  assert(!nullish_FildeshSxpbIT(m));
  pos.elem_id = !fildesh_nullid(m.elem_id) ? m.elem_id : m.cons_id;
  e = &(*sxpb->values)[pos.elem_id];
  assert(e->field_kind == FildeshSxprotoFieldKind_MESSAGE ||
         e->field_kind == FildeshSxprotoFieldKind_LONEOF);

  if (fildesh_nullid(e->elem)) {
    return pos;
  }

  for (eid = e->elem; !fildesh_nullid(eid); eid = e->next) {
    e = &(*sxpb->values)[eid];
    assert(e->text);
    if (0 == strcmp(e->text, k)) {
      pos.field_kind = e->field_kind;
      pos.cons_id = pos.elem_id;
      pos.elem_id = eid;
      break;
    }
  }
  return pos;
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

/** Remove subtree of sxpb. No attempt to reclaim memory (yet).**/
  void
remove_at_FildeshSxpb(FildeshSxpb* sxpb, FildeshSxpbIT it)
{
  FildeshSxprotoValue* p;
  assert(!nullish_FildeshSxpbIT(it));
  assert(!fildesh_nullid(it.elem_id));
  p = &(*sxpb->values)[it.cons_id];
  if (p->elem == it.elem_id) {
    p->elem = (*sxpb->values)[it.elem_id].next;
  }
  else {
    p = &(*sxpb->values)[p->elem];
    while (p->next != it.elem_id) {
      assert(!fildesh_nullid(p->next));
      p = &(*sxpb->values)[p->next];
    }
    p->next = (*sxpb->values)[it.elem_id].next;
  }
}

  const char*
name_at_FildeshSxpb(const FildeshSxpb* sxpb, FildeshSxpbIT it)
{
  const FildeshSxprotoValue* const e = &(*sxpb->values)[(
      fildesh_nullid(it.elem_id) ? it.cons_id : it.elem_id)];
  assert(!nullish_FildeshSxpbIT(it));
  if (e->field_kind == FildeshSxprotoFieldKind_MESSAGE ||
      e->field_kind == FildeshSxprotoFieldKind_LITERAL ||
      e->field_kind == FildeshSxprotoFieldKind_LONEOF ||
      e->field_kind == FildeshSxprotoFieldKind_ARRAY ||
      e->field_kind == FildeshSxprotoFieldKind_MANYOF)
  {
    return e->text;
  }
  return NULL;
}

