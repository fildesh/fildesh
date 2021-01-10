/**
 * \file elastic.c
 * Echo stdin to stdout with an arbitrary sized buffer.
 **/
#define POSIX_SOURCE

#include <aio.h>
#include "utilace.h"
#include "cx/alphatab.h"
#include "cx/table.h"

#include <errno.h>
#include <stdio.h>

/* #define DEBUGGING */

#ifdef DEBUGGING
#define StateMsg(msg)  DBog0(msg)
#else
#define StateMsg(msg)
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
    if (!ios->s[i].done)
      return 0;
  }
  return 1;
}

LaceUtilMain(elastic)
{
  int istat = 0;
  DeclTable( IOState, ios );
  const zuint xbuf_inc = 1024;
  const struct aiocb** aiocb_buf;
  IOState* x; /* Input.*/
  uint i;

  GrowTable( ios, 1 );
  Zeroize( ios.s[0] );
  ios.s[0].aio.aio_fildes = 0;

  if (argi == argc) {
    IOState* o = Grow1Table( ios );
    Zeroize( *o );
    o->aio.aio_fildes = 1;
  }

  while (argi < argc) {
    const char* arg = argv[argi++];
    const int flags
      = O_WRONLY | O_CREAT | O_TRUNC | O_ASYNC | O_APPEND;
    const int mode
      = S_IWUSR | S_IWGRP | S_IWOTH
      | S_IRUSR | S_IRGRP | S_IROTH;
    IOState* o;
    fd_t fd;

    if (eq_cstr (arg, "-")) {
      fd = 1;
    }
    else {
      fd = open (arg, flags, mode);
    }

    if (fd < 0) {
      fprintf (stderr, "%s: failed to open: %s\n", argv[0], arg);
      return 1;
    }

    GrowTable( ios, 1 );
    o = TopTable( ios );
    Zeroize( *o );
    o->aio.aio_fildes = fd;
  }

  AllocTo( aiocb_buf, ios.sz );

  x = &ios.s[0];
  GrowTable( x->buf, xbuf_inc );

  while (!all_done (&ios)) {
    ssize_t sstat;

    /* Initiate read.*/
    if (!x->pending && !x->done) {
      x->aio.aio_buf = x->buf.s;
      x->aio.aio_nbytes = x->buf.sz;
      istat = aio_read (&x->aio);
      if (istat == 0) {
        x->pending = 1;
      }
      else {
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
      istat = aio_write (&o->aio);
      if (istat == 0) {
        o->pending = 1;
      }
      else {
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
      istat = aio_suspend (aiocb_buf, n, 0);
    } while (istat != 0 && errno == EINTR);

    if (istat != 0) {
      StateMsg( "aio_suspend()" );
      break;
    }

    /* Handle reading.*/
    /* If statement that we break from...*/
    if (x->pending) do {
      zuint sz;
      istat = aio_error (&x->aio);

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
      sstat = aio_return (&x->aio);
      if (sstat <= 0) {
        x->done = 1;
        ClearTable( x->buf );
        break;
      }

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
      istat = aio_error (&o->aio);

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

      sstat = aio_return (&o->aio);
      if (sstat < 0) {
        StateMsg( "aio_return(write)" );
        o->done = 1;
        ClearTable( o->buf );
        ClearTable( o->xbuf );
        continue;
      }

      if ((zuint)sstat == o->buf.sz) {
        ClearTable( o->buf );
      }
      else {
        zuint sz = o->buf.sz - (zuint) sstat;
        memmove (o->buf.s, &o->buf.s[sstat], sz);
        ResizeTable( o->buf, sz );
      }
    }
  }

  UFor( i, ios.sz ) {
    fd_t fd = ios.s[i].aio.aio_fildes;
    if (ios.s[i].pending) {
      aio_cancel (fd, &ios.s[i].aio);
    }
    close (fd);
    LoseTable( ios.s[i].buf );
    LoseTable( ios.s[i].xbuf );
  }
  LoseTable( ios );

  lose_sysCx ();
  return 0;
}

