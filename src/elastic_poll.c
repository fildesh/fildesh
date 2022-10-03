/**
 * \file elastic_poll.c
 * Echo stdin to stdout with an arbitrary sized buffer.
 **/
#include "fildesh.h"
#include "fildesh_compat_fd.h"

#include <errno.h>
#include <fcntl.h>
#include <poll.h>
#include <stdio.h>
#include <string.h>
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
  DECLARE_FildeshAT(unsigned char, buf);
  bool done;
  const char* filename;
};


static
  IOState
default_IOState()
{
  IOState io;
  init_FildeshAT(io.buf);
  io.done = false;
  io.filename = NULL;
  return io;
}

static
  void
close_IOState(IOState* io, struct pollfd* pfd) {
  if (io->done) {
    return;
  }
  io->done = 1;
  fildesh_compat_fd_close(pfd->fd);
  close_FildeshAT(io->buf);
}

static
  bool
prepare_for_poll(IOState** ios, struct pollfd** pollfds) {
  unsigned i;
  bool still_reading = 0;

  for (i = 0; i < count_of_FildeshAT(ios); ++i) {
    IOState* io = &(*ios)[i];
    struct pollfd* pfd = &(*pollfds)[i];
    if (count_of_FildeshAT(io->buf) > 0 && !io->done) {
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

  for (i = 1; i < count_of_FildeshAT(ios); ++i) {
    IOState* io = &(*ios)[i];
    if (!io->done) {
      return 1;
    }
  }
  return 0;
}

static
  void
handle_read_write(IOState** ios, struct pollfd** pollfds) {
  unsigned i;
  for (i = 0; i < count_of_FildeshAT(ios); ++i) {
    IOState* io = &(*ios)[i];
    struct pollfd* pfd = &(*pollfds)[i];
    int rwerrno = 0;
    long sstat = 0;

    if (io->done) {
      /* Nothing.*/
    } else if ((pfd->revents & POLLIN) == POLLIN) {
      sstat = read(pfd->fd, (*io->buf), count_of_FildeshAT(io->buf));
      if (sstat < 0) {
        rwerrno = errno;
      } else if (sstat == 0) {
        StateMsg("No input read. Must be done!", 0, 0);
        close_IOState(io, pfd);
      } else /* sstat > 0 */ {
        unsigned j;
        for (j = 1; j < count_of_FildeshAT(ios); ++j) {
          const size_t effective_size = sstat;
          if ((*ios)[j].done) {
            continue;
          }
          memcpy(
              grow_FildeshAT((*ios)[j].buf, effective_size),
              *io->buf,
              effective_size);
        }
      }
    } else if ((pfd->revents & POLLOUT) == POLLOUT) {
      sstat = write(pfd->fd, *io->buf, count_of_FildeshAT(io->buf));
      if (sstat < 0) {
        rwerrno = errno;
      }
      if (sstat > 0) {
        if ((size_t)sstat < count_of_FildeshAT(io->buf)) {
          memmove(*io->buf, &(*io->buf)[sstat],
                  count_of_FildeshAT(io->buf) - sstat);
        }
        mpop_FildeshAT(io->buf, sstat);
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
  DECLARE_FildeshAT(IOState, ios);
  DECLARE_FildeshAT(struct pollfd, pollfds);
  unsigned i;
  int exstatus = 0;

  init_FildeshAT(ios);
  init_FildeshAT(pollfds);

  /* Initialize input.*/
  io = grow1_FildeshAT(ios);
  pfd = grow1_FildeshAT(pollfds);
  *io = default_IOState();
  grow_FildeshAT(io->buf, BUFSIZ);
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

      io = &(*ios)[0];
      pfd = &(*pollfds)[0];

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

      io = grow1_FildeshAT(ios);
      pfd = grow1_FildeshAT(pollfds);

      *io = default_IOState();
      io->filename = arg;
      pfd->fd = fildesh_arg_open_writeonly(io->filename);
      if (pfd->fd < 0) {
        fildesh_log_errorf("failed to -o: %s", io->filename);
        exstatus = 73;
      }
    }
  }

  if (exstatus == 0 && (*pollfds)[0].fd == -1) {
    io = &(*ios)[0];
    pfd = &(*pollfds)[0];
    io->filename = "/dev/stdin";
    pfd->fd = fildesh_arg_open_readonly("-");
    if (pfd->fd < 0) {exstatus = 66;}
  }

  if (exstatus == 0 && count_of_FildeshAT(pollfds) == 1) {
    io = grow1_FildeshAT(ios);
    pfd = grow1_FildeshAT(pollfds);
    *io = default_IOState();
    io->filename = "/dev/stdout";
    pfd->fd = fildesh_arg_open_writeonly("-");
    if (pfd->fd < 0) {exstatus = 73;}
  }
  /**** END ARGUMENT_PARSING ****/

  for (i = 0; i < count_of_FildeshAT(ios) && exstatus == 0; ++i) {
    io = &(*ios)[i];
    pfd = &(*pollfds)[i];
    istat = setfd_nonblock(pfd->fd);
    if (istat < 0) {
      fildesh_log_errorf("failed to set nonblocking: %s\n", io->filename);
      exstatus = 72;
    }
  }

  while (exstatus == 0 && prepare_for_poll(ios, pollfds)) {
    const int infinite_poll_timeout = -1;

    istat = poll(*pollfds, count_of_FildeshAT(pollfds), infinite_poll_timeout);
    if (istat < 0) {
      StateMsg( "poll()", 0, istat );
      break;
    }
    handle_read_write(ios, pollfds);
  }

  for (i = 0; i < count_of_FildeshAT(ios); ++i) {
    io = &(*ios)[i];
    pfd = &(*pollfds)[i];
    close_IOState(io, pfd);
    close_FildeshAT(io->buf);
  }
  close_FildeshAT(ios);
  close_FildeshAT(pollfds);
  return 0;
}

#ifndef FILDESH_BUILTIN_LIBRARY
  int
main(int argc, char** argv)
{
  return main_elastic_poll((unsigned)argc, argv);
}
#endif
