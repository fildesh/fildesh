#include <fildesh/sxproto.h>

#include <assert.h>
#include <stdio.h>
#include <string.h>

const char sxproto_delim_bytes[] = " \n\t\r\v\"();";
const char sxproto_blank_bytes[] = " \n\t\r\v";

typedef struct SxprotoPosition SxprotoPosition;
typedef struct SxprotoSeparation SxprotoSeparation;
typedef struct SxprotoAST SxprotoAST;

struct SxprotoPosition {
  unsigned line_count;
  unsigned column_count;
  FildeshO* err_out;
};

struct SxprotoSeparation {
  bool is_line_comment;
  const char* text;
  SxprotoSeparation* next;
};

enum SxprotoAstType {
  SXPROTO_AST_MESSAGE,
  SXPROTO_AST_SCALAR,
  SXPROTO_AST_REPEATED,
  SXPROTO_AST_MANYOF,
  SXPROTO_AST_END  /* May appear at end of list.*/
};
typedef enum SxprotoAstType SxprotoAstType;

struct SxprotoAST {
  SxprotoSeparation* preseparation;
  const char* text;
  SxprotoAstType ast_type;
  SxprotoAST* elem;
  SxprotoAST* next;
};


static SxprotoSeparation* new_SxprotoSeparation(FildeshAlloc* alloc)
{
  SxprotoSeparation* g = fildesh_allocate(SxprotoSeparation, 1, alloc);
  g->is_line_comment = false;
  g->text = NULL;
  g->next = NULL;
  return g;
}

static SxprotoAST* new_SxprotoAST(FildeshAlloc* alloc)
{
  SxprotoAST* a = fildesh_allocate(SxprotoAST, 1, alloc);
  a->preseparation = NULL;
  a->text = NULL;
  a->ast_type = SXPROTO_AST_END;
  a->elem = NULL;
  a->next = NULL;
  return a;
}

static void
update_SxprotoPosition(SxprotoPosition* pos, const FildeshX* slice)
{
  const char* s = (char*)memchr(slice->at, '\n', slice->size);
  pos->column_count += slice->size;
  while (s) {
    size_t n = &slice->at[slice->size] - s;
    pos->line_count += 1;
    pos->column_count = n;
    s = (char*)memchr(&s[1], '\n', n-1);
  }
}

static void
syntax_error(const SxprotoPosition* pos, const char* msg)
{
  FildeshO* const out = pos->err_out;
  puts_FildeshO(out, "Line ");
  print_int_FildeshO(out, (int)(pos->line_count+1));
  puts_FildeshO(out, " column ");
  print_int_FildeshO(out, (int)(pos->column_count+1));
  puts_FildeshO(out, ": ");
  puts_FildeshO(out, msg);
  putc_FildeshO(out, '\n');
}

  static void
parse_separation(
    FildeshX* in, SxprotoAST* a, SxprotoPosition* pos, FildeshAlloc* alloc)
{
  FildeshX slice;
  SxprotoSeparation* result = NULL;
  SxprotoSeparation* g = NULL;

  for (slice = while_chars_FildeshX(in, sxproto_blank_bytes);
       slice.size > 0 || peek_char_FildeshX(in, ';');
       slice = while_chars_FildeshX(in, sxproto_blank_bytes))
  {
    SxprotoSeparation* next = new_SxprotoSeparation(alloc);
    if (g) {g->next = next;}
    else {result = next;}
    g = next;

    if (slice.size == 0) {
      g->is_line_comment = true;
      slice = sliceline_FildeshX(in);
      assert(slice.size > 0);
      slice.at = &slice.at[1];
      slice.size -= 1;
      pos->line_count += 1;
      pos->column_count = 0;
    }
    else {
      update_SxprotoPosition(pos, &slice);
    }
    g->text = strdup_FildeshX(&slice, alloc);
  }

  if (!a->preseparation) {
    a->preseparation = result;
  }
  else {
    /* Use `g` as iteration.*/
    g = a->preseparation;
    while (g->next) {
      g = g->next;
    }
    g->next = result;
  }
}

