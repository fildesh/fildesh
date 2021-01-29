/**
 * Make a switch statement on strings.
 *
 * Usage example:
 *   cswitch strvbl -x fnlist.txt -case-pfx 'fn = testfn_' -o switch.c
 *
 * Then you can #include "switch.c" from within a function to effectvely
 * have a switch statement like:
 *   switch (strvbl) {
 *     case "line1txt": fn = testfn_line1txt; break;
 *     case "line2txt": fn = testfn_line2txt; break;
 *     ...
 *     case "lineNtxt": fn = testfn_lineNtxt; break;
 *   }
 *
 * Hint: The fnlist.txt file might be generated like:
 *   grep -o -e 'testfn_[^ (]*' mytests.c | sed -e 's/^testfn_//' > fnlist.txt
 **/

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static
int eq_cstr (const char* a, const char* b)
{ return 0 == strcmp(a ,b); }

static
int cmp_cstr_ptr (const void* a, const void* b)
{ return strcmp(*(const char* const*)a, *(const char* const*) b); }

#define BailOut(ret, msg) \
  do { \
    fprintf (stderr, "%s\n", msg); \
    fprintf (stderr, "Usage: %s strname -x list.txt -case-pfx 'fn = testfn_' -o switch.h\n", argv[0]); \
    return ret; \
  } while (0)

/** Read the full text of a file.**/
static
  char*
read_text_file(const char* filename)
{
  typedef struct Chunk Chunk;
  struct Chunk {
    char buf[BUFSIZ];
    Chunk* prev;
  };
  const size_t bufsz = BUFSIZ;
  FILE* in = fopen(filename, "rb");
  Chunk* chunk = 0;
  size_t nchunks = 0;
  char* text;

  if (!in)  return 0;

  while (1) {
    size_t off = 0;
    Chunk* prev = chunk;

    chunk = (Chunk*) malloc(sizeof(Chunk));
    chunk->prev = prev;
    memset(chunk->buf, 0, bufsz * sizeof(char));
    nchunks += 1;

    while (off < bufsz) {
      size_t sz =
        fread(&chunk->buf[off], sizeof(char), bufsz - off, in);
      if (sz == 0)  break;
      off += sz;
    }

    if (off < bufsz)  break;
  }
  fclose(in);

  text = (char*) malloc(nchunks * bufsz * sizeof(char));
  while (nchunks > 0) {
    Chunk* tmp = chunk;
    nchunks -= 1;
    memcpy(&text[nchunks * bufsz], chunk->buf, bufsz * sizeof(char));
    chunk = chunk->prev;
    free(tmp);
  }
  return text;
}

/** Break up lines of text.**/
static
  const char**
sep_text_lines(char* text, size_t* ret_nlines)
{
  static const char delims[] = "\r\n";
  size_t nlines = 0;
  const char** lines;
  char* line;
  size_t i;

  line = text;
  while (line[1]) {
    line = &line[strcspn(line, delims)];
    if (line[0])  nlines += 1;
    line = &line[strspn(line, delims)];
  }

  lines = (const char**) malloc(nlines * sizeof(line));
  line = text;
  for (i = 0; i < nlines; ++i) {
    line = &line[strspn(line, delims)];
    lines[i] = line;
    line = &line[strcspn(line, delims)];
    line[0] = '\0';
    line = &line[1];
  }
  *ret_nlines = nlines;
  return lines;
}

/** Write an escaped character in some reasonable way.**/
static
  void
write_escaped_char (FILE* out, char c)
{
  if (isalnum(c)) {
    fputc (c, out);
    return;
  }
  switch (c) {
  case ' ':
  case '_':
    fputc (c, out);
    break;
  case '\n':
    fputs ("\\n", out);
    break;
  case '\r':
    fputs ("\\r", out);
    break;
  case '\t':
    fputs ("\\t", out);
    break;
  default:
    fprintf(out, "\\x%02X", (unsigned int) (unsigned char) c);
    break;
  }
}

static
  void
write_switch (FILE* out, const char* strname, size_t depth)
{
  if (depth > 0)  fputc ('\n', out);
  fprintf(out, "switch (%s[%lu]) {", strname, depth);
}

static
  void
