
#include "include/fildesh/fildesh_compat_fd.h"
#include "include/fildesh/fildesh_compat_errno.h"
#include "include/fildesh/fildesh_compat_sh.h"

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


/** Uncomment to assert that /dev/stderr stays as fd 2.*/
/* #define STRICT_STDERR_PLACEMENT */

#ifdef STRICT_STDERR_PLACEMENT
#define MAYBE_ASSERT_NOT_STDERR(fd)  assert(fd != 2)
#else
#define MAYBE_ASSERT_NOT_STDERR(fd)
#endif


static
  int
fildesh_compat_fd_cloexec(int fd)
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
fildesh_compat_fd_inherit(int fd)
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
fildesh_compat_fd_close(FildeshCompat_fd fd)
{
  int istat;
  assert(fd >= 0);
#ifndef _MSC_VER
  istat = close(fd);
#else
  istat = _close(fd);
#endif
#ifndef NDEBUG
  if (istat != 0) {
    int e = fildesh_compat_errno_trace();
    /* File descriptor should be valid.*/
    assert(istat == 0 || e != EBADF);
  }
#endif
  if (istat != 0) {return -1;}
  return 0;
}

static
  FildeshCompat_fd
fildesh_compat_fd_duplicate(FildeshCompat_fd fd)
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
  if (0 != fildesh_compat_fd_cloexec(newfd)) {
    /* Caller will assume the fd wasn't created.*/
    fildesh_compat_errno_trace();
    fildesh_compat_fd_close(newfd);
    return -1;
  }
  return newfd;
}

static
  int
fildesh_compat_fd_copy_to_(FildeshCompat_fd dst, FildeshCompat_fd src)
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
fildesh_compat_fd_copy_to(FildeshCompat_fd dst, FildeshCompat_fd src)
{
  int istat;
  if (dst == src) {return 0;}
  /* Acquire exclusive lock because there could be stdio.*/
  FILDESH_COMPAT_FD_ENTER_EXCLUSIVE;
  istat = fildesh_compat_fd_copy_to_(dst, src);
  if (istat >= 0) {
    if (dst <= 2) {
      istat = fildesh_compat_fd_inherit(dst);
    } else {
      istat = fildesh_compat_fd_cloexec(dst);
    }
    if (istat != 0) {fildesh_compat_fd_close(dst);}
  }
  FILDESH_COMPAT_FD_LEAVE_EXCLUSIVE;
  return istat;
}

  int
fildesh_compat_fd_move_to(FildeshCompat_fd dst, FildeshCompat_fd oldfd)
{
  int istat = 0;
  assert(dst >= 0);
  if (dst == oldfd) {return 0;}
  MAYBE_ASSERT_NOT_STDERR(oldfd);
  /* Acquire exclusive lock because there could be stdio.*/
  FILDESH_COMPAT_FD_ENTER_EXCLUSIVE;

  istat = fildesh_compat_fd_copy_to_(dst, oldfd);
  if (0 != fildesh_compat_fd_close(oldfd)) {
    if (istat == 0) {
      fildesh_compat_fd_close(dst);
    }
    istat = -1;
  }

  if (istat >= 0) {
    if (dst <= 2) {
      istat = fildesh_compat_fd_inherit(dst);
    } else {
      istat = fildesh_compat_fd_cloexec(dst);
    }
    if (istat != 0) {fildesh_compat_fd_close(dst);}
  }

  FILDESH_COMPAT_FD_LEAVE_EXCLUSIVE;
  return istat;
}

static
  FildeshCompat_fd
