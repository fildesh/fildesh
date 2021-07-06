
#include "lace_compat_fd.h"
#include "lace_compat_errno.h"
#include "lace_compat_sh.h"

#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <stdarg.h>

#ifdef _MSC_VER
#include <windows.h>
#include <io.h>
#else
#include <unistd.h>
#endif

  int
lace_compat_fd_cloexec(int fd)
{
#ifdef _MSC_VER
  BOOL success = SetHandleInformation(
      (HANDLE)_get_osfhandle(fd), HANDLE_FLAG_INHERIT, 0);
  if (!success) {return -1;}
#else
  int istat;
  int flags = fcntl(fd, F_GETFD);
  if (flags == -1) {return -1;}
  if (0 != (flags & FD_CLOEXEC)) {return 0;}
  flags |= FD_CLOEXEC;
  istat = fcntl(fd, F_SETFD, flags);
  if (istat == -1) {return -1;}
#endif
  return 0;
}

  int
lace_compat_fd_inherit(int fd)
{
#ifdef _MSC_VER
  BOOL success = SetHandleInformation(
      (HANDLE)_get_osfhandle(fd), HANDLE_FLAG_INHERIT, 1);
  if (!success) {return -1;}
#else
  int istat;
  int flags = fcntl(fd, F_GETFD);
  if (flags == -1) {return -1;}
  if (0 == (flags & FD_CLOEXEC)) {return 0;}
  flags &= ~FD_CLOEXEC;
  istat = fcntl(fd, F_SETFD, flags);
  if (istat == -1) {return -1;}
#endif
  return 0;
}

  int
lace_compat_fd_close(lace_compat_fd_t fd)
{
  int istat;
  assert(fd >= 0);
#ifdef _MSC_VER
  istat = _close(fd);
#else
  istat = close(fd);
#endif
  if (istat != 0) {
    int e = lace_compat_errno_trace();
    /* File descriptor should be valid.*/
    assert(istat == 0 || e != EBADF);
  }
  if (istat != 0) {return -1;}
  return 0;
}

static
  lace_compat_fd_t
lace_compat_fd_duplicate(lace_compat_fd_t fd)
{
  int newfd;
#ifdef _MSC_VER
  newfd = _dup(fd);
#else
  newfd = dup(fd);
#endif
  if (0 > newfd) {return -1;}
  if (0 != lace_compat_fd_cloexec(newfd)) {return -1;}
  return newfd;
}

  int
lace_compat_fd_move_to(lace_compat_fd_t dst, lace_compat_fd_t oldfd)
{
  if (dst != oldfd) {
#ifdef _MSC_VER
    int istat = _dup2(oldfd, dst);
#else
    int istat = dup2(oldfd, dst);
#endif
    if (istat < 0) {lace_compat_fd_close(oldfd); return -1;}
    if (0 != lace_compat_fd_close(oldfd)) {lace_compat_fd_close(dst); return -1;}
  }
  return 0;
}

  lace_compat_fd_t
lace_compat_fd_move_off_stdio(lace_compat_fd_t fd)
{
  unsigned i;
  lace_compat_fd_t fds[4] = {-1, -1, -1, -1};
  if (fd < 0 || fd > 2) {return fd;}
  fds[0] = fd;
  for (i = 1; i < 4; ++i) {
    fds[i] = lace_compat_fd_duplicate(fd);
    if (fds[i] < 0 || fds[i] > 2) {
      fd = fds[i];
      fds[i] = -1;
      break;
    }
  }
  for (i = 0; i < 4 && fds[i] >= 0; ++i) {
    lace_compat_fd_close(fds[i]);
  }
  if (fd >= 0) {
    lace_compat_fd_cloexec(fd);
  }
  return fd;
}

  lace_compat_fd_t
lace_compat_fd_reserve()
{
  int fds[2] = {-1, -1};
  int istat;
#ifdef _MSC_VER
  istat = _pipe(fds, 0, 0);
#else
  istat = pipe(fds);
#endif
  if (istat < 0) {return istat;}
  if (fds[0] > 2) {
    lace_compat_fd_close(fds[1]);
    lace_compat_fd_cloexec(fds[0]);
    return fds[0];
  }
  if (fds[1] > 2) {
    lace_compat_fd_close(fds[0]);
    lace_compat_fd_cloexec(fds[1]);
    return fds[1];
  }

  fds[0] = lace_compat_fd_move_off_stdio(fds[0]);
  lace_compat_fd_close(fds[1]);
  if (fds[0] >= 0) {lace_compat_fd_cloexec(fds[0]);}
  return fds[0];
}

  int