write_case (FILE* out, char c)
{
  fputs("\ncase '", out);
  write_escaped_char(out, c);
  fputs("':", out);
}

static
  void
write_end_switch (FILE* out, size_t depth)
{
  fputs ("\ndefault: break;\n}", out);
  if (depth > 0)  fputs ("break;", out);
  else            fputc ('\n', out);
}

/** Write a file containing a switch statement.
 *
 * \return Nonzero when successful.
 **/
static
  int
write_switch_file (const char* ofilename,
                   const char* strname, const char* casepfx,
                   const char* array_name,
                   char* text)
{
  const char** lines = 0;
  size_t nlines = 0;
  size_t i;
  size_t depth = 0;
  FILE* out = fopen(ofilename, "wb");

  if (!out)  return 0;

  lines = sep_text_lines (text, &nlines);
  qsort(lines, nlines, sizeof(lines[0]), cmp_cstr_ptr);

  if (array_name) {
    fprintf (out, "static const char* const %s[%lu] = {",
             array_name, nlines);
    for (i = 0; i < nlines; ++i) {
      size_t j;
      if (i > 0)  fputc (',', out);
      fputs ("\n\"", out);
      for (j = 0; lines[i][j]; ++j)
        write_escaped_char (out, lines[i][j]);
      fputc ('"', out);
    }
    fputs ("\n};\n", out);
  }

  write_switch (out, strname, depth);

  for (i = 0; i < nlines; ++i) {
    const char* a = lines[i];
    const char* b = "";
    size_t next_depth = 0;
    if (i + 1 < nlines) {
      b = lines[i+1];
    }

    while (a[next_depth] == b[next_depth]) {
      if (!a[next_depth])  break;
      if (!b[next_depth])  break;
      ++ next_depth;
    }

    while (depth < next_depth) {
      write_case (out, a[depth]);
      depth += 1;
      write_switch (out, strname, depth);
    }
    write_case (out, a[depth]);
    if (!a[depth]) {
      /* Nothing.*/
    }
    else if (!a[depth+1]) {
      fprintf(out, "if(!%s[%lu])", strname, depth+1);
    }
    else {
      size_t j;
      fprintf(out, "if(0==strcmp(&%s[%lu],\"", strname, depth+1);
      for (j = depth+1; a[j]; ++j) {
        write_escaped_char (out, a[j]);
      }
      fputs ("\"))", out);
    }
    fprintf (out, "{\n%s%s;\n}break;", casepfx, a);

    while (depth > next_depth) {
      write_end_switch (out, depth);
      depth -= 1;
    }
  }
  write_end_switch (out, depth);

  fclose(out);
  free(lines);
  return 1;
}

/** Execute me now!**/
  int
main(int argc, const char** argv)
{
  int argi = 1;
  const char* strname = 0;
  const char* casepfx = 0;
  const char* xfilename = 0;
  const char* ofilename = 0;
  const char* array_name = 0;
  char* text = 0;
  (void) argc;

  strname = argv[argi++];
  if (!strname)
    BailOut(1, "Need an argument.");

  while (argv[argi]) {
    const char* arg = argv[argi++];
    if (eq_cstr ("-x", arg)) {
      xfilename = argv[argi++];
      if (!xfilename)  BailOut(1, "Need something after -x.");
    }
    else if (eq_cstr ("-o", arg)) {
      ofilename = argv[argi++];
      if (!ofilename)  BailOut(1, "Need something after -o.");
    }
    else if (eq_cstr ("-case-pfx", arg)) {
      casepfx = argv[argi++];
      if (!casepfx)  BailOut(1, "Need something after -case-pfx.");
    }
    else if (eq_cstr ("-array", arg)) {
      array_name = argv[argi++];
      if (!casepfx)  BailOut(1, "Need something after -array.");
    }
  }
  if (!xfilename)  BailOut(1, "Please supply -x.");
  if (!ofilename)  BailOut(1, "Please supply -o.");
  if (!casepfx)  BailOut(1, "Please supply -case-pfx.");


  text = read_text_file (xfilename);
  if (!text)  BailOut(1, "Cannot read input.");

  if (!write_switch_file (ofilename, strname, casepfx, array_name, text)) {
    BailOut(1, "Failed to write switch file.");
  }
  free(text);
  return 0;
}

