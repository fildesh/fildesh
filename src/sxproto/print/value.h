#ifndef FILDESH_SXPROTO_PRINT_VALUE_H_
#define FILDESH_SXPROTO_PRINT_VALUE_H_
#include "src/sxproto/value.h"

void print_quoted_sxpb_str_FildeshO(FildeshO* out, const char* s);
void print_sxpb_literal_value_FildeshO(FildeshO* out, const FildeshSxprotoValue* e);
void print_json_literal_value_FildeshO(FildeshO*, const FildeshSxprotoValue*);
void print_json_indent_FildeshO(FildeshO* out, unsigned indent_level);
void print_newline_json_indent_FildeshO(FildeshO* out, unsigned indent_level);

const char*
name_of_manyof_entry_FildeshSxpb(const FildeshSxpb* sxpb, FildeshSxpbIT it);
const char*
name_of_entry_FildeshSxpb(const FildeshSxpb* sxpb, FildeshSxpbIT it);

static inline
  bool
is_like_dict_FildeshSxprotoFieldKind(FildeshSxprotoFieldKind kind)
{
  return (
      kind == FildeshSxprotoFieldKind_MESSAGE ||
      kind == FildeshSxprotoFieldKind_LONEOF);
}

static inline
  bool
is_like_list_FildeshSxprotoFieldKind(FildeshSxprotoFieldKind kind)
{
  return (
      kind == FildeshSxprotoFieldKind_ARRAY ||
      kind == FildeshSxprotoFieldKind_MANYOF ||
      kind == FildeshSxprotoFieldKind_NEST);
}

static inline FildeshSxpb_id id_of_FildeshSxpbIT(FildeshSxpbIT it) {
  return fildesh_nullid(it.elem_id) ? it.cons_id : it.elem_id;
}

static inline
  bool
is_top_of_FildeshSxpb(const FildeshSxpb* sxpb, FildeshSxpbIT it)
{
  (void) sxpb;
  return id_of_FildeshSxpbIT(it) == 0;
}

#endif
