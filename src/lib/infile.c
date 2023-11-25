
#include <fildesh/fildesh.h>
#include "include/fildesh/fildesh_compat_fd.h"
#include "include/fildesh/fildesh_compat_random.h"
#include "include/fildesh/fildesh_compat_string.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

typedef struct FildeshXF FildeshXF;
struct FildeshXF {
  FildeshX base;
  Fildesh_fd fd;
  /* unsigned basename_offset; */
  char* filename;
};

typedef struct FildeshURandomX FildeshURandomX;
struct FildeshURandomX {
  FildeshX base;
};


static
  void
read_FildeshXF(FildeshXF* xf)
{
  static const size_t chunksize = 4096;
  const size_t orig_size = xf->base.size;
  char* buf = grow_FildeshX(&xf->base, chunksize);
  xf->base.size = orig_size + fildesh_compat_fd_read(xf->fd, buf, chunksize);
}

static
  void
close_FildeshXF(FildeshXF* xf)
{
  if (xf->fd >= 0) {
    fildesh_compat_fd_close(xf->fd);
    xf->fd = -1;
  }
  if (xf->filename) {
    free(xf->filename);
    xf->filename = NULL;
  }
}

static
  void
free_FildeshXF(FildeshXF* xf)
{
  free(xf);
}

DEFINE_FildeshX_VTable(FildeshXF, base);

static inline FildeshXF default_FildeshXF() {
  FildeshXF tmp = {DEFAULT_FildeshX, -1, NULL};
  tmp.base.vt = DEFAULT_FildeshXF_FildeshX_VTable;
  return tmp;
}


static void read_FildeshURandomX(FildeshURandomX* xrng)
{
  static const size_t chunksize = 4096;
  const size_t orig_size = xrng->base.size;
  char* buf = grow_FildeshX(&xrng->base, chunksize);
  xrng->base.size =
    orig_size + fildesh_compat_random_bytes(buf, chunksize);
}
static void close_FildeshURandomX(FildeshURandomX* xrng) {(void) xrng;}
static void free_FildeshURandomX(FildeshURandomX* xrng) {free(xrng);}
DEFINE_FildeshX_VTable(FildeshURandomX, base);

static inline FildeshX* open_urandom_FildeshX() {
  FildeshURandomX* xrng = (FildeshURandomX*) malloc(sizeof(FildeshURandomX));
  if (!xrng) {return NULL;}
  xrng->base = default_FildeshX();
  xrng->base.vt = DEFAULT_FildeshURandomX_FildeshX_VTable;
  return &xrng->base;
}


static Fildesh_fd fildesh_open_null_readonly() {
  Fildesh_fd fd = -1;
  Fildesh_fd tmp_fd = -1;
  int istat;
  istat = fildesh_compat_fd_pipe(&tmp_fd, &fd);
  if (istat != 0) {return -1;}
  istat = fildesh_compat_fd_close(tmp_fd);
  if (istat != 0) {fildesh_compat_fd_close(fd); return -1;}
  return fd;
}

static FildeshX* open_null_FildeshXF() {
  FildeshXF* xf;
  Fildesh_fd fd = fildesh_open_null_readonly();
  if (fd < 0) {return NULL;}
  xf = (FildeshXF*) malloc(sizeof(FildeshXF));
  if (!xf) {return NULL;}
  *xf = default_FildeshXF();
  xf->fd = fd;
  xf->filename = fildesh_compat_string_duplicate("/dev/null");
  return &xf->base;
}

  FildeshX*
open_FildeshXF(const char* filename)
{
  return open_sibling_FildeshXF(NULL, filename);
}

  FildeshX*
