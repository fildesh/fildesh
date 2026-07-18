#ifndef FILDESH_SXPROTO_VALUE_H_
#define FILDESH_SXPROTO_VALUE_H_
#include <fildesh/sxproto.h>

struct FildeshSxprotoValue {
  const char* text;
  FildeshSxprotoFieldKind field_kind;
  FildeshSxpb_id elem;
  FildeshSxpb_id next;
};
#define DEFAULT_FildeshSxprotoValue \
{ NULL, FildeshSxprotoFieldKind_UNKNOWN, \
  ~(FildeshSxpb_id)0, ~(FildeshSxpb_id)0, \
}

const char*
default_value_text_FildeshSxpb(FildeshSxpb* sxpb, FildeshSxprotoFieldKind kind);

FildeshSxpbIT
direct_insert_first_FildeshSxpb(
    FildeshSxpb* sxpb,
    FildeshSxpbIT m_it,
    const char* text,
    FildeshSxprotoFieldKind kind);
FildeshSxpbIT
direct_insert_next_FildeshSxpb(
    FildeshSxpb* sxpb,
    FildeshSxpbIT p_it,
    const char* text,
    FildeshSxprotoFieldKind kind);
FildeshSxpbIT
direct_ensure_subfield_FildeshSxpb(
    FildeshSxpb* sxpb,
    FildeshSxpbIT m_it,
    const char* key,
    size_t key_size);

static inline FildeshSxpb_id id_of_FildeshSxpbIT(FildeshSxpbIT it) {
  return fildesh_nullid(it.elem_id) ? it.cons_id : it.elem_id;
}

static inline
  FildeshSxpb_id
literal_value_id_at_FildeshSxpb(
    const FildeshSxpb* sxpb,
    FildeshSxpbIT it)
{
  FildeshSxpb_id id = id_of_FildeshSxpbIT(it);
  const FildeshSxprotoValue* const e = &(*sxpb->values)[id];
  if (!fildesh_nullid(e->elem)) {
    id = e->elem;
  }
  return id;
}

static inline
  const FildeshSxprotoValue*
literal_value_at_FildeshSxpb(
    const FildeshSxpb* sxpb,
    FildeshSxpbIT it)
{
  return &(*sxpb->values)[literal_value_id_at_FildeshSxpb(sxpb, it)];
}

static inline
  bool
is_literal_FildeshSxprotoFieldKind(FildeshSxprotoFieldKind kind)
{
  return (
      kind == FildeshSxprotoFieldKind_LITERAL ||
      kind == FildeshSxprotoFieldKind_LITERAL_STRING ||
      kind == FildeshSxprotoFieldKind_LITERAL_BOOL ||
      kind == FildeshSxprotoFieldKind_LITERAL_INT ||
      kind == FildeshSxprotoFieldKind_LITERAL_FLOAT);
}

static inline
  bool
is_like_dict_FildeshSxprotoFieldKind(FildeshSxprotoFieldKind kind)
{
  return (
      kind == FildeshSxprotoFieldKind_MESSAGE ||
      kind == FildeshSxprotoFieldKind_DICT ||
      kind == FildeshSxprotoFieldKind_LONEOF);
}

static inline
  bool
is_protobuf_repeated_FildeshSxprotoFieldKind(FildeshSxprotoFieldKind kind)
{
  return (
      kind == FildeshSxprotoFieldKind_ARRAY ||
      kind == FildeshSxprotoFieldKind_DICT ||
      kind == FildeshSxprotoFieldKind_MANYOF ||
      kind == FildeshSxprotoFieldKind_NEST);
}

#endif