static
  SxprotoAST*
elem_after_separation(
    FildeshX* in, SxprotoAST* a, SxprotoPosition* pos, FildeshAlloc* alloc)
{
  a->elem = new_SxprotoAST(alloc);
  parse_separation(in, a->elem, pos, alloc);
  if (skipstr_FildeshX(in, ")")) {
    pos->column_count += 1;
    return NULL;
  }
  return a->elem;
}

  static SxprotoAST*
next_after_separation(
    FildeshX* in, SxprotoAST* a, SxprotoPosition* pos, FildeshAlloc* alloc)
{
  a->next = new_SxprotoAST(alloc);
  parse_separation(in, a->next, pos, alloc);
  if (skipstr_FildeshX(in, ")")) {
    pos->column_count += 1;
    return NULL;
  }
  return a->next;
}

  static bool
parse_open_double_quote(
    FildeshX* in, SxprotoAST* a, SxprotoPosition* pos, FildeshAlloc* alloc)
{
  FildeshX line_slice = until_chars_FildeshX(in, "\n");
  FildeshX slice;

  assert(line_slice.at[0] == '"');
  line_slice.off = 1;

  for (slice = until_chars_FildeshX(&line_slice, "\\\"");
       line_slice.off < line_slice.size && line_slice.at[line_slice.off] != '"';
       slice = until_chars_FildeshX(in, "\\\""))
  {
    line_slice.off += 1;
    if (line_slice.off > line_slice.size) {
      line_slice.off += 1;
    }
  }

  pos->column_count += line_slice.off;
  if (line_slice.off == line_slice.size) {
    syntax_error(pos, "Expected closing double quote.");
    return false;
  }

  /* Skip double quote.*/
  line_slice.off += 1;
  pos->column_count += 1;

  slice = line_slice;
  slice.off = 0;
  slice.size = line_slice.off;
  a->text = strdup_FildeshX(&slice, alloc);
  a->ast_type = SXPROTO_AST_SCALAR;

  in->off -= (line_slice.size - line_slice.off);
  return true;
}

  static bool
