#include "src/sxproto/print/value.h"

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
  else if (e->field_kind == FildeshSxprotoFieldKind_LITERAL_BOOL) {
    putstr_FildeshO(out, e->text);
  }
  else if (e->text[0] == '+') {
    putstr_FildeshO(out, &e->text[1]);
  }
  else {
    putstr_FildeshO(out, e->text);
  }
}

  void
print_json_literal_value_FildeshO(FildeshO* out, const FildeshSxprotoValue* e)
{
  if (e->field_kind == FildeshSxprotoFieldKind_LITERAL_BOOL) {
    putstr_FildeshO(out, &e->text[1]);
  }
  else if (e->field_kind == FildeshSxprotoFieldKind_LITERAL_FLOAT) {
    if (e->text[0] == '-') {
      putc_FildeshO(out, '-');
    }
    putc_FildeshO(out, e->text[1]);
    putc_FildeshO(out, '.');
    if (e->text[3] == 'e') {
      putc_FildeshO(out, '0');
    }
    putstr_FildeshO(out, &e->text[3]);
  }
  else {
    print_sxpb_literal_value_FildeshO(out, e);
  }
}

  void
print_json_indent_FildeshO(FildeshO* out, unsigned indent_level)
{
  repeat_byte_FildeshO(out, ' ', 2*indent_level);
}

  void
print_newline_json_indent_FildeshO(FildeshO* out, unsigned indent_level)
{
  putc_FildeshO(out, '\n');
  print_json_indent_FildeshO(out, indent_level);
}

  const char*
name_of_manyof_entry_FildeshSxpb(
    const FildeshSxpb* sxpb,
    FildeshSxpbIT it)
{
  const char* name = name_at_FildeshSxpb(sxpb, it);
  return name ? name : "value";
}

  const char*
name_of_entry_FildeshSxpb(
    const FildeshSxpb* sxpb,
    FildeshSxpbIT it)
{
  const FildeshSxprotoValue* m = &(*sxpb->values)[it.cons_id];
  if (is_like_dict_FildeshSxprotoFieldKind(m->field_kind) ||
      m->field_kind == FildeshSxprotoFieldKind_NEST) {
    return name_at_FildeshSxpb(sxpb, it);
  }
  if (m->field_kind == FildeshSxprotoFieldKind_MANYOF) {
    return name_of_manyof_entry_FildeshSxpb(sxpb, it);
  }
  return NULL;
}