fildesh_compat_fd_move_off_stdio(FildeshCompat_fd fd)
{
  unsigned i;
  int e = 0;
  FildeshCompat_fd fds[4] = {-1, -1, -1, -1};
  MAYBE_ASSERT_NOT_STDERR(fd);
  if (fd < 0 || fd > 2) {return fd;}
  fds[0] = fd;
  for (i = 1; i < 4; ++i) {
    fds[i] = fildesh_compat_fd_duplicate(fd);
    if (fds[i] > 2) {
      /* We'll return this file descriptor; avoid closing it.*/
      fd = fds[i];
      fds[i] = -1;
      break;
    }
    if (fds[i] < 0) {
      e = fildesh_compat_errno_trace();
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
    fildesh_compat_fd_close(fds[i]);
  }
  if (fd < 0) {
    errno = e;
  }
  return fd;
}

  FildeshCompat_fd
fildesh_compat_fd_claim(FildeshCompat_fd fd)
{
  FILDESH_COMPAT_FD_ENTER_SHARED;
  fd = fildesh_compat_fd_move_off_stdio(fd);
  if (fd >= 0) {
    int istat = fildesh_compat_fd_cloexec(fd);
    assert(istat == 0 || errno != EBADF);
    if (istat != 0) {fildesh_compat_errno_trace();}
  }
#ifdef _MSC_VER
  if (fd >= 0) {
    int istat = _setmode(fd, _O_BINARY);
    if (istat < 0) {
      fildesh_compat_errno_trace();
      fildesh_compat_fd_close(fd);
      fd = -1;
    }
  }
#endif
  FILDESH_COMPAT_FD_LEAVE_SHARED;
  return fd;
}

  FildeshCompat_fd
fildesh_compat_file_open_readonly(const char* filename)
{
  FildeshCompat_fd fd;
  int flags;
  FILDESH_COMPAT_FD_ENTER_SHARED;
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
  fd = fildesh_compat_fd_move_off_stdio(fd);
  if (fd >= 0) {
    fildesh_compat_fd_cloexec(fd);
  }
  FILDESH_COMPAT_FD_LEAVE_SHARED;
  return fd;
}

  FildeshCompat_fd
fildesh_compat_file_open_writeonly(const char* filename)
{
  FildeshCompat_fd fd;
  int flags, mode;
  FILDESH_COMPAT_FD_ENTER_SHARED;
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
  fd = fildesh_compat_fd_move_off_stdio(fd);
  if (fd >= 0) {
    fildesh_compat_fd_cloexec(fd);
  }
  FILDESH_COMPAT_FD_LEAVE_SHARED;
  return fd;
}

  int
fildesh_compat_fd_pipe(
    FildeshCompat_fd* ret_produce,
    FildeshCompat_fd* ret_consume)
{
  int fds[2] = {-1, -1};
  int istat;
  FILDESH_COMPAT_FD_ENTER_SHARED;

#ifndef _MSC_VER
  istat = pipe(fds);
#else
  istat = _pipe(fds, 0, _O_BINARY);
#endif

  if (istat == 0 && fds[0] <= 2) {
    fds[0] = fildesh_compat_fd_move_off_stdio(fds[0]);
  } else if (istat == 0 && fds[0] > 2) {
    istat = fildesh_compat_fd_cloexec(fds[0]);
  }
  if (istat == 0 && fds[1] <= 2) {
    fds[1] = fildesh_compat_fd_move_off_stdio(fds[1]);
  } else if (istat == 0 && fds[1] > 2) {
    istat = fildesh_compat_fd_cloexec(fds[1]);
  }

  if (istat != 0 || fds[0] < 0 || fds[1] < 0) {
    if (fds[0] >= 0) {fildesh_compat_fd_close(fds[0]);}
    if (fds[1] >= 0) {fildesh_compat_fd_close(fds[1]);}
    fds[0] = -1;
    fds[1] = -1;
    istat = -1;
  }

  *ret_produce = fds[1];
  *ret_consume = fds[0];
  FILDESH_COMPAT_FD_LEAVE_SHARED;
  return istat;
}

  size_t
fildesh_compat_fd_write(FildeshCompat_fd fd, const void* data, size_t data_size)
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
fildesh_compat_fd_read(FildeshCompat_fd fd, void* buf, size_t buf_capacity)
{
#ifndef _MSC_VER
  ssize_t n = read(fd, buf, buf_capacity);
#else
  long n = _read(fd, buf, buf_capacity);
#endif
  if (n <= 0) {return 0;}
  return (size_t)n;
}

static int fildesh_compat_fd_inherit_empty_stdio(int fd)
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
    istat = fildesh_compat_fd_copy_to_(0, fds[0]);
  } else {
    istat = fildesh_compat_fd_copy_to_(fd, fds[1]);
  }
  if (istat != 0 || fds[0] != fd) {
    fildesh_compat_fd_close(fds[0]);
  }
  if (istat != 0 || fds[1] != fd) {
    fildesh_compat_fd_close(fds[1]);
  }
  return istat;
}

static int
fildesh_compat_fd_spawnvp_setup_stdio(
    FildeshCompat_fd stdin_fd,
    FildeshCompat_fd stdout_fd,
    FildeshCompat_fd stderr_fd)
{
  int istat = 0;
  if (stdin_fd < 0) {
    istat = fildesh_compat_fd_inherit_empty_stdio(0);
  } else if (stdin_fd == 0) {
    /* Nothing. Already inherited.*/
  } else {
    assert(stdin_fd >= 3);
    istat = fildesh_compat_fd_copy_to_(0, stdin_fd);
    if (istat != 0) {return -100;}
    istat = fildesh_compat_fd_close(stdin_fd);
  }
  if (istat != 0) {return -101;}
  if (stdout_fd < 0) {
    istat = fildesh_compat_fd_inherit_empty_stdio(1);
  } else if (stdout_fd == 1) {
    /* Nothing. Already inherited.*/
  } else if (stdout_fd == 2) {
    assert(stderr_fd == 2);
    istat = fildesh_compat_fd_copy_to_(1, 2);
  } else {
    assert(stdout_fd >= 3);
    istat = fildesh_compat_fd_copy_to_(1, stdout_fd);
    if (istat != 0) {return -110;}
    istat = fildesh_compat_fd_close(stdout_fd);
  }
  if (istat != 0) {return -111;}
  if (stderr_fd < 0) {
    istat = fildesh_compat_fd_inherit_empty_stdio(2);
  } else if (stderr_fd == 1) {
    assert(stdout_fd == 1);
    istat = fildesh_compat_fd_copy_to_(2, 1);
  } else if (stderr_fd == 2) {
    /* Nothing. Already inherited.*/
  } else {
    assert(stderr_fd >= 3);
    istat = fildesh_compat_fd_copy_to_(2, stderr_fd);
    if (istat != 0) {return -120;}
    istat = fildesh_compat_fd_close(stderr_fd);
  }
  if (istat != 0) {return -121;}
  return 0;
}

