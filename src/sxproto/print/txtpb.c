#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "src/sxproto/value.h"

static void
write_txtpb_FildeshO(
    FildeshO* out,
    const FildeshSxpb* sxpb,
    FildeshSxpbIT it,
    unsigned indent_level,
    bool newline_on)
{
#define NEWLINE_INDENT(n)  do { \
  if (newline_on) {putc_FildeshO(out, '\n');} \
  else {newline_on = true;} \
  repeat_byte_FildeshO(out, ' ', 2*(n)); \
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
      NEWLINE_INDENT(indent_level);
      putstr_FildeshO(out, e->text);
      if (e->field_kind == FildeshSxprotoFieldKind_MESSAGE ||
          e->field_kind == FildeshSxprotoFieldKind_LONEOF)
      {
        if (fildesh_nullid(e->elem)) {
          putstrlit_FildeshO(out, " {}");
        }
        else {
          putstrlit_FildeshO(out, " {");
          write_txtpb_FildeshO(out, sxpb, sub_it, indent_level+1, true);
          NEWLINE_INDENT(indent_level);
          putc_FildeshO(out, '}');
        }
      }
      else if (e->field_kind == FildeshSxprotoFieldKind_ARRAY ||
               e->field_kind == FildeshSxprotoFieldKind_MANYOF)
      {
        putstrlit_FildeshO(out, ": [");
        write_txtpb_FildeshO(out, sxpb, sub_it, indent_level, true);
        putc_FildeshO(out, ']');
      }
      else {
        assert(!fildesh_nullid(e->elem));
        putstrlit_FildeshO(out, ": ");
        print_sxpb_literal_value_FildeshO(out, &(*sxpb->values)[e->elem]);
      }
    }
    else if (m->field_kind == FildeshSxprotoFieldKind_ARRAY) {
      if (e->field_kind == FildeshSxprotoFieldKind_MESSAGE ||
          e->field_kind == FildeshSxprotoFieldKind_LONEOF)
      {
        putc_FildeshO(out, '{');
        write_txtpb_FildeshO(out, sxpb, sub_it, indent_level+1, true);
        NEWLINE_INDENT(indent_level);
        if (fildesh_nullid(e->next)) {
          putc_FildeshO(out, '}');
        }
        else {
          putstrlit_FildeshO(out, "}, ");
        }
      }
      else {
        print_sxpb_literal_value_FildeshO(out, e);
        if (!fildesh_nullid(e->next)) {
          putstrlit_FildeshO(out, ", ");
        }
      }
    }
    else {
      assert(m->field_kind == FildeshSxprotoFieldKind_MANYOF);
      putc_FildeshO(out, '{');
      NEWLINE_INDENT(indent_level+1);
      if (e->field_kind == FildeshSxprotoFieldKind_MESSAGE ||
          e->field_kind == FildeshSxprotoFieldKind_LONEOF)
      {
        putstr_FildeshO(out, e->text);
        if (fildesh_nullid(e->elem)) {
          putstrlit_FildeshO(out, " {}");
        }
        else {
          putstrlit_FildeshO(out, " {");
          write_txtpb_FildeshO(out, sxpb, sub_it, indent_level+2, true);
          NEWLINE_INDENT(indent_level+1);
          putc_FildeshO(out, '}');
        }
      }
      else if (e->field_kind == FildeshSxprotoFieldKind_ARRAY ||
               e->field_kind == FildeshSxprotoFieldKind_MANYOF)
      {
        putstr_FildeshO(out, e->text);
        putstrlit_FildeshO(out, ": [");
        write_txtpb_FildeshO(out, sxpb, sub_it, indent_level+1, true);
        putc_FildeshO(out, ']');
      }
      else {
        if (fildesh_nullid(e->elem)) {
          putstrlit_FildeshO(out, "value: ");
          print_sxpb_literal_value_FildeshO(out, e);
        }
        else {
          assert(e->field_kind == FildeshSxprotoFieldKind_LITERAL);
          putstr_FildeshO(out, e->text);
          putstrlit_FildeshO(out, ": ");
          print_sxpb_literal_value_FildeshO(out, &(*sxpb->values)[e->elem]);
        }
      }

      NEWLINE_INDENT(indent_level);
      if (fildesh_nullid(e->next)) {
        putc_FildeshO(out, '}');
      }
      else {
        putstrlit_FildeshO(out, "}, ");
      }
    }
    it.elem_id = e->next;
  }
}

  void
print_txtpb_FildeshO(FildeshO* out, FildeshSxpb* sxpb)
{
  const FildeshSxpbIT top_it = top_of_FildeshSxpb(sxpb);
  const FildeshSxprotoFieldKind top_kind = top_it.field_kind;
  const bool on_message = (top_kind == FildeshSxprotoFieldKind_MESSAGE);

  if (!on_message) {putc_FildeshO(out, '[');}
  write_txtpb_FildeshO(
      out, sxpb,
      top_of_FildeshSxpb(sxpb),
      0,
      !on_message);
  if (!on_message) {putc_FildeshO(out, ']');}
  putc_FildeshO(out, '\n');
}
