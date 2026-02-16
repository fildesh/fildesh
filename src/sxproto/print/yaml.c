#include "src/sxproto/print/value.h"

static void
write_yaml_FildeshO(
    FildeshO* out,
    const FildeshSxpb* sxpb,
    FildeshSxpbIT it,
    const char* name,
    unsigned indent_level);

static
  void
print_yaml_child_of_manyof(
    FildeshO* out,
    const FildeshSxpb* sxpb,
    FildeshSxpbIT it,
    unsigned indent_level)
{
  const char* name = name_of_manyof_entry_FildeshSxpb(sxpb, it);
  print_newline_json_indent_FildeshO(out, indent_level);
  putstrlit_FildeshO(out, "- ");
  write_yaml_FildeshO(out, sxpb, it, name, indent_level+2);
}

static
  void
write_yaml_FildeshO(
    FildeshO* out,
    const FildeshSxpb* sxpb,
    FildeshSxpbIT it,
    const char* name,
    unsigned indent_level)
{
  const FildeshSxprotoValue* const m = &(*sxpb->values)[id_of_FildeshSxpbIT(it)];
  const bool is_like_dict = is_like_dict_FildeshSxprotoFieldKind(m->field_kind);
  const bool is_like_list = is_like_list_FildeshSxprotoFieldKind(m->field_kind);
  const bool is_top = is_top_of_FildeshSxpb(sxpb, it);
  bool first = true;

  if (name) {
    putstr_FildeshO(out, name);
    putc_FildeshO(out, ':');
    if (!is_like_dict && !is_like_list) {
      putc_FildeshO(out, ' ');
    }
  }

  for (it = first_at_FildeshSxpb(sxpb, it);
       !nullish_FildeshSxpbIT(it);
       it = next_at_FildeshSxpb(sxpb, it))
  {
    if (m->field_kind == FildeshSxprotoFieldKind_ARRAY) {
      print_newline_json_indent_FildeshO(out, indent_level);
      putstrlit_FildeshO(out, "- ");
      write_yaml_FildeshO(out, sxpb, it, NULL, indent_level+1);
    }
    else if (m->field_kind == FildeshSxprotoFieldKind_MANYOF) {
      print_yaml_child_of_manyof(out, sxpb, it, indent_level);
    }
    else {
      const char* sub_name = name_of_entry_FildeshSxpb(sxpb, it);
      unsigned sub_indent_level = (sub_name ? indent_level + 1 : indent_level);
      if (is_top || (sub_name && (name || !first))) {
        print_newline_json_indent_FildeshO(out, indent_level);
      }
      write_yaml_FildeshO(
          out, sxpb, it,
          name_at_FildeshSxpb(sxpb, it),
          sub_indent_level);
    }
    first = false;
  }

  if (first) {
    if (is_like_dict) {
      if (name) {
        putc_FildeshO(out, ' ');
      }
      putstrlit_FildeshO(out, "{}");
    }
    else if (is_like_list) {
      if (name) {
        putc_FildeshO(out, ' ');
      }
      putstrlit_FildeshO(out, "[]");
    }
    else {
      print_json_literal_value_FildeshO(out, m);
    }
  }
}

  void
print_yaml_FildeshO(FildeshO* out, FildeshSxpb* sxpb)
{
  const FildeshSxpbIT top_it = top_of_FildeshSxpb(sxpb);
  putstrlit_FildeshO(out, "---");
  write_yaml_FildeshO(out, sxpb, top_it, NULL, 0);
  putstrlit_FildeshO(out, "\n");
}