static void
fildesh_compat_fd_spawnvp_cleanup_stdio(
    FildeshCompat_fd stdin_fd,
    FildeshCompat_fd stdout_fd,
    FildeshCompat_fd stderr_fd,
    int status)
{
  if (status != -100 && stdin_fd != 0) {
    fildesh_compat_fd_close(0);
  }
  if (status > -110 && status <= -100) {
    fildesh_compat_fd_close(stdin_fd);
  }
  if (status != -110 && stdout_fd != 1) {
    fildesh_compat_fd_close(1);
  }
  if (status > -120 && status <= -100) {
    fildesh_compat_fd_close(stdout_fd);
  }
  if (status != -120 && stderr_fd != 2) {
    fildesh_compat_fd_close(2);
  }
  if (status > -130 && status <= -100) {
    fildesh_compat_fd_close(stderr_fd);
  }
}

  FildeshCompat_pid
fildesh_compat_fd_spawnvp(
    FildeshCompat_fd stdin_fd,
    FildeshCompat_fd stdout_fd,
    FildeshCompat_fd stderr_fd,
    const FildeshCompat_fd* fds_to_inherit,
    const char* const* argv)
{
  FildeshCompat_pid pid = -1;
  int e = 0;
  int istat = 0;
  unsigned i;
  FILDESH_COMPAT_FD_ENTER_EXCLUSIVE;
  if (fds_to_inherit) {
    for (i = 0; fds_to_inherit[i] >= 0 && istat == 0; ++i) {
      assert(fds_to_inherit[i] >= 3);
      istat = fildesh_compat_fd_inherit(fds_to_inherit[i]);
      if (fds_to_inherit[i] == fds_to_inherit[i+1]) {
        ++i;  /* No need to inherit twice.*/
      }
    }
  }

  if (istat == 0) {
    istat = fildesh_compat_fd_spawnvp_setup_stdio(
        stdin_fd, stdout_fd, stderr_fd);
    if (istat == 0) {
      pid = fildesh_compat_sh_spawn(argv);
      if (pid < 0) {
        e = fildesh_compat_errno_trace();
      }
      fildesh_compat_fd_spawnvp_cleanup_stdio(
          stdin_fd, stdout_fd, stderr_fd, istat);
    }
  }

  if (fds_to_inherit) {
    for (i = 0; fds_to_inherit[i] >= 0; ++i) {
      if (fds_to_inherit[i] == fds_to_inherit[i+1]) {
        fildesh_compat_fd_cloexec(fds_to_inherit[i]);
        ++i;
      } else {
        fildesh_compat_fd_close(fds_to_inherit[i]);
      }
    }
  }
  FILDESH_COMPAT_FD_LEAVE_EXCLUSIVE;
  if (pid < 0) {
    errno = e;
    return -1;
  }
  return pid;
}

  FildeshCompat_pid
fildesh_compat_fd_spawnlp(
    FildeshCompat_fd stdin_fd,
    FildeshCompat_fd stdout_fd,
    FildeshCompat_fd stderr_fd,
    const FildeshCompat_fd* fds_to_inherit,
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
  return fildesh_compat_fd_spawnvp(
      stdin_fd, stdout_fd, stderr_fd, fds_to_inherit, argv);
}

  int
fildesh_compat_fd_spawnvp_wait(
    FildeshCompat_fd stdin_fd,
    FildeshCompat_fd stdout_fd,
    FildeshCompat_fd stderr_fd,
    const FildeshCompat_fd* fds_to_inherit,
    const char* const* argv)

{
  FildeshCompat_pid pid = fildesh_compat_fd_spawnvp(
      stdin_fd, stdout_fd, stderr_fd, fds_to_inherit, argv);
  if (pid < 0) {return -1;}
  return fildesh_compat_sh_wait(pid);
}

  int
fildesh_compat_fd_spawnlp_wait(
    FildeshCompat_fd stdin_fd,
    FildeshCompat_fd stdout_fd,
    FildeshCompat_fd stderr_fd,
    const FildeshCompat_fd* fds_to_inherit,
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
  return fildesh_compat_fd_spawnvp_wait(
      stdin_fd, stdout_fd, stderr_fd, fds_to_inherit, argv);
}
