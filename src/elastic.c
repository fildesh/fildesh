/**
 * \file elastic.c
 * Echo stdin to stdout with an arbitrary sized buffer.
 **/
#define POSIX_SOURCE


#include <aio.h>
#include "cx/syscx.h"
#include "cx/table.h"

#include <errno.h>
#include <stdio.h>

//#define DEBUGGING

#ifndef DEBUGGING
#undef BailOut
#define BailOut(ret, msg)  (void) 0
#endif

int main (int argc, char** argv)
{
  int argi =
    (init_sysCx (&argc, &argv),
     1);
  int istat = 0;

  struct aiocb aio_x[1];
  struct aiocb aio_o[1];

  Bool pending_x = 1;
  Bool pending_o = 0;
  const ujint xbuf_inc = 1024;
  DeclTable( byte, xbuf );
  DeclTable( byte, obuf );
  ujint xbuf_off = 0;
  ujint obuf_off = 0;

  if (argi != 1) {
    fprintf (stderr, "%s: I do not take arguments from humans.\n", argv[0]);
    return 1;
  }

  memset (aio_x, 0, sizeof (*aio_x));
  memset (aio_o, 0, sizeof (*aio_o));

  aio_x->aio_fildes = 0;
  aio_o->aio_fildes = 1;

  GrowTable( xbuf, xbuf_inc );
  aio_x->aio_buf = xbuf.s;
  aio_x->aio_nbytes = xbuf.sz;
  istat = aio_read (aio_x);

  if (istat != 0) {
    BailOut( 1, "aio_read()" );
    return 0;
  }

  while (pending_x || pending_o) {
    ssize_t sstat;

    do {
      const struct aiocb* tmp[2] = { 0, 0 };
      if (pending_x)  tmp[0] = aio_x;
      if (pending_o)  tmp[1] = aio_o;
      istat = aio_suspend (tmp, 2, 0);
    } while (istat != 0 && errno == EINTR);

    if (istat != 0) {
      BailOut( 1, "aio_suspend()");
      break;
    }

    if (pending_o) {
      istat = aio_error (aio_o);
    }
    if (pending_o && istat == 0) {
      sstat = aio_return (aio_o);
      if (sstat < 0) {
        BailOut( 1, "aio_return(write)" );
        break;
      }
      else if (obuf_off + (ujint)sstat < obuf.sz) {
        obuf_off += (ujint) sstat;
        aio_o->aio_buf = &obuf.s[obuf_off];
        aio_o->aio_nbytes = obuf.sz - obuf_off;
        istat = aio_write (aio_o);
        if (istat != 0) {
          BailOut( 1, "aio_write()");
          break;
        }
      }
      else if (pending_x) {
        // Everything has been written.
        pending_o = 0;
      }
      else if (xbuf_off > 0) {
        SizeTable( xbuf, xbuf_off );
        CopyTable( obuf, xbuf );
        aio_o->aio_buf = &obuf.s[obuf_off];
        aio_o->aio_nbytes = obuf.sz - obuf_off;
        istat = aio_write (aio_o);
        if (istat != 0) {
          BailOut( 1, "aio_write()");
          break;
        }
        xbuf_off = 0;
      }
      else {
        // Everything has been written and reading has finished.
        break;
      }
    }
    else if (pending_o && istat != EINPROGRESS) {
      BailOut( 1, "aio_error(write)" );
      break;
    }

    if (pending_x) {
      istat = aio_error (aio_x);
    }
    if (pending_x && istat == 0) {
      sstat = aio_return (aio_x);
      if (sstat <= 0) {
        pending_x = 0;
      }
      else {
        xbuf_off += (ujint) sstat;
      }

      if (!pending_o && xbuf_off > 0) {
        SizeTable( xbuf, xbuf_off );
        CopyTable( obuf, xbuf );
        aio_o->aio_buf = &obuf.s[obuf_off];
        aio_o->aio_nbytes = obuf.sz - obuf_off;
        istat = aio_write (aio_o);
        if (istat != 0) {
          BailOut( 1, "aio_write()");
          break;
        }
        pending_o = 1;
        xbuf_off = 0;
      }

      if (!pending_x) {
        SizeTable( xbuf, 0 );
      }
      else {
        SizeTable( xbuf, xbuf_off + xbuf_inc );
        aio_x->aio_buf = &xbuf.s[xbuf_off];
        aio_x->aio_nbytes = xbuf.sz - xbuf_off;
        istat = aio_read (aio_x);
        if (istat != 0) {
          pending_x = 0;
        }
      }
    }
    else if (pending_x && istat != EINPROGRESS) {
      BailOut( 1, "aio_error(read)" );
      break;
    }
  }

  if (pending_x)  aio_cancel (0, aio_x);
  if (pending_o)  aio_cancel (1, aio_o);

  close (0);
  close (1);

  memset (aio_x, 0, sizeof (*aio_x));
  memset (aio_o, 0, sizeof (*aio_o));

  LoseTable( xbuf );
  LoseTable( obuf );

  lose_sysCx ();
  return 0;
}

