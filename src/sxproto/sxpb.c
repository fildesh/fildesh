#include <assert.h>
#include <string.h>

#include "src/sxproto/value.h"

FildeshSxpb* open_FildeshSxpb() {
  FildeshAlloc* alloc = open_FildeshAlloc();
  FildeshSxpb* sxpb = fildesh_allocate(FildeshSxpb, 1, alloc);
  const FildeshKV default_map = DEFAULT_FildeshKV;
  const FildeshSxprotoValue default_toplevel_cons = {
    "",
    FildeshSxprotoFieldKind_MESSAGE,
    ~(FildeshSxpb_id)0,
    ~(FildeshSxpb_id)0,
  };
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
  assert(e->field_kind == FildeshSxprotoFieldKind_MESSAGE);

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
ensure_field_at_FildeshSxpb(
    FildeshSxpb* sxpb,
    FildeshSxpbIT m,
    const char* k,
    FildeshSxprotoFieldKind kind)
{
  FildeshSxprotoValue* e;
  FildeshSxprotoValue v = DEFAULT_FildeshSxprotoValue;
  const size_t ksize = strlen(k);
  FildeshSxpbIT pos = lookup_subfield_at_FildeshSxpb(sxpb, m, k);
  if (!nullish_FildeshSxpbIT(pos)) {return pos;}
  v.text = ensure_name_FildeshSxpb(sxpb, k, ksize);
  v.field_kind = kind;

  pos.field_kind = kind;
  pos.cons_id = !fildesh_nullid(m.elem_id) ? m.elem_id : m.cons_id;
  pos.elem_id = count_of_FildeshAT(sxpb->values);
  push_FildeshAT(sxpb->values, v);

  e = &(*sxpb->values)[pos.cons_id];
  if (fildesh_nullid(e->elem)) {
    e->elem = pos.elem_id;
  }
  else {
    for (e = &(*sxpb->values)[e->elem]; !fildesh_nullid(e->next);
         e = &(*sxpb->values)[e->next]) {
      /* Nothing here.*/
    }
    e->next = pos.elem_id;
  }
  return pos;
}

  const char*
name_at_FildeshSxpb(const FildeshSxpb* sxpb, FildeshSxpbIT it)
{
  const FildeshSxprotoValue* e;
  assert(!nullish_FildeshSxpbIT(it));
  if (!fildesh_nullid(it.elem_id)) {
    e = &(*sxpb->values)[it.elem_id];
    if (e->field_kind == FildeshSxprotoFieldKind_MESSAGE ||
        e->field_kind == FildeshSxprotoFieldKind_LITERAL ||
        e->field_kind == FildeshSxprotoFieldKind_ARRAY ||
        e->field_kind == FildeshSxprotoFieldKind_MANYOF)
    {
      return e->text;
    }
  }
  e = &(*sxpb->values)[it.cons_id];
  return e->text;
}

