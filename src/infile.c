
#include "lace.h"
#include "lace_compat_fd.h"

#include <assert.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _MSC_VER
#include <io.h>
#else
#include <unistd.h>
#endif

#include <sys/stat.h>

typedef struct LaceXF LaceXF;
struct LaceXF {
  LaceX base;
  lace_fd_t fd;
  /* unsigned basename_offset; */
  char* filename;
};


static
  void
read_LaceXF(LaceXF* xf)
{
  static const size_t chunksize = 4096;
  const size_t orig_size = xf->base.size;
  char* buf = grow_LaceX(&xf->base, chunksize);
  xf->base.size = orig_size + lace_compat_fd_read(xf->fd, buf, chunksize);
}

static
  void
close_LaceXF(LaceXF* xf)
{
  if (xf->fd >= 0) {
    lace_compat_fd_close(xf->fd);
    xf->fd = -1;
  }
  if (xf->filename) {
    free(xf->filename);
    xf->filename = NULL;
  }
}

static
  void
free_LaceXF(LaceXF* xf)
{
  free(xf);
}

DEFINE_LaceX_VTable(LaceXF, base);

static inline LaceXF default_LaceXF() {
  LaceXF tmp = {DEFAULT_LaceX, -1, NULL};
  tmp.base.vt = DEFAULT_LaceXF_LaceX_VTable;
  return tmp;
}

  LaceX*
open_LaceXF(const char* filename)
{
  return open_sibling_LaceXF(NULL, filename);
}

  LaceX*
open_sibling_LaceXF(const char* sibling, const char* filename)
{
  static const char dev_stdin[] = "/dev/stdin";
  static const char dev_fd_prefix[] = "/dev/fd/";
  static const unsigned dev_fd_prefix_length = sizeof(dev_fd_prefix)-1;
  const size_t filename_length = (filename ? strlen(filename) : 0);
  LaceXF xf[1];

  if (!filename) {return NULL;}

  if (0 == strcmp("-", filename) || 0 == strcmp(dev_stdin, filename)) {
    return open_fd_LaceXF(0);
  }
  if (0 == strncmp(dev_fd_prefix, filename, dev_fd_prefix_length)) {
    int fd = -1;
    char* s = lace_parse_int(&fd, &filename[dev_fd_prefix_length]);
    if (!s) {return NULL;}
    return open_fd_LaceXF(fd);
  }

  *xf = default_LaceXF();
  if (filename[0] != '/' && sibling) {
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

  { /* Open.*/
#ifdef _MSC_VER
    const int flags = (
        _O_RDONLY |
        _O_BINARY |
        _O_NOINHERIT);
    xf->fd = _open(xf->filename, flags);
#else
    const int flags = (
        O_RDONLY |
        0 /* O_CLOEXEC is below */);
    xf->fd = open(xf->filename, flags);
    if (xf->fd >= 0) {
      lace_compat_fd_cloexec(xf->fd);
    }
#endif
  }
  xf->fd = lace_compat_fd_move_off_stdio(xf->fd);
  if (xf->fd >= 0) {
    LaceXF* p = malloc(sizeof(LaceXF));
    *p = *xf;
    return &p->base;
  }

  if (xf->filename) {free(xf->filename);}
  return NULL;
}

  LaceX*
open_fd_LaceXF(lace_fd_t fd)
{
  LaceO filename[1] = {DEFAULT_LaceO};
  LaceXF* xf;
  fd = lace_compat_fd_move_off_stdio(fd);
  if (fd < 0) {return NULL;}
  xf = (LaceXF*) malloc(sizeof(LaceXF));
  *xf = default_LaceXF();
  /* File descriptor.*/
  xf->fd = fd;
  /* Filename.*/
  puts_LaceO(filename, "/dev/fd/");
  print_int_LaceO(filename, fd);
  putc_LaceO(filename, '\0');
  xf->filename = &filename->at[0];
  /* Cloexec.*/
  lace_compat_fd_cloexec(fd);
  return &xf->base;
}

  LaceX*
open_arg_LaceXF(unsigned argi, char** argv, LaceX** inputv)
{
  if (inputv && inputv[argi]) {
    LaceX* ret = inputv[argi];
    inputv[argi] = NULL;  /* Claim it.*/
    return ret;
  }
  if (!argv[argi]) {return NULL;}
  if (argi == 0 || (argv[argi][0] == '-' && argv[argi][1] == '\0')) {
    if (inputv) {
      if (inputv[0]) {
        LaceX* ret = inputv[0];
        inputv[0] = NULL;  /* Claim it.*/
        return ret;
      } else {
        return NULL; /* Better not steal the real stdin.*/
      }
    }
    return open_fd_LaceXF(0);
  }
  return open_LaceXF(argv[argi]);
}

const char* filename_LaceXF(LaceX* in) {
  if (in->vt != DEFAULT_LaceXF_LaceX_VTable) {
    return NULL;
  }
  return lace_castup(LaceXF, base, in)->filename;
}
