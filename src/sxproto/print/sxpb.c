#include <assert.h>
#include <string.h>

#include "src/sxproto/print/value.h"
#include "src/sxproto/syntax.h"

static void write_sxpb_named_field_FildeshO(
    FildeshO* out,
    const FildeshSxpb* sxpb,
    FildeshSxpbIT it);
static void write_sxpb_entries_FildeshO(
    FildeshO* out,
    const FildeshSxpb* sxpb,
    FildeshSxpbIT it,
    char separator,
    bool separate_prefix_on,
    FildeshSxprotoFieldKind parent_kind);

static
  void
print_sxpb_atom_FildeshO(FildeshO* out, const char* s)
{
  if (is_sxpb_bare_atom(s, strlen(s))) {
    putstr_FildeshO(out, s);
  }
  else {
    print_quoted_sxpb_str_FildeshO(out, s);
  }
}

static
  bool
is_sxpb_nest_string_words(const char* s)
{
  const size_t n = strlen(s);
  bool has_space = false;
  bool is_word_start = true;
  size_t i;
  for (i = 0; i < n; ++i) {
    if (s[i] == ' ') {
      if (is_word_start) {return false;}
      has_space = true;
      is_word_start = true;
    }
    else {
      if (is_sxpb_delim(s[i])) {return false;}
      if (is_word_start && !has_sxpb_bare_prefix(&s[i], n-i)) {return false;}
      is_word_start = false;
    }
  }
  return has_space && !is_word_start;
}

static
  void
write_sxpb_literal_FildeshO(
    FildeshO* out,
    const FildeshSxpb* sxpb,
    FildeshSxpbIT it)
{
  const FildeshSxprotoValue* const e = literal_value_at_FildeshSxpb(sxpb, it);
  if (e->field_kind == FildeshSxprotoFieldKind_LITERAL_STRING) {
    print_sxpb_atom_FildeshO(out, e->text);
  }
  else {
    print_sxpb_literal_value_FildeshO(out, e);
  }
}

static
  void
write_sxpb_array_entry_FildeshO(
    FildeshO* out,
    const FildeshSxpb* sxpb,
    FildeshSxpbIT it)
{
  const FildeshSxprotoValue* const e = &(*sxpb->values)[id_of_FildeshSxpbIT(it)];
  if (e->field_kind == FildeshSxprotoFieldKind_MESSAGE) {
    putc_FildeshO(out, '(');
    if (!nullish_FildeshSxpbIT(first_at_FildeshSxpb(sxpb, it))) {
      putstrlit_FildeshO(out, "()");
      write_sxpb_entries_FildeshO(
          out, sxpb, it, ' ', true, FildeshSxprotoFieldKind_MESSAGE);
    }
    putc_FildeshO(out, ')');
  }
  else {
    assert(is_literal_FildeshSxprotoFieldKind(e->field_kind));
    write_sxpb_literal_FildeshO(out, sxpb, it);
  }
}

static
  void
write_sxpb_manyof_entry_FildeshO(
    FildeshO* out,
    const FildeshSxpb* sxpb,
    FildeshSxpbIT it)
{
  const FildeshSxprotoValue* const e = &(*sxpb->values)[id_of_FildeshSxpbIT(it)];
  if (is_literal_FildeshSxprotoFieldKind(e->field_kind)) {
    write_sxpb_literal_FildeshO(out, sxpb, it);
  }
  else {
    write_sxpb_named_field_FildeshO(out, sxpb, it);
  }
}

static
  void
write_sxpb_nest_entry_FildeshO(
    FildeshO* out,
    const FildeshSxpb* sxpb,
    FildeshSxpbIT it)
{
  const FildeshSxprotoValue* const e = &(*sxpb->values)[id_of_FildeshSxpbIT(it)];
  if (e->field_kind == FildeshSxprotoFieldKind_NEST) {
    const char* const name = name_at_FildeshSxpb(sxpb, it);
    FildeshSxpbIT child_it = first_at_FildeshSxpb(sxpb, it);
    const FildeshSxprotoValue* child_e = NULL;
    if (!nullish_FildeshSxpbIT(child_it)) {
      child_e = literal_value_at_FildeshSxpb(sxpb, child_it);
    }
    putc_FildeshO(out, '(');
    print_sxpb_atom_FildeshO(out, name);
    if (name[0] == '\0') {
      putstrlit_FildeshO(out, " (\"\")");
      write_sxpb_entries_FildeshO(
          out, sxpb, it, ' ', true, FildeshSxprotoFieldKind_NEST);
    }
    else if (
        !nullish_FildeshSxpbIT(child_it) &&
        nullish_FildeshSxpbIT(next_at_FildeshSxpb(sxpb, child_it)) &&
        child_e->field_kind == FildeshSxprotoFieldKind_LITERAL_STRING &&
        is_sxpb_nest_string_words(child_e->text))
    {
      putstrlit_FildeshO(out, " \"\" ");
      putstr_FildeshO(out, child_e->text);
    }
    else {
      write_sxpb_entries_FildeshO(
          out, sxpb, it, ' ', true, FildeshSxprotoFieldKind_NEST);
    }
    putc_FildeshO(out, ')');
  }
  else if (literal_value_at_FildeshSxpb(sxpb, it)->text[0] == '\0') {
    putstrlit_FildeshO(out, "(\"\" \"\")");
  }
  else {
    write_sxpb_literal_FildeshO(out, sxpb, it);
  }
}

