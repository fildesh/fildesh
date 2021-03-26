
#include "lace.h"

#include <assert.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <io.h>
#else
#include <unistd.h>
#endif

#include <sys/stat.h>


static
  void
read_LaceXF(LaceXF* xf)
{
  static const size_t chunksz = 4096;
  const size_t orig_sz = xf->base.buf.sz;
  char* buf = grow_LaceX(&xf->base, chunksz);

#ifdef _WIN32
  long n = _read(xf->fd, buf, chunksz);
#else
  ssize_t n = read(xf->fd, buf, chunksz);
#endif

  if (n <= 0) {
    xf->base.buf.sz = orig_sz;
  } else {
    xf->base.buf.sz = orig_sz + (size_t) n;
  }
}

static
  void
close_LaceXF(LaceXF* xf)
{
  if (xf->fd < 0) {
    close(xf->fd);
    xf->fd = -1;
  }
  if (xf->filename) {
    free(xf->filename);
    xf->filename = NULL;
  }
}

DEFINE_LaceX_VTable(LaceXF, base);

  bool
open_LaceXF(LaceXF* xf, const char* filename)
{
  static const char dev_stdin[] = "/dev/stdin";
  static const char dev_fd_prefix[] = "/dev/fd/";
  static const unsigned dev_fd_prefix_length = sizeof(dev_fd_prefix)-1;

  assert(xf->fd < 0);
  assert(!xf->filename);
  xf->base.vt = DEFAULT_LaceXF_LaceX_VTable;

  if (0 == strcmp("-", filename) || 0 == strcmp(dev_stdin, filename)) {
    xf->fd = 0;
    xf->filename = malloc(sizeof(dev_stdin));
    memcpy(xf->filename, dev_stdin, sizeof(dev_stdin));
  }
  else if (0 == strncmp(dev_fd_prefix, filename, dev_fd_prefix_length)) {
    int fd = -1;
    char fdstr[3*sizeof(fd)+1];
    unsigned ndigits;
    char* s = lace_parse_int(&fd, &filename[dev_fd_prefix_length]);

    if (!s || fd < 0) {
      return false;
    }
    xf->fd = fd;

    sprintf(fdstr, "%d", fd);
    ndigits = strlen(fdstr);
    xf->filename = malloc(sizeof(dev_fd_prefix) + ndigits);
    memcpy(xf->filename, dev_stdin, sizeof(dev_stdin)-1);
    memcpy(&xf->filename[sizeof(dev_stdin)-1], fdstr, ndigits+1);
  }
  else {
    const size_t filename_length = strlen(filename);
#ifdef _WIN32
    xf->fd = _open(filename, _O_RDONLY);
#else
    xf->fd = open(filename, O_RDONLY);
#endif
    if (xf->fd < 0) {
      return false;
    }
    xf->filename = malloc(filename_length+1);
    memcpy(xf->filename, filename, filename_length+1);
  }
  return true;
}
