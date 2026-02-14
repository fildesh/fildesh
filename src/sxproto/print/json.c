#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "src/sxproto/value.h"

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

static void
write_json_FildeshO(
    FildeshO* out,
    const FildeshSxpb* sxpb,
    FildeshSxpbIT it,
    unsigned indent_level)
{
#define NEWLINE_INDENT(n)  do { \
  putc_FildeshO(out, '\n'); \
  repeat_byte_FildeshO(out, ' ', 2*(n)); \
} while (0)
  const FildeshSxprotoValue* m = &(*sxpb->values)[it.cons_id];
  it.elem_id = m->elem;
  while (!fildesh_nullid(it.elem_id)) {
    const FildeshSxprotoValue* const e = &(*sxpb->values)[it.elem_id];
    bool pending_comma = !fildesh_nullid(e->next);
    FildeshSxpbIT sub_it = DEFAULT_FildeshSxpbIT;
    sub_it.cons_id = it.elem_id;
    if (m->field_kind == FildeshSxprotoFieldKind_MESSAGE ||
        m->field_kind == FildeshSxprotoFieldKind_LONEOF)
    {
      NEWLINE_INDENT(indent_level);
      print_quoted_sxpb_str_FildeshO(out, e->text);
      if (e->field_kind == FildeshSxprotoFieldKind_MESSAGE ||
          e->field_kind == FildeshSxprotoFieldKind_LONEOF)
      {
        if (fildesh_nullid(e->elem)) {
          putstrlit_FildeshO(out, ": {}");
        }
        else {
          putstrlit_FildeshO(out, ": {");
          write_json_FildeshO(out, sxpb, sub_it, indent_level+1);
          NEWLINE_INDENT(indent_level);
          putc_FildeshO(out, '}');
        }
      }
      else if (e->field_kind == FildeshSxprotoFieldKind_ARRAY ||
               e->field_kind == FildeshSxprotoFieldKind_MANYOF)
      {
        putstrlit_FildeshO(out, ": [");
        write_json_FildeshO(out, sxpb, sub_it, indent_level);
        putc_FildeshO(out, ']');
      }
      else {
        assert(!fildesh_nullid(e->elem));
        putstrlit_FildeshO(out, ": ");
        print_json_literal_value_FildeshO(out, &(*sxpb->values)[e->elem]);
      }
    }
    else if (m->field_kind == FildeshSxprotoFieldKind_ARRAY) {
      if (e->field_kind == FildeshSxprotoFieldKind_MESSAGE ||
          e->field_kind == FildeshSxprotoFieldKind_LONEOF)
      {
        putc_FildeshO(out, '{');
        write_json_FildeshO(out, sxpb, sub_it, indent_level+1);
        NEWLINE_INDENT(indent_level);
        putc_FildeshO(out, '}');
        if (pending_comma) {
          pending_comma = false;
          putstrlit_FildeshO(out, ", ");
        }
      }
      else {
        print_json_literal_value_FildeshO(out, e);
        if (pending_comma) {
          pending_comma = false;
          putstrlit_FildeshO(out, ", ");
        }
      }
    }
    else {
      assert(m->field_kind == FildeshSxprotoFieldKind_MANYOF);
      putc_FildeshO(out, '{');
      NEWLINE_INDENT(indent_level+1);
      if (e->field_kind == FildeshSxprotoFieldKind_MESSAGE) {
        print_quoted_sxpb_str_FildeshO(out, e->text);
        if (fildesh_nullid(e->elem)) {
          putstrlit_FildeshO(out, ": {}");
        }
        else {
          putstrlit_FildeshO(out, ": {");
          write_json_FildeshO(out, sxpb, sub_it, indent_level+2);
          NEWLINE_INDENT(indent_level+1);
          putc_FildeshO(out, '}');
        }
      }
      else if (e->field_kind == FildeshSxprotoFieldKind_ARRAY ||
               e->field_kind == FildeshSxprotoFieldKind_MANYOF)
      {
        print_quoted_sxpb_str_FildeshO(out, e->text);
        putstrlit_FildeshO(out, ": [");
        write_json_FildeshO(out, sxpb, sub_it, indent_level+1);
        putc_FildeshO(out, ']');
      }
      else {
        if (fildesh_nullid(e->elem)) {
          putstrlit_FildeshO(out, "\"value\": ");
          print_json_literal_value_FildeshO(out, e);
        }
        else {
          assert(e->field_kind == FildeshSxprotoFieldKind_LITERAL);
          print_quoted_sxpb_str_FildeshO(out, e->text);
          putstrlit_FildeshO(out, ": ");
          print_json_literal_value_FildeshO(out, &(*sxpb->values)[e->elem]);
        }
      }

      NEWLINE_INDENT(indent_level);
      putc_FildeshO(out, '}');
      if (pending_comma) {
        pending_comma = false;
        putstrlit_FildeshO(out, ", ");
      }
    }
    if (pending_comma) {
      putc_FildeshO(out, ',');
    }
    it.elem_id = e->next;
  }
}

  void
print_json_FildeshO(FildeshO* out, FildeshSxpb* sxpb)
{
  const FildeshSxpbIT top_it = top_of_FildeshSxpb(sxpb);
  const FildeshSxprotoFieldKind top_kind = top_it.field_kind;
  const bool on_message = (top_kind == FildeshSxprotoFieldKind_MESSAGE);

  putc_FildeshO(out, on_message ? '{' : '[');
  write_json_FildeshO(
      out, sxpb,
      top_it,
      on_message ? 1 : 0);
  if (on_message) {putc_FildeshO(out, '\n');}
  putc_FildeshO(out, on_message ? '}' : ']');
  putc_FildeshO(out, '\n');
}

