
#include "fildesh_compat_fd.h"
#include "fildesh_compat_errno.h"
#include "fildesh_compat_sh.h"

#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <stdarg.h>
#include <sys/stat.h>

#ifndef _MSC_VER
#include <unistd.h>
#else
#include <windows.h>
#include <io.h>
/* For _CrtSetReportMode().*/
#include <crtdbg.h>


static void invalid_parameter_noop_handler(
    const wchar_t* expression,
    const wchar_t* function,
    const wchar_t* file,
    unsigned int line,
    uintptr_t reserved)
{
  (void) expression;
  (void) function;
  (void) file;
  (void) line;
  (void) reserved;
  /* Nothing.*/
}
#endif

/* Give my lock!*/
#include "fd_exclusive.h"


static
  int
lace_compat_fd_cloexec(int fd)
{
#ifndef _MSC_VER
  int istat;
  int flags = fcntl(fd, F_GETFD);
  if (flags == -1) {return -1;}
  if (0 != (flags & FD_CLOEXEC)) {return 0;}
  flags |= FD_CLOEXEC;
  istat = fcntl(fd, F_SETFD, flags);
  if (istat == -1) {return -1;}
#else
  BOOL success = SetHandleInformation(
      (HANDLE)_get_osfhandle(fd), HANDLE_FLAG_INHERIT, 0);
  if (!success) {return -1;}
#endif
  return 0;
}

static
  int
lace_compat_fd_inherit(int fd)
{
#ifndef _MSC_VER
  int istat;
  int flags = fcntl(fd, F_GETFD);
  if (flags == -1) {return -1;}
  if (0 == (flags & FD_CLOEXEC)) {return 0;}
  flags &= ~FD_CLOEXEC;
  istat = fcntl(fd, F_SETFD, flags);
  if (istat == -1) {return -1;}
#else
  BOOL success = SetHandleInformation(
      (HANDLE)_get_osfhandle(fd), HANDLE_FLAG_INHERIT, 1);
  if (!success) {return -1;}
#endif
  return 0;
}

  int
lace_compat_fd_close(lace_compat_fd_t fd)
{
  int istat;
  assert(fd >= 0);
#ifndef _MSC_VER
  istat = close(fd);
#else
  istat = _close(fd);
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
#ifndef _MSC_VER
  newfd = dup(fd);
#else
  const _invalid_parameter_handler old_handler =
    _set_thread_local_invalid_parameter_handler(invalid_parameter_noop_handler);
  const int old_report_mode = _CrtSetReportMode(_CRT_ASSERT, 0);
  newfd = _dup(fd);
  _CrtSetReportMode(_CRT_ASSERT, old_report_mode);
  _set_thread_local_invalid_parameter_handler(old_handler);
#endif
  if (0 > newfd) {return -1;}
  if (0 != lace_compat_fd_cloexec(newfd)) {return -1;}
  return newfd;
}

static int
lace_compat_fd_copy_to(lace_compat_fd_t dst, lace_compat_fd_t src)
{
  int istat;
  if (dst == src) {return 0;}
#ifndef _MSC_VER
  istat = dup2(src, dst);
#else
  istat = _dup2(src, dst);
#endif
  return (istat >= 0 ? 0 : -1);
}

  int
lace_compat_fd_move_to(lace_compat_fd_t dst, lace_compat_fd_t oldfd)
{
  int istat = 0;
  assert(dst >= 0);
  if (dst == oldfd) {return 0;}
  /* Acquire exclusive lock because there could be stdio.*/
  LACE_COMPAT_FD_ENTER_EXCLUSIVE;

  istat = lace_compat_fd_copy_to(dst, oldfd);
  if (0 != lace_compat_fd_close(oldfd)) {
    if (istat == 0) {
      lace_compat_fd_close(dst);
    }
    istat = -1;
  }

  if (istat >= 0) {
    if (dst <= 2) {
      istat = lace_compat_fd_inherit(dst);
    } else {
      istat = lace_compat_fd_cloexec(dst);
    }
    if (istat != 0) {lace_compat_fd_close(dst);}
  }

  LACE_COMPAT_FD_LEAVE_EXCLUSIVE;
  return istat;
}

