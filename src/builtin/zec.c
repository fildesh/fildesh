/** Simple utility to cat and echo.
 * Paste is also supported!
 **/

#include <fildesh/fildesh.h>
#include "include/fildesh/fildesh_compat_errno.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static
  bool
write_all(FildeshO* out, const char* buf, size_t sz)
{
  memcpy(grow_FildeshO(out, sz), buf, sz);
  flush_FildeshO(out);
  return (0 == out->size);
}

static void show_usage() {
  const char usage[] =
    "Usage: zec [OPTIONS] [FILE...] / [STRING...] / [FILE...]\n"
    "   or: zec [OPTIONS] [FILE...] / [STRING...]\n"
    "   or: zec [OPTIONS] [FILE...]\n"
    "   or: zec [-o FILE] -x FILE\n"
    "The FILE and STRING contents are output in order.\n"
    "OPTIONS:\n"
    "  -o FILE -- Write to a file instead of stdout.\n"
    "  -paste -- Operate like the paste utility without delimiters.\n"
    "      The STRINGs can act as a delimiter or as a prefix/suffix.\n"
    "  -x FILE -- Explicit \"useless use of cat\" syntax (only 1 input).\n"
    ;
  fputs(usage, stderr);
}

static
  void
cat_the_file(FildeshO* out, FildeshX* in)
{
  size_t nread;
  do {
    nread = read_FildeshX(in);
    if (!write_all(out, in->at, nread))
      break;
    in->off += nread;
    maybe_flush_FildeshX(in);
    assert(in->off == 0);
    assert(in->size == 0);
  } while (nread > 0);
  close_FildeshX(in);
}

static
  bool
all_have_data(FildeshX** inputs, unsigned n) {
  unsigned i;
  for (i = 0; i < n; ++i) {
    FildeshX* in = inputs[i];
    if (in->off >= in->size) {
      if (0 == read_FildeshX(in)) {
        return false;
      }
    }
  }
  return true;
}

static
  bool
cat_next_line(FildeshO* out, FildeshX* in)
{
  FildeshX slice = sliceline_FildeshX(in);
  if (!slice.at) {
    return true;
  }
  putslice_FildeshO(out, slice);
  flush_FildeshO(out);
  return (0 == out->size);
}

  int
fildesh_builtin_zec_main(unsigned argc, char** argv,
                      FildeshX** inputv, FildeshO** outputv)
{
  unsigned i;
  unsigned argi = 1;
  unsigned beg_slash = argc;
  unsigned end_slash = argc;
  FildeshO* out = NULL;
  bool paste_mode = false;
  const char* unless_arg = 0;
  FildeshX** inputs;
  size_t mid_sz;
  char* mid_buf;
  unsigned nbegs;
  unsigned nends;
  int exstatus = 0;

  for (argi = 1; argi < argc && exstatus == 0; ++argi) {
    const char* arg = argv[argi];
    if (0 == strcmp(arg, "--")) {
      argi += 1;
      break;
    }
    else if (0 == strcmp(arg, "-h")) {
      show_usage ();
      exstatus = 1;
    }
    else if (0 == strcmp(arg, "-x")) {
      ++argi;
      if (argi + 1 == argc && 0 != strcmp(argv[argi], "/")) {
        break;
      }
      else {
        fildesh_log_error("Please make -x the penultimate arg.");
        exstatus = 64;
      }
    }
    else if (0 == strcmp(arg, "-o")) {
      arg = argv[++argi];
      out = open_arg_FildeshOF(argi, argv, outputv);
      if (!out) {
        fildesh_log_errorf("Cannot open file for writing! %s\n", arg);
        exstatus = 73;
      }
    }
    else if (0 == strcmp(arg, "-paste")) {
      paste_mode = true;
    }
    else if (0 == strcmp(arg, "-unless")) {
      unless_arg = argv[++argi];
      if (!unless_arg) {
        show_usage ();
        fildesh_log_error("Need a string after -unless.\n");
        exstatus = 64;
      }
    }
    else {
      break;
    }
  }

  if (exstatus == 0 && !out) {
    out = open_arg_FildeshOF(0, argv, outputv);
    if (!out) {
      fildesh_log_error("Cannot open /dev/stdout for writing!\n");
      exstatus = 73;
    }
  }

  /* Early return.*/
  if (exstatus != 0) {
    close_FildeshO(out);
    return exstatus;
  }

  if (unless_arg && unless_arg[0]) {
    putstr_FildeshO(out, unless_arg);
    close_FildeshO(out);
    return 0;
  }

  if (argi == argc) {
    FildeshX* in = open_arg_FildeshXF(0, argv, inputv);
    if (in) {
      cat_the_file(out, in);
      close_FildeshO(out);
      return 0;
    }
    close_FildeshO(out);
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
  if (!mid_buf) {exstatus = 71; fildesh_compat_errno_trace();}

  mid_sz = 0;
  for (i = beg_slash+1; i < end_slash && exstatus == 0; ++i) {
    size_t sz = strlen (argv[i]);
    memcpy (&mid_buf[mid_sz], argv[i], sz);
    mid_sz += sz;
  }

  nbegs = beg_slash - argi;
  nends = argc - end_slash - (argc != end_slash ? 1 : 0);

  assert(nbegs < argc);
  assert(nends < argc);
  inputs = (FildeshX**) malloc((nbegs + nends) * sizeof(FildeshX*));
  if (!inputs) {exstatus = 71; fildesh_compat_errno_trace();}

  for (i = 0; i < nbegs && exstatus == 0; ++i) {
    inputs[i] = open_arg_FildeshXF(argi+i, argv, inputv);
    if (!inputs[i] && exstatus == 0) {
      exstatus = 66;
    }
  }

  for (i = 0; i < nends && exstatus == 0; ++i) {
    inputs[nbegs+i] = open_arg_FildeshXF(end_slash+1+i, argv, inputv);
    if (!inputs[nbegs + i] && exstatus == 0) {
      exstatus = 66;
    }
  }

  if (exstatus != 0) {
    if (inputs) {
      for (i = 0; i < nbegs + nends; ++i) {
        close_FildeshX(inputs[i]);
      }
    }
  }
  else if (paste_mode) {
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
      close_FildeshX(inputs[i]);
    }
  }
  else {
    for (i = 0; i < nbegs; ++i) {
      cat_the_file(out, inputs[i]);
    }
    write_all(out, mid_buf, mid_sz);
    for (i = 0; i < nends; ++i) {
      cat_the_file(out, inputs[nbegs+i]);
    }
  }

  fildesh_compat_errno_trace();
  close_FildeshO(out);
  if (inputs) {free(inputs);}
  if (mid_buf) {free(mid_buf);}
  return exstatus;
}

#ifndef FILDESH_BUILTIN_LIBRARY
int main(int argc, char** argv) {
  return fildesh_builtin_zec_main(argc, argv, NULL, NULL);
}
#endif
