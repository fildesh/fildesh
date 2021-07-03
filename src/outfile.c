
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

typedef struct LaceOF LaceOF;
struct LaceOF {
  LaceO base;
  lace_fd_t fd;
  char* filename;
};


static
  void
write_LaceOF(LaceOF* of)
{
  LaceO* o = &of->base;
  o->off += lace_compat_fd_write(of->fd, &o->at[o->off], o->size - o->off);
}

static
  void
close_LaceOF(LaceOF* of)
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
free_LaceOF(LaceOF* of)
{
  free(of);
}

DEFINE_LaceO_VTable(LaceOF, base);

static inline LaceOF default_LaceOF() {
  LaceOF tmp = {DEFAULT_LaceO, -1, NULL};
  tmp.base.vt = DEFAULT_LaceOF_LaceO_VTable;
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
  const size_t filename_length = strlen(filename);
  LaceOF of[1];

  if (0 == strcmp("-", filename) || 0 == strcmp(dev_stdout, filename)) {
    return open_fd_LaceOF(1);
  }
  if (0 == strncmp(dev_fd_prefix, filename, dev_fd_prefix_length)) {
    int fd = -1;
    char* s = lace_parse_int(&fd, &filename[dev_fd_prefix_length]);
    if (!s) {return NULL;}
    return open_fd_LaceOF(fd);
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
    of->filename = malloc(filename_length+1);
    memcpy(of->filename, filename, filename_length+1);
  }

  if (of->fd < 0) {
#ifdef _MSC_VER
    const int flags = (
        _O_WRONLY | _O_CREAT | _O_TRUNC |
        _O_APPEND |
        _O_BINARY |
        _O_NOINHERIT);
    const int mode = _S_IREAD | _S_IWRITE;
    of->fd = _open(filename, flags, mode);
#else
    const int flags = (
        O_WRONLY | O_CREAT | O_TRUNC |
        O_APPEND |
        0 /* O_CLOEXEC is below */);
    const int mode
      = S_IWUSR | S_IWGRP | S_IWOTH
      | S_IRUSR | S_IRGRP | S_IROTH;
    of->fd = open(filename, flags, mode);
    if (of->fd >= 0) {
      lace_compat_fd_cloexec(of->fd);
    }
#endif
  } else {
    lace_compat_fd_cloexec(of->fd);
  }
  if (of->fd < 0 && of->filename) {
    free(of->filename);
    of->filename = NULL;
  }

  if (of->fd >= 0) {
    LaceOF* p = malloc(sizeof(LaceOF));
    *p = *of;
    return &p->base;
  }
  return NULL;
}

  LaceO*
open_fd_LaceOF(lace_fd_t fd)
{
  LaceO filename[1] = {DEFAULT_LaceO};
  LaceOF* of;
  if (fd < 0) {return NULL;}
  of = (LaceOF*) malloc(sizeof(LaceOF));
  *of = default_LaceOF();
  /* File descriptor.*/
  of->fd = fd;
  /* Filename.*/
  puts_LaceO(filename, "/dev/fd/");
  print_int_LaceO(filename, fd);
  putc_LaceO(filename, '\0');
  of->filename = &filename->at[0];
  /* Cloexec.*/
  lace_compat_fd_cloexec(fd);
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
    return open_fd_LaceOF(1);
  }
  return open_LaceOF(argv[argi]);
}

const char* filename_LaceOF(LaceO* in) {
  if (in->vt != DEFAULT_LaceOF_LaceO_VTable) {
    return NULL;
  }
  return lace_castup(LaceOF, base, in)->filename;
}
