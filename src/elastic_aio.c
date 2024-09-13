/**
 * \file elastic_aio.c
 * Echo stdin to stdout with an arbitrary sized buffer.
 **/
#include <aio.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

#define FILDESH_LOG_TRACE_ON
#include <fildesh/fildesh.h>

#include "include/fildesh/fildesh_compat_errno.h"

typedef struct IOState IOState;

struct IOState
{
  struct aiocb aio;
  bool pending;
  bool done;
  size_t off;

  /** Buffer being used for asynchronous operations.*/
  DECLARE_FildeshAT(unsigned char, buf);

  /** Buffer that was read.*/
  DECLARE_FildeshAT(unsigned char, xbuf);
};

static
  IOState
default_IOState()
{
  IOState io;
  memset(&io.aio, 0, sizeof(io.aio));
  io.pending = false;
  io.done = false;
  io.off = 0;
  init_FildeshAT(io.buf);
  init_FildeshAT(io.xbuf);
  return io;
}

bool all_done(IOState** ios)
{
  unsigned i;
  for (i = 1; i < count_of_FildeshAT(ios); ++i) {
    if (!(*ios)[i].done) {
      fildesh_log_trace("all done? No");
      return 0;
    }
  }
  fildesh_log_trace("all done? Yes");
  return 1;
}

static
  int
setfd_async(Fildesh_fd fd)
{
  int istat;
  istat = fcntl(fd, F_GETFD);
  if (istat < 0) {
    return istat;
  }
  return fcntl(fd, F_SETFD, istat | O_ASYNC);
}

  int