parse_after_open_paren(
    FildeshX* in, SxprotoAST* a, SxprotoPosition* pos, FildeshAlloc* alloc)
{
  FildeshX slice;
  SxprotoAST* const result = a;
  SxprotoAstType consistent_ast_type = SXPROTO_AST_END;
  pos->column_count += 1;
  parse_separation(in, result, pos, alloc);

  if (skipstr_FildeshX(in, "(")) {
    pos->column_count += 1;
    parse_separation(in, result, pos, alloc);

    slice = until_chars_FildeshX(in, sxproto_delim_bytes);
    if (slice.size == 0 && skipstr_FildeshX(in, "(")) {
      pos->column_count += 1;
      parse_separation(in, result, pos, alloc);

      slice = until_chars_FildeshX(in, sxproto_delim_bytes);
      if (slice.size == 0) {
        syntax_error(pos, "Expected manyof name.");
        return false;
      }
      update_SxprotoPosition(pos, &slice);
      result->ast_type = SXPROTO_AST_MANYOF;
      consistent_ast_type  = SXPROTO_AST_MANYOF;
      result->text = strdup_FildeshX(&slice, alloc);
      parse_separation(in, result, pos, alloc);
      if (!skipstr_FildeshX(in, ")")) {
        syntax_error(pos, "Expected closing paren for manyof.");
        return false;
      }
      pos->column_count += 1;
    }
    else if (slice.size > 0) {
      update_SxprotoPosition(pos, &slice);
      result->ast_type = SXPROTO_AST_REPEATED;
      result->text = strdup_FildeshX(&slice, alloc);
      parse_separation(in, result, pos, alloc);
    }
    else {
      result->ast_type = SXPROTO_AST_MESSAGE;
      consistent_ast_type  = SXPROTO_AST_MESSAGE;
    }

    if (!skipstr_FildeshX(in, ")")) {
      syntax_error(pos, "Expected closing paren.");
      return false;
    }
    pos->column_count += 1;
  }
  else {
    slice = until_chars_FildeshX(in, sxproto_delim_bytes);
    if (slice.size == 0) {
      syntax_error(pos, "Expected field name.");
      return false;
    }
    update_SxprotoPosition(pos, &slice);
    result->text = strdup_FildeshX(&slice, alloc);
    /* It's unknown right now.*/
    assert(result->ast_type == SXPROTO_AST_END);
  }

  for (a = elem_after_separation(in, a, pos, alloc);
       a;
       a = next_after_separation(in, a, pos, alloc))
  {
    if (skipstr_FildeshX(in, "(")) {
      if (consistent_ast_type == SXPROTO_AST_END) {
        consistent_ast_type = SXPROTO_AST_MESSAGE;
      }
      if (consistent_ast_type == SXPROTO_AST_SCALAR) {
        syntax_error(pos, "Expected a scalar but got a field.");
        return false;
      }

      if (!parse_after_open_paren(in, a, pos, alloc)) {
        return false;
      }
    }
    else if (peek_char_FildeshX(in, '"')) {
      if (consistent_ast_type == SXPROTO_AST_END) {
        consistent_ast_type = SXPROTO_AST_SCALAR;
      }
      if (consistent_ast_type == SXPROTO_AST_MESSAGE) {
        syntax_error(pos, "Expected a field but got a string.");
        return false;
      }

      if (!parse_open_double_quote(in, a, pos, alloc)) {
        return false;
      }
    }
    else {
      if (consistent_ast_type == SXPROTO_AST_END) {
        consistent_ast_type = SXPROTO_AST_SCALAR;
      }
      if (consistent_ast_type == SXPROTO_AST_MESSAGE) {
        syntax_error(pos, "Expected a field but got a scalar.");
        return false;
      }

      slice = until_chars_FildeshX(in, sxproto_delim_bytes);
      if (slice.size == 0) {
        syntax_error(pos, "Expected close paren but got EOF.");
        return false;
      }
      a->text = strdup_FildeshX(&slice, alloc);
      a->ast_type = SXPROTO_AST_SCALAR;
    }
  }
  if (result->ast_type == SXPROTO_AST_END) {
    if (consistent_ast_type == SXPROTO_AST_END) {
      consistent_ast_type = SXPROTO_AST_MESSAGE;
    }
    result->ast_type = consistent_ast_type;
  }

  return true;
}

static
  SxprotoAST*
parse_sxproto(FildeshX* in, FildeshAlloc* alloc, FildeshO* err_out)
{
  SxprotoAST sentinel;
  SxprotoAST* a;
  SxprotoPosition pos[1] = {{0, 0, NULL}};
  pos->err_out = err_out;
  for (a = elem_after_separation(in, &sentinel, pos, alloc);
       a;
       a = next_after_separation(in, a, pos, alloc))
  {
    if (!skipstr_FildeshX(in, "(")) {
      const char* line = getline_FildeshX(in);
      if (!line) {
        /* Successful parse!*/
        return sentinel.elem;
      }
      syntax_error(pos, "Expected open paren to start message but got:");
      syntax_error(pos, line);
      return NULL;
    }
    if (!parse_after_open_paren(in, a, pos, alloc)) {
      return NULL;
    }
  }
  syntax_error(pos, "Got extra closing paren at top level.");
  return NULL;
}

  static void
