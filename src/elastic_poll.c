/**
 * \file elastic_poll.c
 * Echo stdin to stdout with an arbitrary sized buffer.
 **/
#include "cx/syscx.h"
#include "cx/alphatab.h"
#include "cx/table.h"

#include <errno.h>
#include <poll.h>
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
  bool done;
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
  bool
prepare_for_poll(TableT(IOState) ios, TableT(pollfd) pollfds) {
  uint i;
  bool still_reading = 0;

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
      } else if (sstat == 0) {
        StateMsg("No input read. Must be done!", 0, 0);
        close_IOState(io, pfd);
      } else /* sstat > 0 */ {
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

static
  int
setfd_nonblock_sysCx(fd_t fd)
{
  int istat;
  istat = fcntl(fd, F_GETFD);
  if (istat < 0) {
    return istat;
  }
  return fcntl(fd, F_SETFD, istat | O_NONBLOCK);
}

  int
main_elastic_poll(int argi, int argc, char** argv)
{
  int istat = 0;
  IOState* io;
  struct pollfd* pfd;
  DeclTable( IOState, ios );
  DeclTable( pollfd, pollfds );
  uint i;

  /* Initialize input.*/
  io = Grow1Table( ios );
  pfd = Grow1Table( pollfds );
  Zeroize( *io );
  GrowTable( io->buf, BUFSIZ );
  pfd->fd = -1;

  /**** BEGIN ARGUMENT_PARSING ****/
  while (argi < argc) {
    const char* arg = argv[argi++];

    if (eq_cstr(arg, "-x")) {
      if (argi == argc) {
        failout_sysCx("Need input file after -x.");
      }
      arg = argv[argi++];

      io = &ios.s[0];
      pfd = &pollfds.s[0];

      io->filename = arg;
      pfd->fd = open_lace_xfd(io->filename);
    } else {
      if (eq_cstr(arg, "-o")) {
        if (argi == argc) {
          failout_sysCx("Need input file after -o.");
        }
        arg = argv[argi++];
      }

      io = Grow1Table( ios );
      pfd = Grow1Table( pollfds );

      Zeroize( *io );
      io->filename = arg;
      pfd->fd = open_lace_ofd(io->filename);
    }

    if (pfd->fd < 0) {
      DBog1("failed to open: %s\n", io->filename);
      failout_sysCx(0);
    }
  }

  if (pollfds.s[0].fd == -1) {
    io = &ios.s[0];
    pfd = &pollfds.s[0];
    io->filename = "/dev/stdin";
    pfd->fd = open_lace_xfd("-");
  }

  if (pollfds.sz == 1) {
    io = Grow1Table( ios );
    pfd = Grow1Table(pollfds);
    Zeroize( *io );
    io->filename = "/dev/stdout";
    pfd->fd = open_lace_ofd("-");
  }
  /**** END ARGUMENT_PARSING ****/

  for (i = 0; i < ios.sz; ++i) {
    io = &ios.s[i];
    pfd = &pollfds.s[i];
    istat = setfd_nonblock_sysCx(pfd->fd);
    if (istat < 0) {
      DBog1("failed to set nonblocking: %s\n", io->filename);
      failout_sysCx(0);
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
  return 0;
}

#ifdef MAIN_LACE_EXECUTABLE
int main_elastic(int argi, int argc, char** argv) {
  return main_elastic_poll(argi, argc, argv);
}
#else
  int
main(int argc, char** argv)
{
  int argi = init_sysCx(&argc, &argv);
  int istat = main_elastic_poll(argi, argc, argv);
  lose_sysCx();
  return istat;
}
#endif
