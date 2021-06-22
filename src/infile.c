
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
  static const size_t chunksize = 4096;
  const size_t orig_size = xf->base.size;
  char* buf = grow_LaceX(&xf->base, chunksize);

#ifdef _WIN32
  long n = _read(xf->fd, buf, chunksize);
#else
  ssize_t n = read(xf->fd, buf, chunksize);
#endif

  if (n <= 0) {
    xf->base.size = orig_size;
  } else {
    xf->base.size = orig_size + (size_t) n;
  }
}

static
  void
close_LaceXF(LaceXF* xf)
{
  if (xf->fd >= 0) {
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
  return open_sibling_LaceXF(xf, NULL, filename);
}

  bool
open_sibling_LaceXF(LaceXF* xf, const char* sibling, const char* filename)
{
  static const char dev_stdin[] = "/dev/stdin";
  static const char dev_fd_prefix[] = "/dev/fd/";
  static const unsigned dev_fd_prefix_length = sizeof(dev_fd_prefix)-1;
  const size_t filename_length = strlen(filename);

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
    char* s = lace_parse_int(&fd, &filename[dev_fd_prefix_length]);

    if (!s || fd < 0) {
      return false;
    }
    xf->fd = fd;
    xf->filename = malloc(filename_length+1);
    memcpy(xf->filename, filename, filename_length+1);
  }
  else if (filename[0] != '/' && sibling) {
    size_t sibling_dirlen = 0;
    if (sibling) {
      char* p = strrchr(sibling, '/');
      if (p) {
        sibling_dirlen = (size_t)(p - sibling) + 1;
      }
    }
    xf->filename = malloc(sibling_dirlen + filename_length + 1);
    memcpy(xf->filename, sibling, sibling_dirlen);
    memcpy(&xf->filename[sibling_dirlen], filename, filename_length+1);
  }
  else {
    xf->filename = malloc(filename_length+1);
    memcpy(xf->filename, filename, filename_length+1);
  }

  if (xf->fd < 0) {
#ifdef _WIN32
    xf->fd = _open(xf->filename, _O_RDONLY);
#else
    xf->fd = open(xf->filename, O_RDONLY);
#endif
  }
  if (xf->fd < 0 && xf->filename) {
    free(xf->filename);
    xf->filename = NULL;
  }
  return (xf->fd >= 0);
}