static
  lace_compat_fd_t
lace_compat_fd_move_off_stdio(lace_compat_fd_t fd)
{
  unsigned i;
  int e = 0;
  lace_compat_fd_t fds[4] = {-1, -1, -1, -1};
  if (fd < 0 || fd > 2) {return fd;}
  fds[0] = fd;
  for (i = 1; i < 4; ++i) {
    fds[i] = lace_compat_fd_duplicate(fd);
    if (fds[i] > 2) {
      /* We'll return this file descriptor; avoid closing it.*/
      fd = fds[i];
      fds[i] = -1;
      break;
    }
    if (fds[i] < 0) {
      e = lace_compat_errno_trace();
      if (e == EBADF) {
        /* Original file descriptor is bad; avoid closing it.*/
        fds[0] = fds[i-1];
        fds[i-1] = -1;
      }
      fd = -1;
      break;
    }
  }
  for (i = 0; i < 4 && fds[i] >= 0; ++i) {
    lace_compat_fd_close(fds[i]);
  }
  if (fd < 0) {
    errno = e;
  }
  return fd;
}

  lace_compat_fd_t
lace_compat_fd_claim(lace_compat_fd_t fd)
{
  LACE_COMPAT_FD_ENTER_SHARED;
  fd = lace_compat_fd_move_off_stdio(fd);
  if (fd >= 0) {
    int istat = lace_compat_fd_cloexec(fd);
    assert(istat == 0 || errno != EBADF);
    if (istat != 0) {lace_compat_errno_trace();}
  }
#ifdef _MSC_VER
  if (fd >= 0) {
    _setmode(fd, _O_BINARY);
  }
#endif
  LACE_COMPAT_FD_LEAVE_SHARED;
  return fd;
}

  lace_compat_fd_t
lace_compat_file_open_readonly(const char* filename)
{
  lace_compat_fd_t fd;
  int flags;
  LACE_COMPAT_FD_ENTER_SHARED;
#ifndef _MSC_VER
  flags = (
      O_RDONLY |
      0 /* O_CLOEXEC is below */);
  fd = open(filename, flags);
#else
  flags = (
      _O_RDONLY |
      _O_BINARY |
      _O_NOINHERIT);
  fd = _open(filename, flags);
#endif
  fd = lace_compat_fd_move_off_stdio(fd);
  if (fd >= 0) {
    lace_compat_fd_cloexec(fd);
  }
  LACE_COMPAT_FD_LEAVE_SHARED;
  return fd;
}

  lace_compat_fd_t
lace_compat_file_open_writeonly(const char* filename)
{
  lace_compat_fd_t fd;
  int flags, mode;
  LACE_COMPAT_FD_ENTER_SHARED;
#ifndef _MSC_VER
  flags = (
      O_WRONLY | O_CREAT | O_TRUNC |
      O_APPEND |
      0 /* O_CLOEXEC is below */);
  mode
    = S_IWUSR | S_IWGRP | S_IWOTH
    | S_IRUSR | S_IRGRP | S_IROTH;
  fd = open(filename, flags, mode);
#else
  flags = (
      _O_WRONLY | _O_CREAT | _O_TRUNC |
      _O_APPEND |
      _O_BINARY |
      _O_NOINHERIT);
  mode = _S_IREAD | _S_IWRITE;
  fd = _open(filename, flags, mode);
#endif
  fd = lace_compat_fd_move_off_stdio(fd);
  if (fd >= 0) {
    lace_compat_fd_cloexec(fd);
  }
  LACE_COMPAT_FD_LEAVE_SHARED;
  return fd;
}

  int