lace_compat_fd_pipe(lace_compat_fd_t* ret_produce,
                    lace_compat_fd_t* ret_consume)
{
  int fds[2] = {-1, -1};
  int istat;
#ifdef _MSC_VER
  istat = _pipe(fds, 0, 0);
#else
  istat = pipe(fds);
#endif

  if (istat == 0 && fds[0] <= 2) {
    fds[0] = lace_compat_fd_move_off_stdio(fds[0]);
  } else if (istat == 0 && fds[0] > 2) {
    istat = lace_compat_fd_cloexec(fds[0]);
  }
  if (istat == 0 && fds[1] <= 2) {
    fds[1] = lace_compat_fd_move_off_stdio(fds[1]);
  } else if (istat == 0 && fds[1] > 2) {
    istat = lace_compat_fd_cloexec(fds[1]);
  }

  if (istat != 0 || fds[0] < 0 || fds[1] < 0) {
    *ret_produce = -1;
    *ret_consume = -1;
    if (fds[0] >= 0) {lace_compat_fd_close(fds[0]);}
    if (fds[1] >= 0) {lace_compat_fd_close(fds[1]);}
    return -1;
  }

  *ret_produce = fds[1];
  *ret_consume = fds[0];
  return 0;
}

  size_t
lace_compat_fd_write(lace_compat_fd_t fd, const void* data, size_t data_size)
{
#ifdef _MSC_VER
  long n = _write(fd, data, data_size);
#else
  ssize_t n = write(fd, data, data_size);
#endif
  if (n <= 0) {return 0;}
  return (size_t)n;
}

  size_t
lace_compat_fd_read(lace_compat_fd_t fd, void* buf, size_t buf_capacity)
{
#ifdef _MSC_VER
  long n = _read(fd, buf, buf_capacity);
#else
  ssize_t n = read(fd, buf, buf_capacity);
#endif
  if (n <= 0) {return 0;}
  return (size_t)n;
}


  lace_compat_pid_t
lace_compat_fd_spawnvp(const lace_compat_fd_t* fds_to_inherit,
                       const char* const* argv)
{
  lace_compat_pid_t pid;
  int e = 0;
  unsigned i;
  if (fds_to_inherit) {
    for (i = 0; fds_to_inherit[i] >= 0; ++i) {
      lace_compat_fd_inherit(fds_to_inherit[i]);
    }
  }

  pid = lace_compat_sh_spawn(argv);
  if (pid < 0) {
    e = lace_compat_errno_trace();
  }

  if (fds_to_inherit) {
    for (i = 0; fds_to_inherit[i] >= 0; ++i) {
      lace_compat_fd_close(fds_to_inherit[i]);
    }
  }
  if (pid < 0) {
    errno = e;
    return -1;
  }
  return pid;
}

  lace_compat_pid_t
lace_compat_fd_spawnlp(const lace_compat_fd_t* fds_to_inherit,
                       const char* cmd, ...)
{
  const unsigned max_argc = 50;
  const char* argv[51];
  va_list argp;
  unsigned i;

  if (!cmd) {return -1;}
  argv[0] = cmd;
  va_start(argp, cmd);
  for (i = 1; i <= max_argc && argv[i-1]; ++i) {
    argv[i] = va_arg(argp, const char*);
  }
  va_end(argp);
  if (argv[i-1]) {return -1;}
  return lace_compat_fd_spawnvp(fds_to_inherit, argv);
}

  int
lace_compat_fd_spawnvp_wait(const lace_compat_fd_t* fds_to_inherit, const char* const* argv)
{
  lace_compat_pid_t pid = lace_compat_fd_spawnvp(fds_to_inherit, argv);
  if (pid < 0) {return -1;}
  return lace_compat_sh_wait(pid);
}

  int
lace_compat_fd_spawnlp_wait(const lace_compat_fd_t* fds_to_inherit,
                            const char* cmd, ...)
{
  const unsigned max_argc = 50;
  const char* argv[51];
  va_list argp;
  unsigned i;

  if (!cmd) {return -1;}
  argv[0] = cmd;
  va_start(argp, cmd);
  for (i = 1; i <= max_argc && argv[i-1]; ++i) {
    argv[i] = va_arg(argp, const char*);
  }
  va_end(argp);
  if (argv[i-1]) {return -1;}
  return lace_compat_fd_spawnvp_wait(fds_to_inherit, argv);
}
