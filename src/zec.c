/** Simple utility to cat and echo.
 * Paste is also supported!
 **/

#include "utilace.h"
#include "cx/alphatab.h"

typedef struct FInput FInput;
struct FInput {
  fd_t fd;
  uint off;
  uint sz;
  char buf[8093];
};

static
  Bool
write_all (fd_t fd, const char* buf, size_t sz)
{
  size_t off = 0;
  while (off < sz) {
    ssize_t nbytes = write (fd, &buf[off], sz - off);
    if (nbytes <= 0)
      return 0;
    off += nbytes;
  }
  return 1;
}

static
  Bool
fdputs (fd_t fd, const char* msg)
{
  return write_all (fd, msg, strlen(msg));
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
  void
open_input_file (FInput* in, const char* filename)
{
  in->off = 0;
  in->sz = 0;
  in->buf[0] = '\0';

  if (!filename)
    in->fd = 0;
  else if (filename[0] == '-' && filename[1] == '\0')
    in->fd = 0;
  else
    in->fd = open (filename, O_RDONLY);

  if (in->fd < 0) {
    fdputs (2, "Cannot open file! ");
    fdputs (2, filename);
    fdputs (2, "\n");
    exit(1);
  }
}

static
  void
cat_the_file (fd_t o_fd, FInput* in)
{
  ssize_t nread;
  do {
    nread = read (in->fd, in->buf, sizeof(in->buf) - 1);
    if (!write_all (o_fd, in->buf, nread))
      break;
  } while (nread > 0);
  close(in->fd);
}

static
  Bool
read_more (FInput* in)
{
  ssize_t nread = read (in->fd, in->buf, sizeof(in->buf) - 1);
  in->off = 0;
  in->sz = (nread > 0) ? nread : 0;
  in->buf[in->sz] = '\0';
  return (in->sz > 0);
}

static
  Bool
all_have_data (FInput* inputs, uint n) {
  uint i;
  for (i = 0; i < n; ++i) {
    FInput* in = &inputs[i];
    if (in->off >= in->sz) {
      if (!read_more (in))
        return 0;
    }
  }
  return 1;
}

static
  Bool
cat_next_line (fd_t o_fd, FInput* in)
{
  char* s;
  do {
    uint n;

    s = strchr (&in->buf[in->off], '\n');

    if (s)
      n = IdxElt( &in->buf[in->off], s );
    else
      n = in->sz - in->off;

    if (n > 0) {
      if (!write_all (o_fd, &in->buf[in->off], n))
        return 0;
    }

    in->off += n+1;
    if (!s) {
      if (!read_more (in))
        break;
    }

  } while (!s);
  return 1;
}


LaceUtilMain(zec)
{
  int i;
  int beg_slash = argc;
  int end_slash = argc;
  fd_t o_fd = 1;
  Bool paste_mode = 0;
  const char* unless_arg = 0;
  FInput* inputs;
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
        int mode
          = S_IWUSR | S_IWGRP | S_IWOTH
          | S_IRUSR | S_IRGRP | S_IROTH;
        o_fd = open (arg, O_WRONLY | O_CREAT | O_TRUNC, mode);
        if (o_fd < 0) {
          fdputs (2, "Cannot open file for writing! ");
          fdputs (2, arg);
          fdputs (2, "\n");
          exit(1);
        }
      }
    }
    else if (eq_cstr (arg, "-paste")) {
      paste_mode = 1;
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
    FInput in[1];
    open_input_file (in, 0);
    cat_the_file (o_fd, in);
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

  inputs = (FInput*) malloc((nbegs + nends) * sizeof(FInput));

  for (i = 0; i < nbegs; ++i)
    open_input_file (&inputs[i], argv[argi+i]);

  for (i = 0; i < nends; ++i)
    open_input_file (&inputs[nbegs + i], argv[end_slash + 1 + i]);


  if (paste_mode) {
    Bool good = 1;
    while (good) {
      good = all_have_data (inputs, nbegs + nends);

      for (i = 0; i < nbegs && good; ++i)
        good = cat_next_line (o_fd, &inputs[i]);

      if (good)
        good = write_all (o_fd, mid_buf, mid_sz);

      for (i = 0; i < nends && good; ++i)
        good = cat_next_line (o_fd, &inputs[nbegs+i]);

      if (good)
        good = write_all (o_fd, "\n", 1);
    }

    for (i = 0; i < nbegs + nends; ++i)
      close (inputs[i].fd);
  }
  else {
    Bool good = 1;

    for (i = 0; i < nbegs && good; ++i)
      cat_the_file (o_fd, &inputs[i]);

    if (good)
      good = write_all (o_fd, mid_buf, mid_sz);

    for (i = 0; i < nends && good; ++i)
      cat_the_file (o_fd, &inputs[nbegs+i]);
  }

  close (o_fd);
  free (inputs);
  free (mid_buf);
  lose_sysCx ();
  return 0;
}

