/**
 * \file elastic_aio.c
 * Echo stdin to stdout with an arbitrary sized buffer.
 **/

#include <aio.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

#include <fildesh/fildesh.h>

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>

/* #define DEBUGGING */

#ifdef DEBUGGING
#define StateMsg(msg)  fildesh_log_trace(msg)
#define StateMsg1(msg, x)  fildesh_log_tracef("%s: %s", msg, x)
#else
#define StateMsg(msg)
#define StateMsg1(msg, x)
#endif


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
      StateMsg("all done? No");
      return 0;
    }
  }
  StateMsg("all done? Yes");
  return 1;
}

static
  int
setfd_async(fildesh_fd_t fd)
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
    fildesh_fd_t fd;

    if (0 == strcmp(arg, "-x")) {
      if (argi == argc) {
        fprintf(stderr, "%s: need input file after -x.\n", argv[0]);
        return 1;
      }
      arg = argv[argi++];
      fd = fildesh_arg_open_readonly(arg);
      io = &(*ios)[0];
    } else if (0 == strcmp(arg, "-o")) {
      if (argi == argc) {
        fprintf(stderr, "%s: need output file after -o.\n", argv[0]);
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
      fprintf (stderr, "%s: failed to open: %s\n", argv[0], arg);
      return 1;
    }
    if (0 > setfd_async(fd)) {
      fprintf (stderr, "%s: failed to set O_ASYNC on %s.\n", argv[0], arg);
      return 1;
    }
  }

  /* Default input is stdin.*/
  if ((*ios)[0].aio.aio_fildes == -1) {
    fildesh_fd_t fd = fildesh_arg_open_readonly("-");
    IOState* io = &(*ios)[0];
    io->aio.aio_fildes = fd;
    if (0 > setfd_async(fd)) {
      StateMsg("setfd_async(stdin)");
      fprintf (stderr, "%s: failed to set O_ASYNC on stdin.\n", argv[0]);
      return 1;
    }
  }
  /* Default output is stdout.*/
  if (count_of_FildeshAT(ios) == 1) {
    fildesh_fd_t fd = fildesh_arg_open_writeonly("-");
    IOState* io = grow1_FildeshAT(ios);
    *io = default_IOState();
    io->aio.aio_fildes = fd;
    if (0 > setfd_async(fd)) {
      StateMsg("setfd_async(stdout)");
      fprintf (stderr, "%s: failed to set O_ASYNC on stdout.\n", argv[0]);
      return 1;
    }
  }
  StateMsg("Done opening files.");
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
      istat = aio_read(&x->aio);
      if (istat == 0) {
        x->pending = 1;
      }
      else {
        StateMsg("aio_read() error");
        x->done = 1;
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
      istat = aio_write(&o->aio);
      if (istat == 0) {
        o->pending = 1;
      }
      else {
        StateMsg("aio_write() error");
        o->done = 1;
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
      istat = aio_suspend(aiocb_buf, n, 0);
    } while (istat != 0 && errno == EINTR);

    if (istat != 0) {
      StateMsg( "aio_suspend()" );
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
        StateMsg( "aio_error(read)" );
        x->done = 1;
        clear_FildeshAT( x->buf );
        break;
      }
      sstat = aio_return(&x->aio);
      if (sstat <= 0) {
        x->done = 1;
        clear_FildeshAT( x->buf );
        break;
      }
      StateMsg1("aio_return() -> %d", (int)sstat);
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

      o->pending = 0;
      if (istat != 0) {
        StateMsg( "aio_error(write)" );
        o->done = 1;
        clear_FildeshAT( o->buf );
        clear_FildeshAT( o->xbuf );
        continue;
      }

      sstat = aio_return(&o->aio);
      if (sstat < 0) {
        StateMsg( "aio_return(write)" );
        o->done = 1;
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
    fildesh_fd_t fd = (*ios)[i].aio.aio_fildes;
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
