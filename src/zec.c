/** Simple utility to cat and echo.
 * Paste is also supported!
 **/

#include "utilace.h"
#include "cx/alphatab.h"
#include <assert.h>

static
  bool
write_all(fd_t fd, const char* buf, size_t sz)
{
  size_t off = 0;
  while (off < sz) {
    long nbytes = write_sysCx(fd, &buf[off], sz - off);
    if (nbytes <= 0)
      return 0;
    off += nbytes;
  }
  return 1;
}

static
  bool
fdputs(fd_t fd, const char* msg)
{
  return write_all(fd, msg, strlen(msg));
}

static
  void
show_usage ()
{
#define W( a )  fdputs (2, a); fdputs (2, "\n")
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
    fdputs (2, "Cannot open file! ");
    fdputs (2, filename);
    fdputs (2, "\n");
    exit(1);
  }
  return *xf;
}

static
  void
cat_the_file (fd_t o_fd, LaceX* in)
{
  size_t nread;
  do {
    nread = read_LaceX(in);
    if (!write_all(o_fd, in->buf.at, nread))
      break;
    in->off += nread;
    maybe_flush_LaceX(in);
    assert(in->off == 0);
    assert(in->buf.sz == 0);
  } while (nread > 0);
  close_LaceX(in);
}

static
  bool
all_have_data(LaceXF* inputs, uint n) {
  uint i;
  for (i = 0; i < n; ++i) {
    LaceX* in = &inputs[i].base;
    if (in->off >= in->buf.sz) {
      if (0 == read_LaceX(in)) {
        return false;
      }
    }
  }
  return true;
}

static
  bool
cat_next_line(fd_t o_fd, LaceX* in)
{
  char* s = getline_LaceX(in);
  if (!s) {
    return true;
  }
  return fdputs(o_fd, s);
}

LaceUtilMain(zec)
{
  int i;
  int beg_slash = argc;
  int end_slash = argc;
  fd_t o_fd = 1;
  bool paste_mode = false;
  const char* unless_arg = 0;
  LaceXF* inputs;
  size_t mid_sz;
  char* mid_buf;
  int nbegs;
  int nends;

  while (argi < argc) {
    char* arg = argv[argi++];
    if (eq_cstr (arg, "--")) {
      break;
    }
    else if (eq_cstr (arg, "-h")) {
      show_usage ();
      return 0;
    }
    else if (eq_cstr (arg, "-o")) {
      arg = argv[argi++];
      if (!arg) {
        show_usage ();
        failout_sysCx ("Need a filename after -o.");
      }
      if (eq_cstr (arg, "-")) {
        o_fd = 1;
      }
      else {
        o_fd = open_lace_ofd(arg);
        if (o_fd < 0) {
          fdputs (2, "Cannot open file for writing! ");
          fdputs (2, arg);
          fdputs (2, "\n");
          exit(1);
        }
      }
    }
    else if (eq_cstr (arg, "-paste")) {
      paste_mode = true;
    }
    else if (eq_cstr (arg, "-unless")) {
      unless_arg = argv[argi++];
      if (!unless_arg) {
        show_usage ();
        failout_sysCx ("Need a string after -unless.");
      }
    }
    else {
      argi -= 1;
      break;
    }
  }

  if (unless_arg && unless_arg[0]) {
    fdputs (o_fd, unless_arg);
    lose_sysCx ();
    return 0;
  }

  if (argi == argc) {
    LaceXF xf = open_input_file(NULL);
    cat_the_file(o_fd, &xf.base);
    lose_sysCx ();
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
  nends = argc - end_slash - OneIf(argc != end_slash);

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
        good = cat_next_line(o_fd, &inputs[i].base);

      if (good)
        good = write_all(o_fd, mid_buf, mid_sz);

      for (i = 0; i < nends && good; ++i)
        good = cat_next_line(o_fd, &inputs[nbegs+i].base);

      if (good)
        good = write_all(o_fd, "\n", 1);
    }

    for (i = 0; i < nbegs + nends; ++i) {
      close_LaceX(&inputs[i].base);
    }
  }
  else {
    bool good = true;

    for (i = 0; i < nbegs && good; ++i)
      cat_the_file(o_fd, &inputs[i].base);

    if (good)
      good = write_all (o_fd, mid_buf, mid_sz);

    for (i = 0; i < nends && good; ++i)
      cat_the_file(o_fd, &inputs[nbegs+i].base);
  }

  closefd_sysCx(o_fd);
  free (inputs);
  free (mid_buf);
  lose_sysCx ();
  return 0;
}