static
  void
write_sxpb_entries_FildeshO(
    FildeshO* out,
    const FildeshSxpb* sxpb,
    FildeshSxpbIT it,
    char separator,
    bool separate_prefix_on,
    FildeshSxprotoFieldKind parent_kind)
{
  bool first = true;
  for (it = first_at_FildeshSxpb(sxpb, it);
       !nullish_FildeshSxpbIT(it);
       it = next_at_FildeshSxpb(sxpb, it))
  {
    if (separate_prefix_on || !first) {
      putc_FildeshO(out, separator);
    }
    switch (parent_kind) {
      case FildeshSxprotoFieldKind_ARRAY:
        write_sxpb_array_entry_FildeshO(out, sxpb, it);
        break;
      case FildeshSxprotoFieldKind_MANYOF:
        write_sxpb_manyof_entry_FildeshO(out, sxpb, it);
        break;
      case FildeshSxprotoFieldKind_NEST:
        write_sxpb_nest_entry_FildeshO(out, sxpb, it);
        break;
      default:
        write_sxpb_named_field_FildeshO(out, sxpb, it);
        break;
    }
    first = false;
  }
}

static
  void
write_sxpb_field_body_FildeshO(
    FildeshO* out,
    const FildeshSxpb* sxpb,
    FildeshSxpbIT it,
    FildeshSxprotoFieldKind kind)
{
  if (is_literal_FildeshSxprotoFieldKind(kind)) {
    putc_FildeshO(out, ' ');
    write_sxpb_literal_FildeshO(out, sxpb, it);
    return;
  }

  if (kind == FildeshSxprotoFieldKind_ARRAY) {
    putstrlit_FildeshO(out, " (())");
  }
  else if (kind == FildeshSxprotoFieldKind_DICT) {
    putstrlit_FildeshO(out, " ()");
  }
  else if (kind == FildeshSxprotoFieldKind_NEST) {
    putstrlit_FildeshO(out, " (\"\")");
  }
  write_sxpb_entries_FildeshO(out, sxpb, it, ' ', true, kind);
}

static
  void
write_sxpb_loneof_field_FildeshO(
    FildeshO* out,
    const FildeshSxpb* sxpb,
    FildeshSxpbIT it)
{
  const char* const name = name_at_FildeshSxpb(sxpb, it);
  FildeshSxpbIT child_it = first_at_FildeshSxpb(sxpb, it);
  const FildeshSxprotoValue* child_e;
  assert(!nullish_FildeshSxpbIT(child_it));
  child_e = &(*sxpb->values)[id_of_FildeshSxpbIT(child_it)];
  putstrlit_FildeshO(out, "((");
  print_sxpb_atom_FildeshO(out, name);
  putc_FildeshO(out, ' ');
  print_sxpb_atom_FildeshO(out, name_at_FildeshSxpb(sxpb, child_it));
  putc_FildeshO(out, ')');
  write_sxpb_field_body_FildeshO(out, sxpb, child_it, child_e->field_kind);
  putc_FildeshO(out, ')');
}

static
  void
write_sxpb_named_field_FildeshO(
    FildeshO* out,
    const FildeshSxpb* sxpb,
    FildeshSxpbIT it)
{
  const FildeshSxprotoValue* const e = &(*sxpb->values)[id_of_FildeshSxpbIT(it)];
  const char* const name = name_at_FildeshSxpb(sxpb, it);
  if (e->field_kind == FildeshSxprotoFieldKind_LONEOF) {
    write_sxpb_loneof_field_FildeshO(out, sxpb, it);
    return;
  }
  if (e->field_kind == FildeshSxprotoFieldKind_MANYOF) {
    putstrlit_FildeshO(out, "((");
    print_sxpb_atom_FildeshO(out, name);
    putc_FildeshO(out, ')');
    write_sxpb_field_body_FildeshO(out, sxpb, it, e->field_kind);
    putc_FildeshO(out, ')');
    return;
  }

  putc_FildeshO(out, '(');
  print_sxpb_atom_FildeshO(out, name);
  write_sxpb_field_body_FildeshO(out, sxpb, it, e->field_kind);
  putc_FildeshO(out, ')');
}

  void
print_sxpb_FildeshO(FildeshO* out, FildeshSxpb* sxpb)
{
  FildeshSxpbIT top_it = top_of_FildeshSxpb(sxpb);
  const FildeshSxprotoValue* const e = &(*sxpb->values)[id_of_FildeshSxpbIT(top_it)];
  bool separate_prefix_on = true;
  if (e->field_kind == FildeshSxprotoFieldKind_NEST) {
    putstrlit_FildeshO(out, "(\"\")");
  }
  else if (e->field_kind == FildeshSxprotoFieldKind_ARRAY ||
           e->field_kind == FildeshSxprotoFieldKind_MANYOF) {
    putstrlit_FildeshO(out, "(())");
  }
  else if (e->field_kind == FildeshSxprotoFieldKind_DICT) {
    putstrlit_FildeshO(out, "()");
  }
  else {
    separate_prefix_on = false;
  }
  write_sxpb_entries_FildeshO(
      out, sxpb, top_it, '\n', separate_prefix_on, e->field_kind);
  putc_FildeshO(out, '\n');
}
