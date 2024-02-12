
#include <fildesh/fildesh.h>
#include "include/fildesh/fildesh_compat_fd.h"
#include "include/fildesh/fildesh_compat_string.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

typedef struct FildeshOF FildeshOF;
struct FildeshOF {
  FildeshO base;
  Fildesh_fd fd;
  char* filename;
};


static inline
  void
fd_write_FildeshO(FildeshO* o, Fildesh_fd fd)
{
  o->off += fildesh_compat_fd_write(fd, &o->at[o->off], o->size - o->off);
}

static
  void
write_FildeshOF(FildeshOF* of)
{
  fd_write_FildeshO(&of->base, of->fd);
}

static
  void
close_FildeshOF(FildeshOF* of)
{
  if (of->fd >= 0) {
    fildesh_compat_fd_close(of->fd);
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


static inline FildeshOF default_FildeshOF() {
  FildeshOF tmp = {DEFAULT_FildeshO, -1, NULL};
  tmp.base.vt = DEFAULT_FildeshOF_FildeshO_VTable;
  return tmp;
}

/* Specialization for /dev/stderr, which we avoid closing.*/
typedef struct FildeshStderrO FildeshStderrO;
struct FildeshStderrO {FildeshO base;};
static inline void write_FildeshStderrO(FildeshStderrO* o) {
  fd_write_FildeshO(&o->base, 2);
}
static inline void close_FildeshStderrO(FildeshStderrO* o) {(void) o;}
static inline void free_FildeshStderrO(FildeshStderrO* o) {free(o);}
DEFINE_FildeshO_VTable(FildeshStderrO, base);
static FildeshO* open_stderr_FildeshO() {
  FildeshStderrO* o = (FildeshStderrO*) malloc(sizeof(FildeshStderrO));
  if (!o) {return NULL;}
  o->base = default_FildeshO();
  o->base.vt = DEFAULT_FildeshStderrO_FildeshO_VTable;
  return &o->base;
}

/* Specialization for /dev/null, which we avoid writing to.*/
typedef struct FildeshVoidO FildeshVoidO;
struct FildeshVoidO {FildeshO base;};
static inline void write_FildeshVoidO(FildeshVoidO* o) {
  o->base.off = o->base.size;
}
static inline void close_FildeshVoidO(FildeshVoidO* o) {(void) o;}
static inline void free_FildeshVoidO(FildeshVoidO* o) {free(o);}
DEFINE_FildeshO_VTable(FildeshVoidO, base);
static FildeshO* open_null_FildeshO() {
  FildeshVoidO* o = (FildeshVoidO*) malloc(sizeof(FildeshVoidO));
  if (!o) {return NULL;}
  o->base = default_FildeshO();
  o->base.vt = DEFAULT_FildeshVoidO_FildeshO_VTable;
  o->base.flush_lgsize = 1;
  return &o->base;
}

  FildeshO*
open_FildeshOF(const char* filename)
{
  return open_sibling_FildeshOF(NULL, filename);
}

  FildeshO*
open_sibling_FildeshOF(const char* sibling, const char* filename)
{
  static const char dev_stdout[] = "/dev/stdout";
  static const char dev_stderr[] = "/dev/stderr";
  static const char dev_null[] = "/dev/null";
  static const char dev_fd_prefix[] = "/dev/fd/";
  static const unsigned dev_fd_prefix_length = sizeof(dev_fd_prefix)-1;
  const size_t filename_length = (filename ? strlen(filename) : 0);
  FildeshOF of[1];

  if (!filename) {return NULL;}

  if (0 == strcmp("-", filename) || 0 == strcmp(dev_stdout, filename)) {
    return open_fd_FildeshO(1);
  }
  if (0 == strcmp(dev_stderr, filename)) {
    return open_stderr_FildeshO();
  }
  if (0 == strcmp(dev_null, filename)) {
    return open_null_FildeshO();
  }
  if (0 == strncmp(dev_fd_prefix, filename, dev_fd_prefix_length)) {
    int fd = -1;
    char* s = fildesh_parse_int(&fd, &filename[dev_fd_prefix_length]);
    if (!s) {return NULL;}
    return open_fd_FildeshO(fd);
  }

  *of = default_FildeshOF();
  {
    FildeshO oss[1] = {DEFAULT_FildeshO};
    if (sibling) {putstr_FildeshO(oss, sibling);}
    sibling_pathname_bytestring_FildeshO(
        oss, (const unsigned char*)filename, filename_length);
    if (oss->size > 0) {
      putc_FildeshO(oss, '\0');
      of->filename = fildesh_compat_string_duplicate(oss->at);
    }
    close_FildeshO(oss);
  }

  if (of->filename) {
    of->fd = fildesh_compat_file_open_writeonly(of->filename);
    if (of->fd >= 0) {
      FildeshOF* p = malloc(sizeof(FildeshOF));
      if (p) {
        *p = *of;
        return &p->base;
      }
      /* Failed to allocate.*/
      fildesh_compat_fd_close(of->fd);
    }
    /* Failed to open file.*/
    free(of->filename);
  }
  return NULL;
}

  Fildesh_fd
fildesh_arg_open_writeonly(const char* filename)
{
  static const char dev_stdout[] = "/dev/stdout";
  static const char dev_stderr[] = "/dev/stderr";
  static const char dev_fd_prefix[] = "/dev/fd/";
  static const unsigned dev_fd_prefix_length = sizeof(dev_fd_prefix)-1;

  if (!filename) {return -1;}

  if (0 == strcmp("-", filename) || 0 == strcmp(dev_stdout, filename)) {
    return fildesh_compat_fd_claim(1);
  }
  if (0 == strcmp(dev_stderr, filename)) {
    return 2;
  }
  if (0 == strncmp(dev_fd_prefix, filename, dev_fd_prefix_length)) {
    int fd = -1;
    char* s = fildesh_parse_int(&fd, &filename[dev_fd_prefix_length]);
    if (!s) {return -1;}
    return fildesh_compat_fd_claim(fd);
  }

  return fildesh_compat_file_open_writeonly(filename);
}

  FildeshO*
open_fd_FildeshO(Fildesh_fd fd)
{
  char filename[FILDESH_FD_PATH_SIZE_MAX];
  unsigned filename_size;
  FildeshOF* of;
  fd = fildesh_compat_fd_claim(fd);
  if (fd < 0) {return NULL;}
  of = (FildeshOF*) malloc(sizeof(FildeshOF));
  if (!of) {fildesh_compat_fd_close(fd); return NULL;}
  *of = default_FildeshOF();
  /* File descriptor.*/
  of->fd = fd;
  /* Filename.*/
  filename_size = 1 + fildesh_encode_fd_path(filename, fd);
  of->filename = (char*)malloc(filename_size);
  if (!of->filename) {fildesh_compat_fd_close(fd); free(of); return NULL;}
  memcpy(of->filename, filename, filename_size);
  return &of->base;
}

  FildeshO*
open_arg_FildeshOF(unsigned argi, char** argv, FildeshO** outputv)
{
  if (outputv && outputv[argi]) {
    FildeshO* ret = outputv[argi];
    outputv[argi] = NULL;  /* Claim it.*/
    return ret;
  }
  if (!argv[argi]) {return NULL;}
  if (argi == 0 || (argv[argi][0] == '-' && argv[argi][1] == '\0')) {
    if (outputv) {
      if (outputv[0]) {
        FildeshO* ret = outputv[0];
        outputv[0] = NULL;  /* Claim it.*/
        return ret;
      } else {
        return NULL; /* Better not steal the real stdout.*/
      }
    }
    return open_fd_FildeshO(1);
  }
  return open_FildeshOF(argv[argi]);
}

const char* filename_FildeshOF(FildeshO* out) {
  if (out->vt != DEFAULT_FildeshOF_FildeshO_VTable) {
    return NULL;
  }
  return fildesh_castup(FildeshOF, base, out)->filename;
}
