
#include "lace.h"
#include "lace_compat_fd.h"
#include "lace_compat_string.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

typedef struct FildeshOF FildeshOF;
struct FildeshOF {
  LaceO base;
  lace_fd_t fd;
  char* filename;
};


static
  void
write_FildeshOF(FildeshOF* of)
{
  LaceO* o = &of->base;
  o->off += lace_compat_fd_write(of->fd, &o->at[o->off], o->size - o->off);
}

static
  void
close_FildeshOF(FildeshOF* of)
{
  if (of->fd >= 0) {
    lace_compat_fd_close(of->fd);
    of->fd = -1;
  }
  if (of->filename) {
    free(of->filename);
    of->filename = NULL;
  }
}

static
  void
free_FildeshOF(FildeshOF* of)
{
  free(of);
}

DEFINE_FildeshO_VTable(FildeshOF, base);

static inline FildeshOF default_LaceOF() {
  FildeshOF tmp = {DEFAULT_FildeshO, -1, NULL};
  tmp.base.vt = DEFAULT_FildeshOF_FildeshO_VTable;
  return tmp;
}

  LaceO*
open_LaceOF(const char* filename)
{
  return open_sibling_LaceOF(NULL, filename);
}

  LaceO*
open_sibling_LaceOF(const char* sibling, const char* filename)
{
  static const char dev_stdout[] = "/dev/stdout";
  static const char dev_fd_prefix[] = "/dev/fd/";
  static const unsigned dev_fd_prefix_length = sizeof(dev_fd_prefix)-1;
  const size_t filename_length = (filename ? strlen(filename) : 0);
  FildeshOF of[1];

  if (!filename) {return NULL;}

  if (0 == strcmp("-", filename) || 0 == strcmp(dev_stdout, filename)) {
    return open_fd_LaceO(1);
  }
  if (0 == strncmp(dev_fd_prefix, filename, dev_fd_prefix_length)) {
    int fd = -1;
    char* s = lace_parse_int(&fd, &filename[dev_fd_prefix_length]);
    if (!s) {return NULL;}
    return open_fd_LaceO(fd);
  }

  *of = default_LaceOF();
  if (filename[0] != '/' && sibling) {
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
    of->filename = lace_compat_string_duplicate(filename);
  }

  of->fd = lace_compat_file_open_writeonly(of->filename);
  if (of->fd >= 0) {
    FildeshOF* p = malloc(sizeof(FildeshOF));
    *p = *of;
    return &p->base;
  }

  if (of->filename) {free(of->filename);}
  return NULL;
}

  lace_fd_t
lace_arg_open_writeonly(const char* filename)
{
  static const char dev_stdout[] = "/dev/stdout";
  static const char dev_fd_prefix[] = "/dev/fd/";
  static const unsigned dev_fd_prefix_length = sizeof(dev_fd_prefix)-1;

  if (!filename) {return -1;}

  if (0 == strcmp("-", filename) || 0 == strcmp(dev_stdout, filename)) {
    return lace_compat_fd_claim(1);
  }
  if (0 == strncmp(dev_fd_prefix, filename, dev_fd_prefix_length)) {
    int fd = -1;
    char* s = lace_parse_int(&fd, &filename[dev_fd_prefix_length]);
    if (!s) {return -1;}
    return lace_compat_fd_claim(fd);
  }

  return lace_compat_file_open_writeonly(filename);
}

  LaceO*
open_fd_LaceO(lace_fd_t fd)
{
  char filename[LACE_FD_PATH_SIZE_MAX];
  unsigned filename_size;
  FildeshOF* of;
  fd = lace_compat_fd_claim(fd);
  if (fd < 0) {return NULL;}
  of = (FildeshOF*) malloc(sizeof(FildeshOF));
  *of = default_LaceOF();
  /* File descriptor.*/
  of->fd = fd;
  /* Filename.*/
  filename_size = 1 + lace_encode_fd_path(filename, fd);
  of->filename = (char*)malloc(filename_size);
  memcpy(of->filename, filename, filename_size);
  return &of->base;
}

  LaceO*
open_arg_LaceOF(unsigned argi, char** argv, LaceO** outputv)
{
  if (outputv && outputv[argi]) {
    LaceO* ret = outputv[argi];
    outputv[argi] = NULL;  /* Claim it.*/
    return ret;
  }
  if (!argv[argi]) {return NULL;}
  if (argi == 0 || (argv[argi][0] == '-' && argv[argi][1] == '\0')) {
    if (outputv) {
      if (outputv[0]) {
        LaceO* ret = outputv[0];
        outputv[0] = NULL;  /* Claim it.*/
        return ret;
      } else {
        return NULL; /* Better not steal the real stdout.*/
      }
    }
    return open_fd_LaceO(1);
  }
  return open_LaceOF(argv[argi]);
}

const char* filename_LaceOF(LaceO* out) {
  if (out->vt != DEFAULT_FildeshOF_FildeshO_VTable) {
    return NULL;
  }
  return lace_castup(FildeshOF, base, out)->filename;
}
