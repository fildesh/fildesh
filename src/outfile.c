
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
write_LaceOF(LaceOF* of)
{
  LaceO* o = &of->base;

#ifdef _WIN32
  long n = _write(of->fd, &o->at[o->off], o->size - o->off);
#else
  ssize_t n = write(of->fd, &o->at[o->off], o->size - o->off);
#endif

  if (n > 0) {
    o->off += (size_t) n;
  }
}

static
  void
close_LaceOF(LaceOF* of)
{
  if (of->fd >= 0) {
#ifndef _WIN32
    /* TODO: Causes a short read somewhere.*/
    close(of->fd);
#endif
    of->fd = -1;
  }
  if (of->filename) {
    free(of->filename);
    of->filename = NULL;
  }
}

DEFINE_LaceO_VTable(LaceOF, base);

  bool
open_LaceOF(LaceOF* of, const char* filename)
{
  return open_sibling_LaceOF(of, NULL, filename);
}

  bool
open_sibling_LaceOF(LaceOF* of, const char* sibling, const char* filename)
{
  static const char dev_stdout[] = "/dev/stdout";
  static const char dev_fd_prefix[] = "/dev/fd/";
  static const unsigned dev_fd_prefix_length = sizeof(dev_fd_prefix)-1;
  const size_t filename_length = strlen(filename);

  assert(of->fd < 0);
  assert(!of->filename);
  of->base.vt = DEFAULT_LaceOF_LaceO_VTable;

  if (0 == strcmp("-", filename) || 0 == strcmp(dev_stdout, filename)) {
    of->fd = 1;
    of->filename = malloc(sizeof(dev_stdout));
    memcpy(of->filename, dev_stdout, sizeof(dev_stdout));
  }
  else if (0 == strncmp(dev_fd_prefix, filename, dev_fd_prefix_length)) {
    int fd = -1;
    char* s = lace_parse_int(&fd, &filename[dev_fd_prefix_length]);

    if (!s || fd < 0) {
      return false;
    }
    of->fd = fd;
    of->filename = malloc(filename_length+1);
    memcpy(of->filename, filename, filename_length+1);
  }
  else if (filename[0] != '/' && sibling) {
    size_t sibling_dirlen = 0;
    if (sibling) {
      char* p = strrchr(sibling, '/');
      if (p) {
        sibling_dirlen = (size_t)(p - sibling) + 1;
      }
    }
    of->filename = malloc(sibling_dirlen + filename_length + 1);
    memcpy(of->filename, sibling, sibling_dirlen);
    memcpy(&of->filename[sibling_dirlen], filename, filename_length+1);
  }
  else {
    of->filename = malloc(filename_length+1);
    memcpy(of->filename, filename, filename_length+1);
  }

  if (of->fd < 0) {
#ifdef _WIN32
    const int flags = _O_WRONLY | _O_CREAT | _O_TRUNC | O_APPEND;
    const int mode = _S_IREAD | _S_IWRITE;
    of->fd = _open(filename, flags, mode);
#else
    const int flags =  O_WRONLY | O_CREAT | O_TRUNC | O_APPEND;
    const int mode
      = S_IWUSR | S_IWGRP | S_IWOTH
      | S_IRUSR | S_IRGRP | S_IROTH;
    of->fd = open(filename, flags, mode);
#endif
  }
  if (of->fd < 0 && of->filename) {
    free(of->filename);
    of->filename = NULL;
  }
  return (of->fd >= 0);
}
