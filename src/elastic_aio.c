/**
 * \file elastic_aio.c
 * Echo stdin to stdout with an arbitrary sized buffer.
 **/

#include <aio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>

#include "cx/alphatab.h"
#include "cx/table.h"
#include "fildesh.h"

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
  Bool pending;
  Bool done;
  zuint off;

  /** Buffer being used for asynchronous operations.*/
  TableT(byte) buf;

  /** Buffer that was read.*/
  TableT(byte) xbuf;
};

DeclTableT(IOState, IOState);

Bool all_done (const TableT(IOState)* ios)
{
  uint i;
  for (i = 1; i < ios->sz; ++i) {
    if (!ios->s[i].done) {
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
  DeclTable( IOState, ios );
  const zuint xbuf_inc = 1024;
  const struct aiocb** aiocb_buf;
  IOState* x; /* Input.*/
  uint i;

  /**** BEGIN ARGUMENT_PARSING ****/
  GrowTable( ios, 1 );
  Zeroize( ios.s[0] );
  ios.s[0].aio.aio_fildes = -1;

  while (argi < argc) {
    const char* arg = argv[argi++];
    IOState* io;
    fildesh_fd_t fd;

    if (eq_cstr (arg, "-x")) {
      if (argi == argc) {
        fprintf(stderr, "%s: need input file after -x.\n", argv[0]);
        return 1;
      }
      arg = argv[argi++];
      fd = fildesh_arg_open_readonly(arg);
      io = &ios.s[0];
    } else if (eq_cstr (arg, "-o")) {
      if (argi == argc) {
        fprintf(stderr, "%s: need output file after -o.\n", argv[0]);
        return 1;
      }
      arg = argv[argi++];
      fd = fildesh_arg_open_writeonly(arg);
      io = Grow1Table( ios );
    } else {
      fd = fildesh_arg_open_writeonly(arg);
      io = Grow1Table( ios );
    }
    Zeroize( *io );
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
  if (ios.s[0].aio.aio_fildes == -1) {
    fildesh_fd_t fd = fildesh_arg_open_readonly("-");
    IOState* io = &ios.s[0];
    io->aio.aio_fildes = fd;
    if (0 > setfd_async(fd)) {
      StateMsg("setfd_async(stdin)");
      fprintf (stderr, "%s: failed to set O_ASYNC on stdin.\n", argv[0]);
      return 1;
    }
  }
  /* Default output is stdout.*/
  if (ios.sz == 1) {
    fildesh_fd_t fd = fildesh_arg_open_writeonly("-");
    IOState* io = Grow1Table( ios );
    Zeroize( *io );
    io->aio.aio_fildes = fd;
    if (0 > setfd_async(fd)) {
      StateMsg("setfd_async(stdout)");
      fprintf (stderr, "%s: failed to set O_ASYNC on stdout.\n", argv[0]);
      return 1;
    }
  }
  StateMsg("Done opening files.");
  /**** END ARGUMENT_PARSING ****/

  AllocTo( aiocb_buf, ios.sz );

  x = &ios.s[0];
  GrowTable( x->buf, xbuf_inc );

  while (!all_done (&ios)) {
    ssize_t sstat;

    /* Initiate read.*/
    if (!x->pending && !x->done) {
      x->aio.aio_buf = x->buf.s;
      x->aio.aio_nbytes = x->buf.sz;
      istat = aio_read(&x->aio);
      if (istat == 0) {
        x->pending = 1;
      }
      else {
        StateMsg("aio_read() error");
        x->done = 1;
        ClearTable( x->buf );
      }
    }

    /* Initiate writes.*/
    for (i = 1; i < ios.sz; ++i) {
      IOState* o = &ios.s[i];
      if (o->pending || o->done)  continue;
      CatTable( o->buf, o->xbuf );
      ClearTable( o->xbuf );
      if (o->buf.sz == 0) {
        o->done = x->done;
        continue;
      }

      o->aio.aio_buf = o->buf.s;
      o->aio.aio_nbytes = o->buf.sz;
      istat = aio_write(&o->aio);
      if (istat == 0) {
        o->pending = 1;
      }
      else {
        StateMsg("aio_write() error");
        o->done = 1;
        ClearTable( o->buf );
      }
    }

    /* Wait for read/write.*/
    do {
      uint n = 0;
      for (i = 0; i < ios.sz; ++i) {
        IOState* io = &ios.s[i];
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
      zuint sz;
      istat = aio_error(&x->aio);

      if (istat == EINPROGRESS) {
        break;
      }

      x->pending = 0;
      if (istat != 0) {
        StateMsg( "aio_error(read)" );
        x->done = 1;
        ClearTable( x->buf );
        break;
      }
      sstat = aio_return(&x->aio);
      if (sstat <= 0) {
        x->done = 1;
        ClearTable( x->buf );
        break;
      }
      StateMsg1("aio_return() -> %d", (int)sstat);
      x->aio.aio_offset += sstat;

      sz = x->buf.sz;
      x->buf.sz = sstat;
      for (i = 1; i < ios.sz; ++i) {
        IOState* o = &ios.s[i];
        if (o->done)  continue;
        CatTable( o->xbuf, x->buf );
      }
      x->buf.sz = sz;
    } while (0);


    /* Handle some writing.*/
    for (i = 1; i < ios.sz; ++i) {
      IOState* o = &ios.s[i];

      if (!o->pending || o->done)  continue;
      istat = aio_error(&o->aio);

      if (istat == EINPROGRESS) {
        continue;
      }

      o->pending = 0;
      if (istat != 0) {
        StateMsg( "aio_error(write)" );
        o->done = 1;
        ClearTable( o->buf );
        ClearTable( o->xbuf );
        continue;
      }

      sstat = aio_return(&o->aio);
      if (sstat < 0) {
        StateMsg( "aio_return(write)" );
        o->done = 1;
        ClearTable( o->buf );
        ClearTable( o->xbuf );
        continue;
      }
      o->aio.aio_offset += sstat;

      if ((zuint)sstat == o->buf.sz) {
        ClearTable( o->buf );
      }
      else {
        zuint sz = o->buf.sz - (zuint) sstat;
        memmove(o->buf.s, &o->buf.s[sstat], sz);
        ResizeTable( o->buf, sz );
      }
    }
  }

  UFor( i, ios.sz ) {
    fildesh_fd_t fd = ios.s[i].aio.aio_fildes;
    if (ios.s[i].pending) {
      aio_cancel(fd, &ios.s[i].aio);
    }
    close(fd);
    LoseTable( ios.s[i].buf );
    LoseTable( ios.s[i].xbuf );
  }
  LoseTable( ios );
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
