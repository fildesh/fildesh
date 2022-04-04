
#include "fildesh.h"
#include "fildesh_compat_errno.h"

#include <assert.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

static void print_usage() {
  const char usage[] =
    "Usage: bestmatch -x-lut LUT [OPTION]*\n"
    "  -x-lut LUT -- File used for lookup. Lines formatted as \"KEY\\tVALUE\".\n"
    "  -x QUERIES -- Input queries; stdin by default.\n"
    "  -o FILE -- Output file; stdout by default.\n"
    "  -d DELIM -- Use a non-tab delimiter in LUT. Empty string means none.\n"
    ;
  fputs(usage, stderr);
}

static char** split_lines(char* buf, unsigned* ret_line_count)
{
  unsigned i;
  unsigned line_count = 0;
  char* s;
  char** lines;

  for (s = strchr(buf, '\n'); s; s = strchr(s, '\n')) {
    ++line_count;
    s[0] = '\0';
    s = &s[1];
  }

  lines = (char**) malloc((line_count+1)*sizeof(char*));

  s = buf;
  for (i = 0; i < line_count; ++i) {
    unsigned len = strlen(s);
    lines[i] = s;
    s = &s[1 + len];
  }
  assert(!s[-1]);

  if (line_count == 0 || lines[line_count-1][0]) {
    lines[line_count] = &s[-1];
    line_count += 1;
  }
  *ret_line_count = line_count;
  return lines;
}

/** Find the length of the longest common subsequence between `x` and `y`.
 * `a` is scratch space. It must be at least as long as `y`.
 **/
static
  unsigned
lcs_count(
    const char* x, unsigned m,
    const char* y, unsigned n,
    unsigned* a)
{
  unsigned i, j;

  if (m == 0 || n == 0) {return 0;}
  memset(a, 0, n * sizeof(unsigned));

  for (i = 0; i < m; ++i) {
    unsigned back_j = 0, back_ij = 0;
    char xc;
    xc = tolower (x[i]);
    for (j = 0; j < n; ++j)
    {
      unsigned back_i;
      char yc;
      yc = tolower (y[j]);
      back_i = a[j];

      if (xc == yc)
        a[j] = back_ij + 1;
      else if (back_j > back_i)
        a[j] = back_j;

      back_ij = back_i;
      back_j = a[j];
    }
  }

  return a[n-1];
}

static
  unsigned
matching_line(const char* query,
              char* const* lines, unsigned line_count,
              const unsigned* key_widths, unsigned* a)
{
  unsigned max_count = 0;
  unsigned min_key_width = 1;
  unsigned match_index = 0;
  unsigned query_width = strlen(query);
  unsigned i;

  for (i = 0; i < line_count; ++i) {
    unsigned count = lcs_count(
        query, query_width,
        lines[i], key_widths[i],
        a);

    if (count > max_count) {
      max_count = count;
      min_key_width = key_widths[i];
      match_index = i;
    }
    else if (count >= max_count && key_widths[i] < min_key_width) {
      min_key_width = key_widths[i];
      match_index = i;
    }
  }
  return match_index;
}

  int
fildesh_builtin_bestmatch_main(
    unsigned argc, char** argv, FildeshX** inputv, FildeshO** outputv)
{
  int exstatus = 0;
  unsigned argi = 1;
  unsigned* lcs_array = NULL;
  FildeshX* lookup_in = NULL;
  FildeshX* stream_in = NULL;
  FildeshO* out = NULL;
  char* s;
  const char* delim = "\t";
  char** lines = NULL;
  unsigned line_count = 0;
  unsigned* key_widths = NULL;
  unsigned max_key_width = 0;

  for (argi = 1; argi < argc && exstatus == 0; ++argi) {
    if (0 == strcmp(argv[argi], "-x-lut")) {
      lookup_in = open_arg_FildeshXF(++argi, argv, inputv);
      if (!lookup_in) {
        fildesh_compat_errno_trace();
        fildesh_log_errorf("Cannot open file for reading: %s", argv[argi]);
        exstatus = 66;
      }
    }
    else if (0 == strcmp(argv[argi], "-x")) {
      stream_in = open_arg_FildeshXF(++argi, argv, inputv);
      if (!stream_in) {
        fildesh_compat_errno_trace();
        fildesh_log_errorf("Cannot open file for reading: %s", argv[argi]);
        exstatus = 66;
      }
    }
    else if (0 == strcmp(argv[argi], "-o")) {
      out = open_arg_FildeshOF(++argi, argv, outputv);
      if (!out) {
        fildesh_log_errorf("Cannot open file for writing: %s", argv[argi]);
        exstatus = 73;
      }
    }
    else if (0 == strcmp(argv[argi], "-d")) {
      delim = argv[++argi];
      if (!delim) {
        fildesh_log_error("Missing delimiter after -d.");
        exstatus = 64;
      } else if (delim[0] == '\0') {
        delim = NULL;
      }
    }
    else {
      fildesh_log_errorf("Unknown argument: %s", argv[argi]);
      exstatus = 64;
    }
  }

  if (exstatus == 0 && !lookup_in) {
    fildesh_log_error("Please provide a -x-lut file.");
    print_usage();
    exstatus = 64;
  }

  if (exstatus == 0 && !stream_in) {
    stream_in = open_arg_FildeshXF(0, argv, inputv);
    if (!stream_in) {
      fildesh_log_error("Cannot open stdin.");
      exstatus = 66;
    }
  }

  if (exstatus == 0 && !out) {
    out = open_arg_FildeshOF(0, argv, outputv);
    if (!out) {
      fildesh_log_error("Cannot open stdout.");
      exstatus = 73;
    }
  }

  if (exstatus == 0) {
    char* buf = slurp_FildeshX(lookup_in);
    unsigned i;
    lines = split_lines(buf, &line_count);
    key_widths = (unsigned*)malloc(line_count * sizeof(unsigned));
    for (i = 0; i < line_count; ++i) {
      unsigned key_width = delim ? strcspn(lines[i], delim) : strlen(lines[i]);
      key_widths[i] = key_width;
      if (key_width > max_key_width) {
        max_key_width = key_width;
      }
    }
    lcs_array = (unsigned*)malloc(max_key_width * sizeof(unsigned));
  }

  if (exstatus == 0) {
    for (s = getline_FildeshX(stream_in); s;
         s = getline_FildeshX(stream_in))
    {
      unsigned i = matching_line(s, lines, line_count, key_widths, lcs_array);
      puts_FildeshO(out, lines[i]);
      putc_FildeshO(out, '\n');
    }
  }

  if (lcs_array) {free(lcs_array);}
  if (key_widths) {free(key_widths);}
  if (lines) {free(lines);}
  close_FildeshX(lookup_in);
  close_FildeshX(stream_in);
  close_FildeshO(out);
  return exstatus;
}

#ifndef FILDESH_BUILTIN_LIBRARY
  int
main(int argc, char** argv)
{
  return fildesh_builtin_bestmatch_main((unsigned)argc, argv, NULL, NULL);
}
#endif