lace_compat_fd_pipe(lace_compat_fd_t* ret_produce,
                    lace_compat_fd_t* ret_consume)
{
  int fds[2] = {-1, -1};
  int istat;
  LACE_COMPAT_FD_ENTER_SHARED;

#ifndef _MSC_VER
  istat = pipe(fds);
#else
  istat = _pipe(fds, 0, _O_BINARY);
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
    if (fds[0] >= 0) {lace_compat_fd_close(fds[0]);}
    if (fds[1] >= 0) {lace_compat_fd_close(fds[1]);}
    fds[0] = -1;
    fds[1] = -1;
    istat = -1;
  }

  *ret_produce = fds[1];
  *ret_consume = fds[0];
  LACE_COMPAT_FD_LEAVE_SHARED;
  return istat;
}

  size_t
lace_compat_fd_write(lace_compat_fd_t fd, const void* data, size_t data_size)
{
#ifndef _MSC_VER
  ssize_t n = write(fd, data, data_size);
#else
  long n = _write(fd, data, data_size);
#endif
  if (n <= 0) {return 0;}
  return (size_t)n;
}

  size_t
lace_compat_fd_read(lace_compat_fd_t fd, void* buf, size_t buf_capacity)
{
#ifndef _MSC_VER
  ssize_t n = read(fd, buf, buf_capacity);
#else
  long n = _read(fd, buf, buf_capacity);
#endif
  if (n <= 0) {return 0;}
  return (size_t)n;
}

static int lace_compat_fd_inherit_empty_stdio(int fd)
{
  int fds[2] = {-1, -1};
  int istat;
#ifndef _MSC_VER
  istat = pipe(fds);
#else
  istat = _pipe(fds, 0, 0);
#endif
  if (istat != 0) {return -1;}
  if (fd == 0) {
    istat = lace_compat_fd_copy_to(0, fds[0]);
  } else {
    istat = lace_compat_fd_copy_to(fd, fds[1]);
  }
  if (istat != 0 || fds[0] != fd) {
    lace_compat_fd_close(fds[0]);
  }
  if (istat != 0 || fds[1] != fd) {
    lace_compat_fd_close(fds[1]);
  }
  return istat;
}

static int
lace_compat_fd_spawnvp_setup_stdio(lace_compat_fd_t stdin_fd,
                                   lace_compat_fd_t stdout_fd,
                                   lace_compat_fd_t stderr_fd)
{
  int istat = 0;
  if (stdin_fd < 0) {
    istat = lace_compat_fd_inherit_empty_stdio(0);
  } else if (stdin_fd == 0) {
    /* Nothing. Already inherited.*/
  } else {
    assert(stdin_fd >= 3);
    istat = lace_compat_fd_copy_to(0, stdin_fd);
    if (istat != 0) {return -100;}
    istat = lace_compat_fd_close(stdin_fd);
  }
  if (istat != 0) {return -101;}
  if (stdout_fd < 0) {
    istat = lace_compat_fd_inherit_empty_stdio(1);
  } else if (stdout_fd == 1) {
    /* Nothing. Already inherited.*/
  } else if (stdout_fd == 2) {
    assert(stderr_fd == 2);
    istat = lace_compat_fd_copy_to(1, 2);
  } else {
    assert(stdout_fd >= 3);
    istat = lace_compat_fd_copy_to(1, stdout_fd);
    if (istat != 0) {return -110;}
    istat = lace_compat_fd_close(stdout_fd);
  }
  if (istat != 0) {return -111;}
  if (stderr_fd < 0) {
    istat = lace_compat_fd_inherit_empty_stdio(2);
  } else if (stderr_fd == 1) {
    assert(stdout_fd == 1);
    istat = lace_compat_fd_copy_to(2, 1);
  } else if (stderr_fd == 2) {
    /* Nothing. Already inherited.*/
  } else {
    assert(stderr_fd >= 3);
    istat = lace_compat_fd_copy_to(2, stderr_fd);
    if (istat != 0) {return -120;}
    istat = lace_compat_fd_close(stderr_fd);
  }
  if (istat != 0) {return -121;}
  return 0;
}

