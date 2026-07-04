#include "src/sxproto/print/value.h"

static void
write_txtpb_FildeshO(
    FildeshO* out,
    const FildeshSxpb* sxpb,
    FildeshSxpbIT it,
    const char* name,
    unsigned indent_level);

static
  void
print_txtpb_child_of_dict(
    FildeshO* out,
    const FildeshSxpb* sxpb,
    FildeshSxpbIT it,
    unsigned indent_level)
{
  const char* key = name_of_entry_FildeshSxpb(sxpb, it);
  putc_FildeshO(out, '{');
  print_newline_json_indent_FildeshO(out, indent_level+1);
  putstrlit_FildeshO(out, "key: ");
  print_quoted_sxpb_str_FildeshO(out, key);
  print_newline_json_indent_FildeshO(out, indent_level+1);
  write_txtpb_FildeshO(out, sxpb, it, "value", indent_level+1);
  print_newline_json_indent_FildeshO(out, indent_level);
  putc_FildeshO(out, '}');
}

static
  void
print_txtpb_child_of_manyof(
    FildeshO* out,
    const FildeshSxpb* sxpb,
    FildeshSxpbIT it,
    unsigned indent_level)
{
  const char* name = name_of_manyof_entry_FildeshSxpb(sxpb, it);
  putc_FildeshO(out, '{');
  print_newline_json_indent_FildeshO(out, indent_level+1);
  write_txtpb_FildeshO(out, sxpb, it, name, indent_level+1);
  print_newline_json_indent_FildeshO(out, indent_level);
  putc_FildeshO(out, '}');
}

static
  void
print_txtpb_child_of_nest(
    FildeshO* out,
    const FildeshSxpb* sxpb,
    FildeshSxpbIT it,
    unsigned indent_level)
{
  const char* name = name_of_entry_FildeshSxpb(sxpb, it);
  putc_FildeshO(out, '{');
  print_newline_json_indent_FildeshO(out, indent_level+1);
  putstrlit_FildeshO(out, "key: ");
  if (it.field_kind == FildeshSxprotoFieldKind_NEST) {
    print_quoted_sxpb_str_FildeshO(out, name);
    print_newline_json_indent_FildeshO(out, indent_level+1);
    putstrlit_FildeshO(out, "values: ");
  }
  write_txtpb_FildeshO(out, sxpb, it, NULL, indent_level+1);
  print_newline_json_indent_FildeshO(out, indent_level);
  putc_FildeshO(out, '}');
}

  static void
write_txtpb_FildeshO(
    FildeshO* out,
    const FildeshSxpb* sxpb,
    FildeshSxpbIT it,
    const char* name,
    unsigned indent_level)
{
  const FildeshSxprotoValue* const m = &(*sxpb->values)[id_of_FildeshSxpbIT(it)];
  const bool is_like_list = (
      is_protobuf_repeated_FildeshSxprotoFieldKind(m->field_kind));
  const bool is_like_mesg = (
      is_like_dict_FildeshSxprotoFieldKind(m->field_kind) &&
      m->field_kind != FildeshSxprotoFieldKind_DICT);
  const bool is_top = is_top_of_FildeshSxpb(sxpb, it);
  bool first = true;

  if (name) {
    putstr_FildeshO(out, name);
    if (!is_like_mesg) {
      putc_FildeshO(out, ':');
    }
    putc_FildeshO(out, ' ');
  }

  if (is_like_mesg) {
    if (!is_top) {
      putc_FildeshO(out, '{');
    }
  }
  else if (is_like_list) {
    putc_FildeshO(out, '[');
  }

  for (it = first_at_FildeshSxpb(sxpb, it);
       !nullish_FildeshSxpbIT(it);
       it = next_at_FildeshSxpb(sxpb, it))
  {
    if (!first) {
      if (is_like_list) {
        putstrlit_FildeshO(out, ", ");
      }
    }

    if (m->field_kind == FildeshSxprotoFieldKind_DICT) {
      print_txtpb_child_of_dict(out, sxpb, it, indent_level);
    }
    else if (m->field_kind == FildeshSxprotoFieldKind_MANYOF) {
      print_txtpb_child_of_manyof(out, sxpb, it, indent_level);
    }
    else if (m->field_kind == FildeshSxprotoFieldKind_NEST) {
      print_txtpb_child_of_nest(out, sxpb, it, indent_level);
    }
    else {
      const char* sub_name = name_of_entry_FildeshSxpb(sxpb, it);
      unsigned sub_indent_level = (
          sub_name && !is_top
          ? indent_level + 1
          : indent_level);
      if (sub_name && (!is_top || !first)) {
        print_newline_json_indent_FildeshO(out, sub_indent_level);
      }
      write_txtpb_FildeshO(out, sxpb, it, sub_name, sub_indent_level);
    }
    first = false;
  }

  if (is_like_mesg && !is_top) {
    /* Add newline when nonempty or within an array.*/
    if (!first || !name) {
      print_newline_json_indent_FildeshO(out, indent_level);
    }
    putc_FildeshO(out, '}');
  }
  else if (is_like_list) {
    putc_FildeshO(out, ']');
  }
  else if (first) {
    print_sxpb_literal_value_FildeshO(out, m);
  }
}

  void
print_txtpb_FildeshO(FildeshO* out, FildeshSxpb* sxpb)
{
  const FildeshSxpbIT top_it = top_of_FildeshSxpb(sxpb);
  write_txtpb_FildeshO(out, sxpb, top_it, NULL, 0);
  putc_FildeshO(out, '\n');
}
