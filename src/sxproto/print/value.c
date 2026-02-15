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
  if (e->field_kind != FildeshSxprotoFieldKind_LITERAL_FLOAT) {
    print_sxpb_literal_value_FildeshO(out, e);
    return;
  }
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
