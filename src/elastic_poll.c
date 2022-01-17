/**
 * \file elastic_poll.c
 * Echo stdin to stdout with an arbitrary sized buffer.
 **/
#include "fildesh.h"
#include "fildesh_compat_fd.h"
#include "cx/table.h"

#include <errno.h>
#include <fcntl.h>
#include <poll.h>
#include <stdio.h>
#include <unistd.h>

/* #define DEBUGGING */

#ifdef DEBUGGING
#define StateMsg(msg, i, istat)  fildesh_log_tracef("%s %u %d", msg, i, istat)
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
  fildesh_compat_fd_close(pfd->fd);
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
      sstat = read(pfd->fd, io->buf.s, io->buf.sz);
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
      sstat = write(pfd->fd, io->buf.s, io->buf.sz);
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
setfd_nonblock(fildesh_fd_t fd)
{
  int istat;
  istat = fcntl(fd, F_GETFD);
  if (istat < 0) {
    return istat;
  }
  return fcntl(fd, F_SETFD, istat | O_NONBLOCK);
}

  int
main_elastic_poll(unsigned argc, char** argv)
{
  unsigned argi = 1;
  int istat = 0;
  IOState* io;
  struct pollfd* pfd;
  DeclTable( IOState, ios );
  DeclTable( pollfd, pollfds );
  uint i;
  int exstatus = 0;

  /* Initialize input.*/
  io = Grow1Table( ios );
  pfd = Grow1Table( pollfds );
  Zeroize( *io );
  GrowTable( io->buf, BUFSIZ );
  pfd->fd = -1;

  /**** BEGIN ARGUMENT_PARSING ****/
  while (argi < argc && exstatus == 0) {
    const char* arg = argv[argi++];

    if (0 == strcmp(arg, "-x")) {
      if (argi == argc) {
        fildesh_log_error("Need input file after -x.");
        exstatus = 64;
        break;
      }
      arg = argv[argi++];

      io = &ios.s[0];
      pfd = &pollfds.s[0];

      io->filename = arg;
      pfd->fd = fildesh_arg_open_readonly(io->filename);
      if (pfd->fd < 0) {
        fildesh_log_errorf("failed to open -x: %s", io->filename);
        exstatus = 66;
      }
    } else {
      if (0 == strcmp(arg, "-o")) {
        if (argi == argc) {
          fildesh_log_error("Need output file after -o.");
          exstatus = 64;
          break;
        }
        arg = argv[argi++];
      }

      io = Grow1Table( ios );
      pfd = Grow1Table( pollfds );

      Zeroize( *io );
      io->filename = arg;
      pfd->fd = fildesh_arg_open_writeonly(io->filename);
      if (pfd->fd < 0) {
        fildesh_log_errorf("failed to -o: %s", io->filename);
        exstatus = 73;
      }
    }
  }

  if (exstatus == 0 && pollfds.s[0].fd == -1) {
    io = &ios.s[0];
    pfd = &pollfds.s[0];
    io->filename = "/dev/stdin";
    pfd->fd = fildesh_arg_open_readonly("-");
    if (pfd->fd < 0) {exstatus = 66;}
  }

  if (exstatus == 0 && pollfds.sz == 1) {
    io = Grow1Table( ios );
    pfd = Grow1Table(pollfds);
    Zeroize( *io );
    io->filename = "/dev/stdout";
    pfd->fd = fildesh_arg_open_writeonly("-");
    if (pfd->fd < 0) {exstatus = 73;}
  }
  /**** END ARGUMENT_PARSING ****/

  for (i = 0; i < ios.sz && exstatus == 0; ++i) {
    io = &ios.s[i];
    pfd = &pollfds.s[i];
    istat = setfd_nonblock(pfd->fd);
    if (istat < 0) {
      fildesh_log_errorf("failed to set nonblocking: %s\n", io->filename);
      exstatus = 72;
    }
  }

  while (exstatus == 0 && prepare_for_poll(ios, pollfds)) {
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

#ifndef FILDESH_BUILTIN_LIBRARY
  int
main(int argc, char** argv)
{
  return main_elastic_poll((unsigned)argc, argv);
}
#endif
