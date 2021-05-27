/** Simple utility to cat and echo.
 * Paste is also supported!
 **/

#include "lace.h"
#ifdef MAIN_LACE_EXECUTABLE
#include "utilace.h"
#endif

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
badnews(const char* msg)
{
  fputs(msg, stderr);
}

static
  void
show_usage ()
{
#define W( a )  badnews(a); badnews("\n")
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
  LaceXF
open_input_file(const char* filename)
{
  LaceXF xf[1] = {DEFAULT_LaceXF};

  if (!filename) {
    filename = "-";
  }
  if (!open_LaceXF(xf, filename)) {
    badnews("Cannot open file! ");
    badnews(filename);
    badnews("\n");
    exit(1);
  }
  return *xf;
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
all_have_data(LaceXF* inputs, unsigned n) {
  unsigned i;
  for (i = 0; i < n; ++i) {
    LaceX* in = &inputs[i].base;
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

#ifdef MAIN_LACE_EXECUTABLE
LaceUtilMain(zec)
#else
int main(int argc, const char** argv)
#endif
{
#ifndef MAIN_LACE_EXECUTABLE
  int argi = 1;
#endif
  int i;
  int beg_slash = argc;
  int end_slash = argc;
  LaceOF ofile = DEFAULT_LaceOF;
  bool paste_mode = false;
  const char* unless_arg = 0;
  LaceXF* inputs;
  size_t mid_sz;
  char* mid_buf;
  int nbegs;
  int nends;

  while (argi < argc) {
    const char* arg = argv[argi++];
    if (0 == strcmp(arg, "--")) {
      break;
    }
    else if (0 == strcmp(arg, "-h")) {
      show_usage ();
      return 0;
    }
    else if (0 == strcmp(arg, "-o")) {
      arg = argv[argi++];
      if (!arg) {
        show_usage ();
        badnews("Need a filename after -o.\n");
        exit(1);
      }
      if (!open_LaceOF(&ofile, arg)) {
        badnews("Cannot open file for writing! ");
        badnews(arg);
        badnews("\n");
        exit(1);
      }
    }
    else if (0 == strcmp(arg, "-paste")) {
      paste_mode = true;
    }
    else if (0 == strcmp(arg, "-unless")) {
      unless_arg = argv[argi++];
      if (!unless_arg) {
        show_usage ();
        badnews("Need a string after -unless.\n");
        exit(1);
      }
    }
    else {
      argi -= 1;
      break;
    }
  }

  if (ofile.fd < 0) {
    if (!open_LaceOF(&ofile, "-")) {
      badnews("Cannot open /dev/stdout for writing!\n");
      exit(1);
    }
  }

  if (unless_arg && unless_arg[0]) {
    puts_LaceO(&ofile.base, unless_arg);
    close_LaceO(&ofile.base);
#ifdef MAIN_LACE_EXECUTABLE
    lose_sysCx ();
#endif
    return 0;
  }

  if (argi == argc) {
    LaceXF xf = open_input_file(NULL);
    cat_the_file(&ofile.base, &xf.base);
#ifdef MAIN_LACE_EXECUTABLE
    lose_sysCx ();
#endif
    return 0;
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

  inputs = (LaceXF*) malloc((nbegs + nends) * sizeof(LaceXF));

  for (i = 0; i < nbegs; ++i)
    inputs[i] = open_input_file(argv[argi+i]);

  for (i = 0; i < nends; ++i)
    inputs[nbegs + i] = open_input_file(argv[end_slash + 1 + i]);


  if (paste_mode) {
    bool good = true;
    while (good) {
      good = all_have_data(inputs, nbegs + nends);

      for (i = 0; i < nbegs && good; ++i)
        good = cat_next_line(&ofile.base, &inputs[i].base);

      if (good)
        good = write_all(&ofile.base, mid_buf, mid_sz);

      for (i = 0; i < nends && good; ++i)
        good = cat_next_line(&ofile.base, &inputs[nbegs+i].base);

      if (good)
        good = write_all(&ofile.base, "\n", 1);
    }

    for (i = 0; i < nbegs + nends; ++i) {
      close_LaceX(&inputs[i].base);
    }
  }
  else {
    bool good = true;

    for (i = 0; i < nbegs && good; ++i)
      cat_the_file(&ofile.base, &inputs[i].base);

    if (good)
      good = write_all(&ofile.base, mid_buf, mid_sz);

    for (i = 0; i < nends && good; ++i)
      cat_the_file(&ofile.base, &inputs[nbegs+i].base);
  }

  close_LaceO(&ofile.base);
  free (inputs);
  free (mid_buf);
#ifdef MAIN_LACE_EXECUTABLE
  lose_sysCx ();
#endif
  return 0;
}