translate_SxprotoSeparation(
    const SxprotoSeparation* g,
    const char* postpreseparation,
    FildeshO* out)
{
  while (g) {
    if (g->is_line_comment) {
      putc_FildeshO(out, '#');
      puts_FildeshO(out, g->text);
      putc_FildeshO(out, '\n');
    } else if (g->text && g->text[0]) {
      puts_FildeshO(out, g->text);
    } else {
      putc_FildeshO(out, ' ');
    }
    g = g->next;
  }
  if (postpreseparation) {
    puts_FildeshO(out, postpreseparation);
  }
}

  static void
translate_SxprotoAST(
    const SxprotoAST* a,
    const char* postpreseparation,
    FildeshO* out)
{
  translate_SxprotoSeparation(a->preseparation, postpreseparation, out);
  if (a->ast_type == SXPROTO_AST_REPEATED) {
    assert(a->text);
    puts_FildeshO(out, a->text);
    puts_FildeshO(out, ": [");
    a = a->elem;
    while (a) {
      assert(a->ast_type != SXPROTO_AST_REPEATED);
      if (a->ast_type == SXPROTO_AST_MESSAGE) {
        assert(!a->text);
      }
      else {
        assert(a->ast_type != SXPROTO_AST_REPEATED);
        assert(!a->elem);
      }
      translate_SxprotoAST(a, NULL, out);
      if (a->next && a->next->ast_type != SXPROTO_AST_END) {
        putc_FildeshO(out, ',');
      }
      a = a->next;
    }
    puts_FildeshO(out, "]");
  }
  else if (a->ast_type == SXPROTO_AST_MANYOF) {
    assert(a->text);
    puts_FildeshO(out, a->text);
    puts_FildeshO(out, " {values: [");
    a = a->elem;
    while (a) {
      if (a->ast_type == SXPROTO_AST_MESSAGE ||
          a->ast_type == SXPROTO_AST_SCALAR ||
          a->ast_type == SXPROTO_AST_MANYOF)
      {
        if (!a->text || (a->ast_type == SXPROTO_AST_SCALAR && !a->elem)) {
          translate_SxprotoAST(a, "{value: ", out);
        }
        else {
          translate_SxprotoAST(a, "{", out);
        }
        putc_FildeshO(out, '}');
      }
      else {
        assert(a->ast_type == SXPROTO_AST_END);
        assert(!a->elem);
        assert(!a->next);
        translate_SxprotoAST(a, NULL, out);
      }
      if (a->next && a->next->ast_type != SXPROTO_AST_END) {
        putc_FildeshO(out, ',');
      }
      a = a->next;
    }
    puts_FildeshO(out, "]}");
  }
  else if (a->ast_type == SXPROTO_AST_MESSAGE) {
    if (a->text) {
      puts_FildeshO(out, a->text);
      putc_FildeshO(out, ' ');
    }
    putc_FildeshO(out, '{');
    a = a->elem;
    while (a) {
      translate_SxprotoAST(a, NULL, out);
      a = a->next;
    }
    putc_FildeshO(out, '}');
  }
  else if (a->ast_type == SXPROTO_AST_SCALAR) {
    puts_FildeshO(out, a->text);
    if (a->elem) {
      putc_FildeshO(out, ':');
    }
    a = a->elem;
    while (a) {
      assert(a->ast_type == SXPROTO_AST_SCALAR || a->ast_type == SXPROTO_AST_END);
      assert(!a->elem);
      translate_SxprotoAST(a, NULL, out);
      a = a->next;
    }
  }
  else {
    assert(a->ast_type == SXPROTO_AST_END);
  }
}


/* Leaves output files open.*/
bool sxproto2textproto(FildeshX* in, FildeshO* out, FildeshO* err_out)
{
  FildeshAlloc* alloc = open_FildeshAlloc();
  SxprotoAST* result;
  SxprotoAST* a;

  result = parse_sxproto(in, alloc, err_out);
  close_FildeshX(in);
  for (a = result; a; a = a->next) {
    translate_SxprotoAST(a, NULL, out);
  }
  close_FildeshAlloc(alloc);
  return !!result;
}
