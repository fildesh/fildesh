#include "src/sxproto/print/value.h"

static void
write_json_FildeshO(
    FildeshO* out,
    const FildeshSxpb* sxpb,
    FildeshSxpbIT it,
    const char* name,
    unsigned indent_level);

static
  void
print_json_child_of_manyof(
    FildeshO* out,
    const FildeshSxpb* sxpb,
    FildeshSxpbIT it,
    unsigned indent_level)
{
  const char* name = name_of_manyof_entry_FildeshSxpb(sxpb, it);
  putc_FildeshO(out, '{');
  print_newline_json_indent_FildeshO(out, indent_level+1);
  write_json_FildeshO(out, sxpb, it, name, indent_level+1);
  print_newline_json_indent_FildeshO(out, indent_level);
  putc_FildeshO(out, '}');
}

static
  void
print_json_child_of_nest(
    FildeshO* out,
    const FildeshSxpb* sxpb,
    FildeshSxpbIT it,
    unsigned indent_level)
{
  const char* name = name_of_entry_FildeshSxpb(sxpb, it);
  print_newline_json_indent_FildeshO(out, indent_level+1);
  if (it.field_kind == FildeshSxprotoFieldKind_LITERAL_STRING) {
    write_json_FildeshO(out, sxpb, it, name, indent_level+1);
  }
  else {
    putc_FildeshO(out, '{');
    print_newline_json_indent_FildeshO(out, indent_level+2);
    write_json_FildeshO(out, sxpb, it, name, indent_level+2);
    print_newline_json_indent_FildeshO(out, indent_level+1);
    putc_FildeshO(out, '}');
  }
}

static
  void
write_json_FildeshO(
    FildeshO* out,
    const FildeshSxpb* sxpb,
    FildeshSxpbIT it,
    const char* name,
    unsigned indent_level)
{
  const FildeshSxprotoValue* const m = &(*sxpb->values)[id_of_FildeshSxpbIT(it)];
  const bool is_like_dict = is_like_dict_FildeshSxprotoFieldKind(m->field_kind);
  const bool is_like_list = is_like_list_FildeshSxprotoFieldKind(m->field_kind);
  bool first = true;

  if (name) {
    print_quoted_sxpb_str_FildeshO(out, name);
    putstrlit_FildeshO(out, ": ");
  }

  if (is_like_dict) {
    putc_FildeshO(out, '{');
  }
  else if (is_like_list) {
    putc_FildeshO(out, '[');
  }

  for (it = first_at_FildeshSxpb(sxpb, it);
       !nullish_FildeshSxpbIT(it);
       it = next_at_FildeshSxpb(sxpb, it))
  {
    if (!first) {
      putc_FildeshO(out, ',');
      if (is_like_list && m->field_kind != FildeshSxprotoFieldKind_NEST) {
        putc_FildeshO(out, ' ');
      }
    }

    if (m->field_kind == FildeshSxprotoFieldKind_MANYOF) {
      print_json_child_of_manyof(out, sxpb, it, indent_level);
    }
    else if (m->field_kind == FildeshSxprotoFieldKind_NEST) {
      print_json_child_of_nest(out, sxpb, it, indent_level);
    }
    else {
      const char* sub_name = name_of_entry_FildeshSxpb(sxpb, it);
      unsigned sub_indent_level = (sub_name ? indent_level + 1 : indent_level);
      if (sub_name) {
        print_newline_json_indent_FildeshO(out, indent_level+1);
      }
      write_json_FildeshO(out, sxpb, it, sub_name, sub_indent_level);
    }
    first = false;
  }

  if (is_like_dict) {
    if (!first || !name) {
      print_newline_json_indent_FildeshO(out, indent_level);
    }
    putc_FildeshO(out, '}');
  }
  else if (is_like_list) {
    if (m->field_kind == FildeshSxprotoFieldKind_NEST && !first) {
      print_newline_json_indent_FildeshO(out, indent_level);
    }
    putc_FildeshO(out, ']');
  }
  else if (first) {
    print_json_literal_value_FildeshO(out, m);
  }
}

  void
print_json_FildeshO(FildeshO* out, FildeshSxpb* sxpb)
{
  const FildeshSxpbIT top_it = top_of_FildeshSxpb(sxpb);
  write_json_FildeshO(out, sxpb, top_it, NULL, 0);
  putc_FildeshO(out, '\n');
}
