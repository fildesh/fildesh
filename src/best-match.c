
#include "fildesh.h"
#include "fildesh_compat_errno.h"

#include <assert.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

static void print_usage() {
#define f(s)  fputs(s "\n", stderr)
  f( "Usage: best-match TABLE QUERIES [OPTION]*" );
  f( "    TABLE is a file used for lookup." );
  f( "    QUERIES is a file, each line prompts an output of the closest match from TABLE." );
#undef f
}

static
  char**
split_lines (char* buf, unsigned* ret_max_len)
{
  unsigned i;
  unsigned n = 1;
  unsigned max_len = 1;
  char* s = buf;
  char** lines;

  while (1)
  {
    s = strchr (s, '\n');
    if (!s)  break;
    ++n;
    s[0] = '\0';
    s = &s[1];
  }

  lines = (char**) malloc(sizeof(char*)*(n+1));
  lines[n] = 0;

  s = buf;
  for (i = 0; i < n; ++i)
  {
    unsigned len;
    lines[i] = s;
    len = strlen (s);
    if (len > max_len)  max_len = len;
    s = &s[1 + len];
  }

  *ret_max_len = max_len;
  return lines;
}

/** Find the length of the longest common subsequence between /x/ and /y/.
 * /width/ of /a/ must not be less than the length of /y/.
 **/
static
  unsigned
lcs_count (unsigned* a, unsigned width, const char* x, const char* y)
{
  unsigned i, j, m, n;

  memset (a, 0, width * sizeof (unsigned));
  m = strlen (x);
  n = strlen (y);
  assert(n <= width);

  if (m == 0 || n == 0)  return 0;

  for (i = 0; i < m; ++i)
  {
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
matching_line (unsigned* a, unsigned width, const char* s, char* const* lines)
{
  unsigned max_count = 0;
  unsigned match_idx = 0;
  unsigned i;

  for (i = 0; lines[i]; ++i)
  {
    unsigned count;
    count = lcs_count (a, width, s, lines[i]);
    if (count > max_count)
    {
      max_count = count;
      match_idx = i;
    }
  }
  return match_idx;
}

  int
main_best_match(unsigned argc, char** argv)
{
  int exstatus = 0;
  unsigned argi = 1;
  unsigned* lcs_array;
  FildeshX* lookup_in = NULL;
  FildeshX* stream_in = NULL;
  const char* lookup_in_arg = NULL;
  const char* stream_in_arg = NULL;
  FildeshO* out = NULL;
  char* buf;
  char* s;
  char** lines;
  unsigned width;

  if (argi + 2 <= argc) {
    lookup_in_arg = argv[argi++];
    stream_in_arg = argv[argi++];
  } else {
    exstatus = 64;
  }

  while (argi < argc && exstatus == 0) {
    const char* arg = argv[argi];
    ++ argi;
    if (0 == strcmp (arg, "-h")) {
      print_usage();
      exstatus = 1;
    }
    else {
      fildesh_log_errorf("Unknown argument: %s", arg);
      exstatus = 64;
    }
  }

  if (exstatus == 0) {
    lookup_in = open_FildeshXF(lookup_in_arg);
    if (!lookup_in) {
      lace_compat_errno_trace();
      fildesh_log_errorf("bestmatch: cannot open %s", lookup_in_arg);
      exstatus = 66;
    }
  }
  if (exstatus == 0) {
    stream_in = open_FildeshXF(stream_in_arg);
    if (!stream_in) {
      lace_compat_errno_trace();
      fildesh_log_errorf("bestmatch: cannot open %s", stream_in_arg);
      exstatus = 66;
    }
  }

  if (exstatus != 0) {
    close_FildeshX(lookup_in);
    close_FildeshX(stream_in);
    print_usage();
    return exstatus;
  }

  buf = slurp_FildeshX(lookup_in);
  lines = split_lines(buf, &width);
  lcs_array = (unsigned*)malloc(sizeof(unsigned)*width);

  out = open_FildeshOF("-");
  if (!out) {
    fildesh_log_error("Cannot open stdout.");
    return 1;
  }

  for (s = getline_FildeshX(stream_in);
       s;
       s = getline_FildeshX(stream_in))
  {
    unsigned i;
    i = matching_line (lcs_array, width, s, lines);
    puts_FildeshO(out, lines[i]);
    putc_FildeshO(out, '\n');
  }

  free (lcs_array);
  free (lines);
  close_FildeshX(lookup_in);
  close_FildeshX(stream_in);
  close_FildeshO(out);
  return 0;
}

#ifndef LACE_BUILTIN_LIBRARY
  int
main(int argc, char** argv)
{
  int istat = main_best_match(argc, argv);
  return istat;
}
#endif
