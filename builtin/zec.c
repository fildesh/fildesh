/** Simple utility to cat and echo.
 * Paste is also supported!
 **/

#include "lace.h"
#include "lace_compat_errno.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static
  bool
write_all(LaceO* out, const char* buf, size_t sz)
{
  memcpy(grow_LaceO(out, sz), buf, sz);
  flush_LaceO(out);
  return (0 == out->size);
}

static
  void
show_usage ()
{
#define W( a )  fputs(a "\n", stderr)
  W("Usage: zec [OPTIONS] [FILE...] / [STRING...] / [FILE...]");
  W("   or: zec [OPTIONS] [FILE...] / [STRING...]");
  W("   or: zec [OPTIONS] [FILE...]");
  W("The FILE and STRING contents are output in order.");
  W("OPTIONS:");
  W("  -paste  Operate like the paste utility without delimiters.");
  W("          The STRINGs can act as a delimiter or as a prefix/suffix.");
#undef W
}

static
  void
cat_the_file(LaceO* out, LaceX* in)
{
  size_t nread;
  do {
    nread = read_LaceX(in);
    if (!write_all(out, in->at, nread))
      break;
    in->off += nread;
    maybe_flush_LaceX(in);
    assert(in->off == 0);
    assert(in->size == 0);
  } while (nread > 0);
  close_LaceX(in);
}

static
  bool
all_have_data(LaceX** inputs, unsigned n) {
  unsigned i;
  for (i = 0; i < n; ++i) {
    LaceX* in = inputs[i];
    if (in->off >= in->size) {
      if (0 == read_LaceX(in)) {
        return false;
      }
    }
  }
  return true;
}

static
  bool
cat_next_line(LaceO* out, LaceX* in)
{
  char* s = getline_LaceX(in);
  if (!s) {
    return true;
  }
  puts_LaceO(out, s);
  flush_LaceO(out);
  return (0 == out->size);
}

  int
lace_builtin_zec_main(unsigned argc, char** argv,
                      LaceX** inputv, LaceO** outputv)
{
  unsigned i;
  unsigned argi = 1;
  unsigned beg_slash = argc;
  unsigned end_slash = argc;
  LaceO* out = NULL;
  bool paste_mode = false;
  const char* unless_arg = 0;
  LaceX** inputs;
  size_t mid_sz;
  char* mid_buf;
  unsigned nbegs;
  unsigned nends;

  for (argi = 1; argi < argc; ++argi) {
    const char* arg = argv[argi];
    if (0 == strcmp(arg, "--")) {
      argi += 1;
      break;
    }
    else if (0 == strcmp(arg, "-h")) {
      show_usage ();
      return 0;
    }
    else if (0 == strcmp(arg, "-o")) {
      arg = argv[++argi];
      out = open_arg_LaceOF(argi, argv, outputv);
      if (!out) {
        lace_log_errorf("Cannot open file for writing! %s\n", arg);
        return 1;
      }
    }
    else if (0 == strcmp(arg, "-paste")) {
      paste_mode = true;
    }
    else if (0 == strcmp(arg, "-unless")) {
      unless_arg = argv[++argi];
      if (!unless_arg) {
        show_usage ();
        lace_log_error("Need a string after -unless.\n");
        return 1;
      }
    }
    else {
      break;
    }
  }

  if (!out) {
    out = open_arg_LaceOF(0, argv, outputv);
    if (!out) {
      lace_log_error("Cannot open /dev/stdout for writing!\n");
      return 1;
    }
  }

  if (unless_arg && unless_arg[0]) {
    puts_LaceO(out, unless_arg);
    close_LaceO(out);
    return 0;
  }

  if (argi == argc) {
    LaceX* in = open_arg_LaceXF(0, argv, inputv);
    if (in) {
      cat_the_file(out, in);
      close_LaceO(out);
      return 0;
    }
    close_LaceO(out);
    return 1;
  }

  for (i = argi; i < argc; ++i) {
    if (argv[i][0]=='/' && argv[i][1]=='\0') {
      if (i < beg_slash)
        beg_slash = i;
      else
        end_slash = i;
    }
  }

  mid_sz = 0;
  for (i = beg_slash+1; i < end_slash; ++i)
    mid_sz += strlen (argv[i]);

  mid_buf = (char*) malloc (mid_sz);

  mid_sz = 0;
  for (i = beg_slash+1; i < end_slash; ++i) {
    size_t sz = strlen (argv[i]);
    memcpy (&mid_buf[mid_sz], argv[i], sz);
    mid_sz += sz;
  }

  nbegs = beg_slash - argi;
  nends = argc - end_slash - (argc != end_slash ? 1 : 0);

  inputs = (LaceX**) malloc((nbegs + nends) * sizeof(LaceX*));

  for (i = 0; i < nbegs; ++i) {
    inputs[i] = open_arg_LaceXF(argi+i, argv, inputv);
    if (!inputs[i]) {
      return 1;
    }
  }

  for (i = 0; i < nends; ++i) {
    inputs[nbegs+i] = open_arg_LaceXF(end_slash+1+i, argv, inputv);
    if (!inputs[nbegs + i]) {
      return 1;
    }
  }

  if (paste_mode) {
    bool good = true;
    while (good) {
      good = all_have_data(inputs, nbegs + nends);

      for (i = 0; i < nbegs && good; ++i)
        good = cat_next_line(out, inputs[i]);

      if (good)
        good = write_all(out, mid_buf, mid_sz);

      for (i = 0; i < nends && good; ++i)
        good = cat_next_line(out, inputs[nbegs+i]);

      if (good)
        good = write_all(out, "\n", 1);
    }

    for (i = 0; i < nbegs + nends; ++i) {
      close_LaceX(inputs[i]);
    }
  }
  else {
    bool good = true;

    for (i = 0; i < nbegs && good; ++i)
      cat_the_file(out, inputs[i]);

    if (good)
      good = write_all(out, mid_buf, mid_sz);

    for (i = 0; i < nends && good; ++i)
      cat_the_file(out, inputs[nbegs+i]);
  }

  lace_compat_errno_trace();
  close_LaceO(out);
  free (inputs);
  free (mid_buf);
  return 0;
}

#ifndef LACE_BUILTIN_LIBRARY
int main(int argc, char** argv) {
  return lace_builtin_zec_main(argc, argv, NULL, NULL);
}
#endif