open_sibling_FildeshXF(const char* sibling, const char* filename)
{
  static const char dev_stdin[] = "/dev/stdin";
  static const char dev_urandom[] = "/dev/urandom";
  static const char dev_null[] = "/dev/null";
  static const char dev_fd_prefix[] = "/dev/fd/";
  static const unsigned dev_fd_prefix_length = sizeof(dev_fd_prefix)-1;
  const size_t filename_length = (filename ? strlen(filename) : 0);
  FildeshXF xf[1];

  if (!filename) {return NULL;}

  if (0 == strcmp("-", filename) || 0 == strcmp(dev_stdin, filename)) {
    return open_fd_FildeshX(0);
  }
  if (0 == strcmp(dev_urandom, filename)) {
    return open_urandom_FildeshX();
  }
  if (0 == strcmp(dev_null, filename)) {
    return open_null_FildeshXF();
  }
  if (0 == strncmp(dev_fd_prefix, filename, dev_fd_prefix_length)) {
    int fd = -1;
    char* s = fildesh_parse_int(&fd, &filename[dev_fd_prefix_length]);
    if (!s) {return NULL;}
    return open_fd_FildeshX(fd);
  }

  *xf = default_FildeshXF();
  if (filename[0] != '/' && sibling) {
    size_t sibling_dirlen = 0;
    if (sibling) {
      char* p = strrchr(sibling, '/');
      if (p) {
        sibling_dirlen = (size_t)(p - sibling) + 1;
      }
    }
    xf->filename = malloc(sibling_dirlen + filename_length + 1);
    if (xf->filename) {
      memcpy(xf->filename, sibling, sibling_dirlen);
      memcpy(&xf->filename[sibling_dirlen], filename, filename_length+1);
    }
  }
  else {
    xf->filename = fildesh_compat_string_duplicate(filename);
  }

  if (xf->filename) {
    xf->fd = fildesh_compat_file_open_readonly(xf->filename);
    if (xf->fd >= 0) {
      FildeshXF* p = malloc(sizeof(FildeshXF));
      if (p) {
        *p = *xf;
        return &p->base;
      }
      /* Failed to allocate.*/
      fildesh_compat_fd_close(xf->fd);
    }
    /* Failed to open file.*/
    free(xf->filename);
  }
  return NULL;
}

  Fildesh_fd
fildesh_arg_open_readonly(const char* filename)
{
  static const char dev_stdin[] = "/dev/stdin";
  static const char dev_null[] = "/dev/null";
  static const char dev_fd_prefix[] = "/dev/fd/";
  static const unsigned dev_fd_prefix_length = sizeof(dev_fd_prefix)-1;

  if (!filename) {return -1;}

  if (0 == strcmp("-", filename) || 0 == strcmp(dev_stdin, filename)) {
    return fildesh_compat_fd_claim(0);
  }
  if (0 == strcmp(dev_null, filename)) {
    return fildesh_open_null_readonly();
  }
  if (0 == strncmp(dev_fd_prefix, filename, dev_fd_prefix_length)) {
    int fd = -1;
    char* s = fildesh_parse_int(&fd, &filename[dev_fd_prefix_length]);
    if (!s) {return -1;}
    return fildesh_compat_fd_claim(fd);
  }

  return fildesh_compat_file_open_readonly(filename);
}

  FildeshX*
open_fd_FildeshX(Fildesh_fd fd)
{
  char filename[FILDESH_FD_PATH_SIZE_MAX];
  unsigned filename_size;
  FildeshXF* xf;
  fd = fildesh_compat_fd_claim(fd);
  if (fd < 0) {return NULL;}
  xf = (FildeshXF*) malloc(sizeof(FildeshXF));
  if (!xf) {fildesh_compat_fd_close(fd); return NULL;}
  *xf = default_FildeshXF();
  /* File descriptor.*/
  xf->fd = fd;
  /* Filename.*/
  filename_size = 1 + fildesh_encode_fd_path(filename, fd);
  xf->filename = (char*)malloc(filename_size);
  if (!xf->filename) {fildesh_compat_fd_close(fd); free(xf); return NULL;}
  memcpy(xf->filename, filename, filename_size);
  return &xf->base;
}

  FildeshX*
open_arg_FildeshXF(unsigned argi, char** argv, FildeshX** inputv)
{
  if (inputv && inputv[argi]) {
    FildeshX* ret = inputv[argi];
    inputv[argi] = NULL;  /* Claim it.*/
    return ret;
  }
  if (!argv[argi]) {return NULL;}
  if (argi == 0 || (argv[argi][0] == '-' && argv[argi][1] == '\0')) {
    if (inputv) {
      if (inputv[0]) {
        FildeshX* ret = inputv[0];
        inputv[0] = NULL;  /* Claim it.*/
        return ret;
      } else {
        return NULL; /* Better not steal the real stdin.*/
      }
    }
    return open_fd_FildeshX(0);
  }
  return open_FildeshXF(argv[argi]);
}

const char* filename_FildeshXF(FildeshX* in) {
  if (in->vt != DEFAULT_FildeshXF_FildeshX_VTable) {
    return NULL;
  }
  return fildesh_castup(FildeshXF, base, in)->filename;
}
