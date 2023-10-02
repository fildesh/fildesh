#include "src/sxproto/value.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>

static
  void
write_txtpb_literal(FildeshO* out, const FildeshSxprotoValue* e)
{
  if (e->field_kind == FildeshSxprotoFieldKind_LITERAL_STRING) {
    size_t i;
    putc_FildeshO(out, '"');
    for (i = 0; e->text[i] != '\0'; ++i) {
      switch (e->text[i]) {
        case '"':   puts_FildeshO(out, "\\\"");  break;
        case '\\':  puts_FildeshO(out, "\\\\");  break;
        case '\n':  puts_FildeshO(out, "\\n");  break;
        default:    putc_FildeshO(out, e->text[i]);  break;
      }
    }
    putc_FildeshO(out, '"');
  }
  else if (e->text[0] == '+') {
    puts_FildeshO(out, &e->text[1]);
  }
  else {
    puts_FildeshO(out, e->text);
  }
}

static void
write_txtpb_FildeshO(
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
    FildeshSxpbIT sub_it = DEFAULT_FildeshSxpbIT;
    sub_it.cons_id = it.elem_id;
    if (m->field_kind == FildeshSxprotoFieldKind_MESSAGE) {
      NEWLINE_INDENT(indent_level);
      puts_FildeshO(out, e->text);
      if (e->field_kind == FildeshSxprotoFieldKind_MESSAGE) {
        if (fildesh_nullid(e->elem)) {
          puts_FildeshO(out, " {}");
        }
        else {
          puts_FildeshO(out, " {");
          write_txtpb_FildeshO(out, sxpb, sub_it, indent_level+1);
          NEWLINE_INDENT(indent_level);
          putc_FildeshO(out, '}');
        }
      }
      else if (e->field_kind == FildeshSxprotoFieldKind_ARRAY) {
        puts_FildeshO(out, ": [");
        write_txtpb_FildeshO(out, sxpb, sub_it, indent_level);
        putc_FildeshO(out, ']');
      }
      else if (e->field_kind == FildeshSxprotoFieldKind_MANYOF) {
        puts_FildeshO(out, " {values: [");
        write_txtpb_FildeshO(out, sxpb, sub_it, indent_level);
        puts_FildeshO(out, "]}");
      }
      else {
        assert(!fildesh_nullid(e->elem));
        puts_FildeshO(out, ": ");
        write_txtpb_literal(out, &(*sxpb->values)[e->elem]);
      }
    }
    else if (m->field_kind == FildeshSxprotoFieldKind_ARRAY) {
      if (e->field_kind == FildeshSxprotoFieldKind_MESSAGE) {
        putc_FildeshO(out, '{');
        write_txtpb_FildeshO(out, sxpb, sub_it, indent_level+1);
        NEWLINE_INDENT(indent_level);
        if (fildesh_nullid(e->next)) {
          putc_FildeshO(out, '}');
        }
        else {
          puts_FildeshO(out, "}, ");
        }
      }
      else {
        write_txtpb_literal(out, e);
        if (!fildesh_nullid(e->next)) {
          puts_FildeshO(out, ", ");
        }
      }
    }
    else {
      assert(m->field_kind == FildeshSxprotoFieldKind_MANYOF);
      putc_FildeshO(out, '{');
      NEWLINE_INDENT(indent_level+1);
      if (e->field_kind == FildeshSxprotoFieldKind_MESSAGE) {
        puts_FildeshO(out, e->text);
        if (fildesh_nullid(e->elem)) {
          puts_FildeshO(out, " {}");
        }
        else {
          puts_FildeshO(out, " {");
          write_txtpb_FildeshO(out, sxpb, sub_it, indent_level+2);
          NEWLINE_INDENT(indent_level+1);
          putc_FildeshO(out, '}');
        }
      }
      else if (e->field_kind == FildeshSxprotoFieldKind_ARRAY) {
        puts_FildeshO(out, e->text);
        puts_FildeshO(out, ": [");
        write_txtpb_FildeshO(out, sxpb, sub_it, indent_level+1);
        putc_FildeshO(out, ']');
      }
      else if (e->field_kind == FildeshSxprotoFieldKind_MANYOF) {
        puts_FildeshO(out, e->text);
        puts_FildeshO(out, " {values: [");
        write_txtpb_FildeshO(out, sxpb, sub_it, indent_level+1);
        puts_FildeshO(out, "]}");
      }
      else {
        if (fildesh_nullid(e->elem)) {
          puts_FildeshO(out, "value: ");
          write_txtpb_literal(out, e);
        }
        else {
          assert(e->field_kind == FildeshSxprotoFieldKind_LITERAL);
          puts_FildeshO(out, e->text);
          puts_FildeshO(out, ": ");
          write_txtpb_literal(out, &(*sxpb->values)[e->elem]);
        }
      }

      NEWLINE_INDENT(indent_level);
      if (fildesh_nullid(e->next)) {
        putc_FildeshO(out, '}');
      }
      else {
        puts_FildeshO(out, "}, ");
      }
    }
    it.elem_id = e->next;
  }
}

  void
print_txtpb_FildeshO(FildeshO* out, FildeshSxpb* sxpb)
{
  write_txtpb_FildeshO(
      out, sxpb,
      top_of_FildeshSxpb(sxpb),
      0);
  putc_FildeshO(out, '\n');
}

/* Leaves output files open.*/
/* Deprecated. Use print_txtpb_FildeshO().*/
bool sxproto2textproto(FildeshX* in, FildeshO* out, FildeshO* err_out)
{
  FildeshSxpb* sxpb = slurp_sxpb_close_FildeshX(in, NULL, err_out);
  if (!sxpb) {
    return false;
  }
  print_txtpb_FildeshO(out, sxpb);
  close_FildeshSxpb(sxpb);
  return true;
}
