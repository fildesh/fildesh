
#include "lace_compat_fd.h"

#include <assert.h>
#include <fcntl.h>

#ifdef _WIN32
#include <windows.h>
#include <io.h>
#else
#include <unistd.h>
#endif

  int
lace_compat_fd_cloexec(int fd)
{
#ifdef _WIN32
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
#ifdef _WIN32
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
#ifdef _WIN32
  istat = _close(fd);
#else
  istat = close(fd);
#endif
  if (istat != 0) {return -1;}
  return 0;
}

  lace_compat_fd_t
lace_compat_fd_duplicate(lace_compat_fd_t fd)
{
  int newfd;
#ifdef _WIN32
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
#ifdef _WIN32
    int istat = _dup2(oldfd, dst);
#else
    int istat = dup2(oldfd, dst);
#endif
    if (istat < 0) {return -1;}
    if (0 != lace_compat_fd_close(oldfd)) {return -1;}
  }
  if (0 != lace_compat_fd_cloexec(dst)) {return -1;}
  return 0;
}

  int
lace_compat_fd_pipe(lace_compat_fd_t* ret_produce,
                    lace_compat_fd_t* ret_consume)
{
  int fds[2] = {-1, -1};
  int istat;
#ifdef _WIN32
  istat = _pipe(fds, 0, 0);
#else
  istat = pipe(fds);
#endif
  *ret_produce = fds[1];
  *ret_consume = fds[0];
  if (istat != 0) {return -1;}
  if (0 != lace_compat_fd_cloexec(fds[0])) {return -1;}
  if (0 != lace_compat_fd_cloexec(fds[1])) {return -1;}
  return 0;
}

  size_t
lace_compat_fd_write(lace_compat_fd_t fd, const void* data, size_t data_size)
{
#ifdef _WIN32
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
#ifdef _WIN32
  long n = _read(fd, buf, buf_capacity);
#else
  ssize_t n = read(fd, buf, buf_capacity);
#endif
  if (n <= 0) {return 0;}
  return (size_t)n;
}