main_elastic_aio(unsigned argc, char** argv)
{
  unsigned argi = 1;
  int istat = 0;
  DECLARE_DEFAULT_FildeshAT(IOState, ios);
  const size_t xbuf_inc = 1024;
  const struct aiocb** aiocb_buf;
  IOState* x; /* Input.*/
  unsigned i;

  /**** BEGIN ARGUMENT_PARSING ****/
  grow_FildeshAT(ios, 1);
  (*ios)[0] = default_IOState();
  (*ios)[0].aio.aio_fildes = -1;

  while (argi < argc) {
    const char* arg = argv[argi++];
    IOState* io;
    Fildesh_fd fd;

    if (0 == strcmp(arg, "-x")) {
      if (argi == argc) {
        fildesh_log_errorf("%s: need input file after -x.", argv[0]);
        return 1;
      }
      arg = argv[argi++];
      fd = fildesh_arg_open_readonly(arg);
      io = &(*ios)[0];
    } else if (0 == strcmp(arg, "-o")) {
      if (argi == argc) {
        fildesh_log_errorf("%s: need output file after -o.", argv[0]);
        return 1;
      }
      arg = argv[argi++];
      fd = fildesh_arg_open_writeonly(arg);
      io = grow1_FildeshAT(ios);
    } else {
      fd = fildesh_arg_open_writeonly(arg);
      io = grow1_FildeshAT(ios);
    }
    *io = default_IOState();
    io->aio.aio_fildes = fd;

    if (fd < 0) {
      fildesh_log_errorf("%s: failed to open: %s", argv[0], arg);
      return 1;
    }
    if (0 > setfd_async(fd)) {
      fildesh_log_errorf("%s: failed to set O_ASYNC on %s.", argv[0], arg);
      return 1;
    }
  }

  /* Default input is stdin.*/
  if ((*ios)[0].aio.aio_fildes == -1) {
    Fildesh_fd fd = fildesh_arg_open_readonly("-");
    IOState* io = &(*ios)[0];
    io->aio.aio_fildes = fd;
    if (0 > setfd_async(fd)) {
      fildesh_log_trace("setfd_async(stdin)");
      fildesh_log_errorf("%s: failed to set O_ASYNC on stdin.", argv[0]);
      return 1;
    }
  }
  /* Default output is stdout.*/
  if (count_of_FildeshAT(ios) == 1) {
    Fildesh_fd fd = fildesh_arg_open_writeonly("-");
    IOState* io = grow1_FildeshAT(ios);
    *io = default_IOState();
    io->aio.aio_fildes = fd;
    if (0 > setfd_async(fd)) {
      fildesh_log_trace("setfd_async(stdout)");
      fildesh_log_errorf("%s: failed to set O_ASYNC on stdout.", argv[0]);
      return 1;
    }
  }
  fildesh_log_trace("Done opening files.");
  /**** END ARGUMENT_PARSING ****/

  aiocb_buf = (const struct aiocb**) malloc(
      sizeof(*aiocb_buf) * count_of_FildeshAT(ios));

  x = &(*ios)[0];
  grow_FildeshAT(x->buf, xbuf_inc);

  while (!all_done(ios)) {
    ssize_t sstat;

    /* Initiate read.*/
    if (!x->pending && !x->done) {
      x->aio.aio_buf = *x->buf;
      x->aio.aio_nbytes = count_of_FildeshAT(x->buf);
      do {
        fildesh_compat_errno_trace();
        istat = aio_read(&x->aio);
      } while (istat != 0 && EAGAIN == fildesh_compat_errno_clear());
      if (istat == 0) {
        x->pending = true;
      }
      else {
        fildesh_log_trace("aio_read() error");
        x->done = true;
        clear_FildeshAT(x->buf);
      }
    }

    /* Initiate writes.*/
    for (i = 1; i < count_of_FildeshAT(ios); ++i) {
      IOState* o = &(*ios)[i];
      const size_t buf_size = count_of_FildeshAT(o->buf);
      const size_t xbuf_size = count_of_FildeshAT(o->xbuf);
      if (o->pending || o->done)  continue;
      /* xbuf >> buf */
      grow_FildeshAT(o->buf, xbuf_size);
      memcpy(&(*o->buf)[buf_size], *o->xbuf, xbuf_size);
      clear_FildeshAT(o->xbuf);
      if (count_of_FildeshAT(o->buf) == 0) {
        o->done = x->done;
        continue;
      }

      o->aio.aio_buf = *o->buf;
      o->aio.aio_nbytes = count_of_FildeshAT(o->buf);
      do {
        fildesh_compat_errno_trace();
        istat = aio_write(&o->aio);
      } while (istat != 0 && EAGAIN == fildesh_compat_errno_clear());
      if (istat == 0) {
        o->pending = true;
      }
      else {
        fildesh_log_trace("aio_write() error");
        o->done = true;
        clear_FildeshAT( o->buf );
      }
    }

    /* Wait for read/write.*/
    do {
      unsigned n = 0;
      for (i = 0; i < count_of_FildeshAT(ios); ++i) {
        IOState* io = &(*ios)[i];
        if (io->pending) {
          aiocb_buf[n++] = &io->aio;
        }
      }
      istat = 0;
      if (n > 0) {
        istat = aio_suspend(aiocb_buf, n, NULL);
      }
    } while (istat != 0 && errno == EINTR);

    if (istat != 0) {
      fildesh_log_trace("aio_suspend()");
      break;
    }

    /* Handle reading.*/
    /* If statement that we break from...*/
    if (x->pending) do {
      istat = aio_error(&x->aio);

      if (istat == EINPROGRESS) {
        break;
      }

      x->pending = 0;
      if (istat != 0) {
        fildesh_log_trace("aio_error(read)");
        x->done = true;
        clear_FildeshAT( x->buf );
        break;
      }
      sstat = aio_return(&x->aio);
      if (sstat <= 0) {
        x->done = true;
        clear_FildeshAT( x->buf );
        break;
      }
      fildesh_log_tracef("aio_return() -> %d", (int)sstat);
      x->aio.aio_offset += sstat;

      for (i = 1; i < count_of_FildeshAT(ios); ++i) {
        IOState* o = &(*ios)[i];
        const size_t o_xbuf_size = count_of_FildeshAT(o->xbuf);
        if (o->done)  continue;
        /* x->buf >> o->xbuf */
        grow_FildeshAT(o->xbuf, sstat);
        memcpy(&(*o->xbuf)[o_xbuf_size], *x->buf, sstat);
      }
    } while (0);


    /* Handle some writing.*/
    for (i = 1; i < count_of_FildeshAT(ios); ++i) {
      IOState* o = &(*ios)[i];

      if (!o->pending || o->done)  continue;
      istat = aio_error(&o->aio);

      if (istat == EINPROGRESS) {
        continue;
      }

      o->pending = false;
      if (istat != 0) {
        fildesh_log_trace("aio_error(write)");
        o->done = true;
        clear_FildeshAT( o->buf );
        clear_FildeshAT( o->xbuf );
        continue;
      }

      sstat = aio_return(&o->aio);
      if (sstat < 0) {
        fildesh_log_trace("aio_return(write)");
        o->done = true;
        clear_FildeshAT( o->buf );
        clear_FildeshAT( o->xbuf );
        continue;
      }
      o->aio.aio_offset += sstat;

      if ((size_t)sstat == count_of_FildeshAT(o->buf)) {
        clear_FildeshAT( o->buf );
      }
      else {
        size_t sz = count_of_FildeshAT(o->buf) - (size_t) sstat;
        memmove(*o->buf, &(*o->buf)[sstat], sz);
        resize_FildeshAT(o->buf, sz);
      }
    }
  }

  for (i = 0; i < count_of_FildeshAT(ios); ++i) {
    Fildesh_fd fd = (*ios)[i].aio.aio_fildes;
    if ((*ios)[i].pending) {
      aio_cancel(fd, &(*ios)[i].aio);
    }
    close(fd);
    close_FildeshAT((*ios)[i].buf);
    close_FildeshAT((*ios)[i].xbuf);
  }
  close_FildeshAT(ios);
  free(aiocb_buf);
  return 0;
}

#ifndef FILDESH_BUILTIN_LIBRARY
  int
main(int argc, char** argv)
{
  int exstatus;
  exstatus = main_elastic_aio((unsigned)argc, argv);
  return exstatus;
}
#endif
