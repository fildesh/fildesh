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

#endif