static void
lace_compat_fd_spawnvp_cleanup_stdio(lace_compat_fd_t stdin_fd,
                                     lace_compat_fd_t stdout_fd,
                                     lace_compat_fd_t stderr_fd,
                                     int status)
{
  if (status != -100 && stdin_fd != 0) {
    lace_compat_fd_close(0);
  }
  if (status > -110 && status <= -100) {
    lace_compat_fd_close(stdin_fd);
  }
  if (status != -110 && stdout_fd != 1) {
    lace_compat_fd_close(1);
  }
  if (status > -120 && status <= -100) {
    lace_compat_fd_close(stdout_fd);
  }
  if (status != -120 && stderr_fd != 2) {
    lace_compat_fd_close(2);
  }
  if (status > -130 && status <= -100) {
    lace_compat_fd_close(stderr_fd);
  }
}

  lace_compat_pid_t
lace_compat_fd_spawnvp(lace_compat_fd_t stdin_fd,
                       lace_compat_fd_t stdout_fd,
                       lace_compat_fd_t stderr_fd,
                       const lace_compat_fd_t* fds_to_inherit,
                       const char* const* argv)
{
  lace_compat_pid_t pid = -1;
  int e = 0;
  int istat = 0;
  unsigned i;
  LACE_COMPAT_FD_ENTER_EXCLUSIVE;
  if (fds_to_inherit) {
    for (i = 0; fds_to_inherit[i] >= 0 && istat == 0; ++i) {
      assert(fds_to_inherit[i] >= 3);
      istat = lace_compat_fd_inherit(fds_to_inherit[i]);
      if (fds_to_inherit[i] == fds_to_inherit[i+1]) {
        ++i;  /* No need to inherit twice.*/
      }
    }
  }

  if (istat == 0) {
    istat = lace_compat_fd_spawnvp_setup_stdio(stdin_fd, stdout_fd, stderr_fd);
    if (istat == 0) {
      pid = lace_compat_sh_spawn(argv);
      if (pid < 0) {
        e = lace_compat_errno_trace();
      }
      lace_compat_fd_spawnvp_cleanup_stdio(stdin_fd, stdout_fd, stderr_fd, istat);
    }
  }

  if (fds_to_inherit) {
    for (i = 0; fds_to_inherit[i] >= 0; ++i) {
      if (fds_to_inherit[i] == fds_to_inherit[i+1]) {
        lace_compat_fd_cloexec(fds_to_inherit[i]);
        ++i;
      } else {
        lace_compat_fd_close(fds_to_inherit[i]);
      }
    }
  }
  LACE_COMPAT_FD_LEAVE_EXCLUSIVE;
  if (pid < 0) {
    errno = e;
    return -1;
  }
  return pid;
}

  lace_compat_pid_t
lace_compat_fd_spawnlp(lace_compat_fd_t stdin_fd,
                       lace_compat_fd_t stdout_fd,
                       lace_compat_fd_t stderr_fd,
                       const lace_compat_fd_t* fds_to_inherit,
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
  return lace_compat_fd_spawnvp(
      stdin_fd, stdout_fd, stderr_fd, fds_to_inherit, argv);
}

  int
lace_compat_fd_spawnvp_wait(lace_compat_fd_t stdin_fd,
                            lace_compat_fd_t stdout_fd,
                            lace_compat_fd_t stderr_fd,
                            const lace_compat_fd_t* fds_to_inherit,
                            const char* const* argv)

{
  lace_compat_pid_t pid = lace_compat_fd_spawnvp(
      stdin_fd, stdout_fd, stderr_fd, fds_to_inherit, argv);
  if (pid < 0) {return -1;}
  return lace_compat_sh_wait(pid);
}

  int
lace_compat_fd_spawnlp_wait(lace_compat_fd_t stdin_fd,
                            lace_compat_fd_t stdout_fd,
                            lace_compat_fd_t stderr_fd,
                            const lace_compat_fd_t* fds_to_inherit,
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
  return lace_compat_fd_spawnvp_wait(
      stdin_fd, stdout_fd, stderr_fd, fds_to_inherit, argv);
}
