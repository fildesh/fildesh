#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "src/sxproto/value.h"

void print_json_literal_value_FildeshO(FildeshO*, const FildeshSxprotoValue*);

static void
print_yaml_key_FildeshO(FildeshO* out, const FildeshSxprotoValue* e)
{
  putstr_FildeshO(out, e->text);
  putc_FildeshO(out, ':');
}

static void
write_yaml_FildeshO(
    FildeshO* out,
    const FildeshSxpb* sxpb,
    FildeshSxpbIT it,
    unsigned indent_level,
    bool on_indented_line)
{
#define NEWLINE_INDENT(n, on_indented_line)  do { \
  if (on_indented_line) { \
    on_indented_line = false; \
  } \
  else { \
    putc_FildeshO(out, '\n'); \
    repeat_byte_FildeshO(out, ' ', 2*(n)); \
  } \
} while (0)
  const FildeshSxprotoValue* m = &(*sxpb->values)[it.cons_id];
  it.elem_id = m->elem;
  while (!fildesh_nullid(it.elem_id)) {
    const FildeshSxprotoValue* const e = &(*sxpb->values)[it.elem_id];
    FildeshSxpbIT sub_it = DEFAULT_FildeshSxpbIT;
    sub_it.cons_id = it.elem_id;
    if (m->field_kind == FildeshSxprotoFieldKind_MESSAGE ||
        m->field_kind == FildeshSxprotoFieldKind_LONEOF)
    {
      NEWLINE_INDENT(indent_level, on_indented_line);
      print_yaml_key_FildeshO(out, e);
      if (e->field_kind == FildeshSxprotoFieldKind_MESSAGE ||
          e->field_kind == FildeshSxprotoFieldKind_LONEOF)
      {
        if (fildesh_nullid(e->elem)) {
          putstrlit_FildeshO(out, " {}");
        }
        else {
          write_yaml_FildeshO(out, sxpb, sub_it, indent_level+1, false);
        }
      }
      else if (e->field_kind == FildeshSxprotoFieldKind_ARRAY ||
               e->field_kind == FildeshSxprotoFieldKind_MANYOF)
      {
        if (fildesh_nullid(e->elem)) {
          putstrlit_FildeshO(out, " []");
        }
        else {
          write_yaml_FildeshO(out, sxpb, sub_it, indent_level+1, false);
        }
      }
      else {
        assert(!fildesh_nullid(e->elem));
        putc_FildeshO(out, ' ');
        print_json_literal_value_FildeshO(out, &(*sxpb->values)[e->elem]);
      }
    }
    else if (m->field_kind == FildeshSxprotoFieldKind_ARRAY) {
      NEWLINE_INDENT(indent_level, on_indented_line);
      putstrlit_FildeshO(out, "- ");
      if (e->field_kind == FildeshSxprotoFieldKind_MESSAGE ||
          e->field_kind == FildeshSxprotoFieldKind_LONEOF)
      {
        if (fildesh_nullid(e->elem)) {
          putstrlit_FildeshO(out, "{}");
        }
        else {
          write_yaml_FildeshO(out, sxpb, sub_it, indent_level+1, true);
        }
      }
      else {
        print_json_literal_value_FildeshO(out, e);
      }
    }
    else {
      assert(m->field_kind == FildeshSxprotoFieldKind_MANYOF);
      NEWLINE_INDENT(indent_level, on_indented_line);
      putstrlit_FildeshO(out, "- ");
      if (e->field_kind == FildeshSxprotoFieldKind_MESSAGE) {
        print_yaml_key_FildeshO(out, e);
        if (fildesh_nullid(e->elem)) {
          putstrlit_FildeshO(out, " {}");
        }
        else {
          write_yaml_FildeshO(out, sxpb, sub_it, indent_level+2, false);
        }
      }
      else if (e->field_kind == FildeshSxprotoFieldKind_ARRAY ||
               e->field_kind == FildeshSxprotoFieldKind_MANYOF)
      {
        print_yaml_key_FildeshO(out, e);
        if (fildesh_nullid(e->elem)) {
          putstrlit_FildeshO(out, " []");
        }
        else {
          write_yaml_FildeshO(out, sxpb, sub_it, indent_level+2, false);
        }
      }
      else {
        if (fildesh_nullid(e->elem)) {
          putstrlit_FildeshO(out, "value: ");
          print_json_literal_value_FildeshO(out, e);
        }
        else {
          assert(e->field_kind == FildeshSxprotoFieldKind_LITERAL);
          print_yaml_key_FildeshO(out, e);
          putc_FildeshO(out, ' ');
          print_json_literal_value_FildeshO(out, &(*sxpb->values)[e->elem]);
        }
      }
    }
    it.elem_id = e->next;
  }
}

  void
print_yaml_FildeshO(FildeshO* out, FildeshSxpb* sxpb)
{
  putstrlit_FildeshO(out, "---");
  write_yaml_FildeshO(
      out, sxpb,
      top_of_FildeshSxpb(sxpb),
      0, false);
  putstrlit_FildeshO(out, "\n");
}

