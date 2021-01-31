/**
 * \file elastic_poll.c
 * Echo stdin to stdout with an arbitrary sized buffer.
 **/
#include <poll.h>
#include "utilace.h"
#include "cx/alphatab.h"
#include "cx/table.h"

#include <errno.h>
#include <stdio.h>

/* #define DEBUGGING */

#ifdef DEBUGGING
#define StateMsg(msg, i, istat)  DBog3("%s %u %d", msg, i, istat)
#else
#define StateMsg(msg, i, istat)
#endif

typedef struct IOState IOState;

struct IOState
{
  TableT(byte) buf;
  Bool done;
  const char* filename;
};

DeclTableT(IOState, IOState);
DeclTableT(pollfd, struct pollfd);

static
  void
close_IOState(IOState* io, struct pollfd* pfd) {
  if (io->done) {
    return;
  }
  io->done = 1;
  closefd_sysCx(pfd->fd);
  ClearTable( io->buf );
}

static
  Bool
prepare_for_poll(TableT(IOState) ios, TableT(pollfd) pollfds) {
  uint i;
  Bool still_reading = 0;

  UFor( i, ios.sz ) {
    IOState* io = &ios.s[i];
    struct pollfd* pfd = &pollfds.s[i];
    if (io->buf.sz > 0 && !io->done) {
      if (i == 0) {
        pfd->events = POLLIN;
        still_reading = 1;
      } else {
        pfd->events = POLLOUT;
      }
    } else {
      pfd->events = 0;
      if (!still_reading) {
        close_IOState(io, pfd);
      }
    }
    pfd->revents = 0;
  }

  for (i = 1; i < ios.sz; ++i) {
    IOState* io = &ios.s[i];
    if (!io->done) {
      return 1;
    }
  }
  return 0;
}

static
  void
handle_read_write(TableT(IOState) ios, TableT(pollfd) pollfds) {
  uint i;

  UFor( i, ios.sz ) {
    IOState* io = &ios.s[i];
    struct pollfd* pfd = &pollfds.s[i];
    int rwerrno = 0;
    long sstat = 0;

    if (io->done) {
      /* Nothing.*/
    } else if ((pfd->revents & POLLIN) == POLLIN) {
      sstat = read_sysCx(pfd->fd, io->buf.s, io->buf.sz);
      if (sstat < 0) {
        rwerrno = errno;
      }
      if (sstat > 0) {
        size_t tmp_sz = io->buf.sz;
        uint j;
        io->buf.sz = sstat;
        for (j = 1; j < ios.sz; ++j) {
          if (ios.s[j].done) {
            continue;
          }
          CatTable( ios.s[j].buf, io->buf );
        }
        io->buf.sz = tmp_sz;
      }
    } else if ((pfd->revents & POLLOUT) == POLLOUT) {
      sstat = write_sysCx(pfd->fd, io->buf.s, io->buf.sz);
      if (sstat < 0) {
        rwerrno = errno;
      }
      if (sstat > 0) {
        if ((size_t)sstat < io->buf.sz) {
          memmove(io->buf.s, &io->buf.s[sstat], io->buf.sz - sstat);
        }
        io->buf.sz -= sstat;
      }
    }

    pfd->revents &= ~(POLLIN | POLLOUT);
    if (pfd->revents != 0) {
      /* Something happened.*/
      StateMsg("revents", i, pfd->revents);
      close_IOState(io, pfd);
    }

    if (io->done || rwerrno == 0 || rwerrno == EINTR ||
        rwerrno == EAGAIN || rwerrno == EWOULDBLOCK) {
      if (!io->done) {
        /* Nothing.*/
        StateMsg("ok", i, rwerrno);
      }
    } else {
      errno = rwerrno;
      StateMsg("sstat", i, rwerrno);
      errno = 0;
      close_IOState(io, pfd);
    }
  }
}


LaceUtilMain(elastic)
{
  int istat = 0;
  IOState* io;
  struct pollfd* pfd;
  DeclTable( IOState, ios );
  DeclTable( pollfd, pollfds );
  uint i;

  io = Grow1Table( ios );
  pfd = Grow1Table( pollfds );

  Zeroize( *io );
  io->filename = "/dev/stdin";
  GrowTable( io->buf, BUFSIZ );
  pfd->fd = 0;

  if (argi == argc) {
    io = Grow1Table( ios );
    pfd = Grow1Table(pollfds);
    Zeroize( *io );
    io->filename = "/dev/stdout";
    pfd->fd = 1;
  }
  while (argi < argc) {
    io = Grow1Table( ios );
    pfd = Grow1Table( pollfds );

    Zeroize( *io );
    io->filename = argv[argi++];

    if (eq_cstr (io->filename, "-")) {
      pfd->fd = 1;
    }
    else {
      pfd->fd = open_lace_ofd(io->filename);
    }
    if (pfd->fd < 0) {
      fprintf (stderr, "%s: failed to open: %s\n", argv[0], io->filename);
      lose_sysCx ();
      return 1;
    }
  }

  for (i = 0; i < ios.sz; ++i) {
    io = &ios.s[i];
    pfd = &pollfds.s[i];
    istat = fcntl(pfd->fd, F_GETFD);
    if (istat < 0) {
      fprintf(stderr, "%s: failed to get flags on: %s\n", argv[0], io->filename);
      lose_sysCx ();
      return 1;
    }
    istat = fcntl(pfd->fd, F_SETFD, istat | O_NONBLOCK);
    if (istat < 0) {
      fprintf(stderr, "%s: failed to set O_NONBLOCK on: %s\n", argv[0], io->filename);
      lose_sysCx ();
      return 1;
    }
  }

  while (prepare_for_poll(ios, pollfds)) {
    const int infinite_poll_timeout = -1;

    istat = poll(pollfds.s, pollfds.sz, infinite_poll_timeout);
    if (istat < 0) {
      StateMsg( "poll()", 0, istat );
      break;
    }
    handle_read_write(ios, pollfds);
  }

  UFor( i, ios.sz ) {
    io = &ios.s[i];
    pfd = &pollfds.s[i];
    close_IOState(io, pfd);
    LoseTable( io->buf );
  }
  LoseTable( ios );
  LoseTable( pollfds );

  lose_sysCx ();
  return 0;
}

