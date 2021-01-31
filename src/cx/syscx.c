/**
 * \file syscx.c
 * Interact with the operating system.
 **/
#include "syscx.h"

#include "fileb.h"

#include <errno.h>
#include <signal.h>

DeclTableT( HookFn, struct { void (*f) (); void* x; } );

static DeclTable( HookFn, LoseFns );
static const char* ExeName = 0;

  const char*
exename_of_sysCx ()
{
  return ExeName;
}

static
  void
signal_hook_sysCx (int sig)
{
  DBog1( "Caught signal: %d", sig );
  failout_sysCx ("");
}

#if 0
static
  int
parse_fd_uri_sysCx (const char* s)
{
  static const char fd_pfx[] = "fd://";
  int fd = -1;
  if (!s)  return -1;
  if (strlen (s) <= strlen (fd_pfx))  return -1;
  if (0 != memcmp (s, fd_pfx, strlen (fd_pfx)))  return -1;

  s += strlen (fd_pfx);
  s = xget_int_cstr (&fd, s);
  if (!s)  return -1;
  return fd;
}
#endif

static
  fd_t
parse_fd_arg_sysCx (const char* s)
{
  int fd = -1;
  if (!s)  return -1;
  s = xget_int_cstr (&fd, s);
  if (!s)  return -1;
  return fd;
}

static
  void
parse_args_sysCx (int* pargc, char*** pargv)
{
  bool execing = false;
  int argc = *pargc;
  char** argv = *pargv;
  int argi = 0;
  ExeName = argv[argi++];

  if (!argv[argi])  return;
  if (!eql_cstr (argv[argi++], MagicArgv1_sysCx))  return;

  while (argv[argi] && !eql_cstr (argv[argi], "--"))
  {
    char* arg = argv[argi++];
    if (eql_cstr (arg, "-stdxfd"))
    {
      fd_t fd = parse_fd_arg_sysCx (argv[argi++]);
      if (fd >= 0)  dup2_sysCx (fd, 0);
      else
      {
        DBog1( "Bad -stdxfd argument: %s", argv[argi-1] );
        failout_sysCx ("");
      }
    }
    else if (eql_cstr (arg, "-stdofd"))
    {
      fd_t fd = parse_fd_arg_sysCx (argv[argi++]);
      if (fd >= 0)  dup2_sysCx (fd, 1);
      else
      {
        DBog1( "Bad -stdofd argument: %s", argv[argi-1] );
        failout_sysCx ("");
      }
    }
    if (eql_cstr (arg, "-closefd"))
    {
      fd_t fd = parse_fd_arg_sysCx (argv[argi++]);
      if (fd >= 0)  closefd_sysCx (fd);
      else
      {
        DBog1( "Bad -closefd argument: %s", argv[argi-1] );
        failout_sysCx ("");
      }
    }
    else if (eql_cstr (arg, "-exe"))
    {
      argv[0] = argv[argi++];
    }
    else if (eql_cstr (arg, "-exec"))
    {
      execing = true;
    }
  }

  {
    int i;
    for (i = 1; i <= argc-argi; ++i) {
      argv[i] = argv[argi+i];
    }
  }

  *pargc = argc - argi;
  *pargv = argv;

  if (execing)
    execvp_sysCx (argv);
}

/** Initialize the system.
 *
 * \return The number one.
 * \sa lose_sysCx()
 **/
  int
init_sysCx (int* pargc, char*** pargv)
{
  stderr_OFileB ();
  parse_args_sysCx (pargc, pargv);

  stdin_XFileB ();
  stdout_OFileB ();
  signal (SIGSEGV, signal_hook_sysCx);
  return 1;
}

  void
push_losefn_sysCx (void (*f) ())
{
  DeclGrow1Table( HookFn, hook, LoseFns );
  hook->f = f;
  hook->x = 0;
}
  void
push_losefn1_sysCx (void (*f) (void*), void* x)
{
  DeclGrow1Table( HookFn, hook, LoseFns );
  hook->f = (void (*) ()) f;
  hook->x = x;
}

  void
lose_sysCx ()
{
  uint i;
  UFor( i, LoseFns.sz ) {
    /* Do in reverse because it's a stack.*/
    DeclEltTable( HookFn, hook, LoseFns, LoseFns.sz-i-1 );
    if (hook->x)
      ((void (*) (void*)) hook->f) (hook->x);
    else
      hook->f ();
  }
  LoseTable( LoseFns );

  lose_XFileB (stdin_XFileB ());
  lose_OFileB (stdout_OFileB ());
  lose_OFileB (stderr_OFileB ());
}

  void
failout_sysCx (const char* msg)
{
  if (msg)
  {
    int err = errno;
    /* Use literal stderr just in case we have memory problems.*/
    FILE* f = stderr;

    /* Flush these so the next message is last.*/
    flush_OFileB (stdout_OFileB ());
    flush_OFileB (stderr_OFileB ());

    fprintf (f, "FAILOUT: %s\n", exename_of_sysCx ());

#ifdef LACE_POSIX_SOURCE
    {
      char hostname[128];
      uint n = ArraySz(hostname);
      gethostname(hostname, n);
      hostname[n-1] = 0;
      fprintf (f, "^^^ Host: %s\n", hostname);
    }
#endif

    if (msg[0])
      fprintf (f, "^^^ Reason: %s\n", msg);
    if (err != 0)
      fprintf (f, "^^^ errno:%d %s\n", err, strerror (err));
  }
  lose_sysCx ();
  if (false)
    abort();
  else
    exit(1);
}

  void
dbglog_printf3 (const char* file,
    const char* func,
    uint line,
    const char* fmt,
    ...)
{
  va_list args;
  int err = errno;
  OFile* of = stderr_OFile ();

  while (true) {
    const char* tmp = strstr (file, "bld/");
    if (!tmp)  break;
    file = &tmp[4];
  }

  printf_OFile (of, "./%s(%u) %s: ", file, line, func);

  va_start (args, fmt);
  vprintf_OFile (of, fmt, args);
  va_end(args);

  oput_char_OFile (of, '\n');

  if (err != 0)
  {
#if 0
    /* Why no work? */
    const uint n = 2048 * sizeof(char);
    char* s;

    printf_FileB (of, "^^^ errno:%d ", err);

    s = (char*) ensure_OFile (of, n);
    s[0] = '\0';

    strerror_r (err, s, n);

    of->off += strlen (s) * sizeof(char);
    oput_char_File (of, '\n');
#else
    printf_OFile (of, "^^^ errno:%d %s\n", err, strerror (err));
#endif
    errno = 0;
  }
  flush_OFile (of);
}


  XFileB*
stdin_XFileB ()
{
  static bool initialized = false;
  static XFileB xfb[1];
  if (!initialized)
  {
    init_XFileB (xfb);
    set_FILE_FileB (&xfb->fb, stdin);
    xfb->fb.byline = true;
    initialized = true;
  }
  return xfb;
}

  OFileB*
stdout_OFileB ()
{
  static bool initialized = false;
  static OFileB ofb[1];
  if (!initialized)
  {
    init_OFileB (ofb);
    set_FILE_FileB (&ofb->fb, stdout);
    initialized = true;
  }
  return ofb;
}

  OFileB*
stderr_OFileB ()
{
  static bool initialized = false;
  static OFileB ofb[1];
  if (!initialized)
  {
    init_OFileB (ofb);
    set_FILE_FileB (&ofb->fb, stderr);
    initialized = true;
  }
  return ofb;
}

  XFile*
stdin_XFile ()
{
  XFileB* xfb = stdin_XFileB ();
  return &xfb->xf;
}

  OFile*
stdout_OFile ()
{
  OFileB* ofb = stdout_OFileB ();
  return &ofb->of;
}

  OFile*
stderr_OFile ()
{
  OFileB* ofb = stderr_OFileB ();
  return &ofb->of;
}

  bool
pipe_sysCx (fd_t* fds)
{
  int ret = -1;
#ifdef LACE_POSIX_SOURCE
  ret = pipe (fds);
#else
  ret = _pipe (fds, BUFSIZ, 0);
#endif
  return (ret == 0);
}

  fd_t
dup_sysCx (fd_t fd)
{
  int ret = -1;
#ifdef LACE_POSIX_SOURCE
  ret = dup (fd);
#else
  ret = _dup (fd);
#endif
  return ret;
}

  bool
dup2_sysCx (fd_t oldfd, fd_t newfd)
{
  int ret = -1;
#ifdef LACE_POSIX_SOURCE
  ret = dup2 (oldfd, newfd);
#else
  ret = _dup2 (oldfd, newfd);
#endif
  return (ret == 0);
}

  int
open_lace_xfd(const char* filename)
{
#ifdef LACE_POSIX_SOURCE
  return open(filename, O_RDONLY);
#else
  return _open(filename, _O_RDONLY);
#endif
}

  int
open_lace_ofd(const char* filename)
{
#ifdef LACE_POSIX_SOURCE
  const int flags =  O_WRONLY | O_CREAT | O_TRUNC | O_APPEND;
  const int mode
    = S_IWUSR | S_IWGRP | S_IWOTH
    | S_IRUSR | S_IRGRP | S_IROTH;
  return open(filename, flags, mode);
#else
  const int flags = _O_WRONLY | _O_CREAT | _O_TRUNC | O_APPEND;
  const int mode = _S_IREAD | _S_IWRITE;
  return _open(filename, flags, mode);
#endif
}

  long
read_sysCx (fd_t fd, void* buf, long sz)
{
#ifdef LACE_POSIX_SOURCE
  return read (fd, buf, sz);
#else
  return _read (fd, buf, sz);
#endif
}

  long
write_sysCx (fd_t fd, const void* buf, long sz)
{
#ifdef LACE_POSIX_SOURCE
  return write (fd, buf, sz);
#else
  return _write (fd, buf, sz);
#endif
}

  bool
closefd_sysCx (fd_t fd)
{
  int ret = -1;
#ifdef LACE_POSIX_SOURCE
  ret = close (fd);
#else
  ret = _close (fd);
#endif
  return (ret == 0);
}

  FILE*
fdopen_sysCx (fd_t fd, const char* mode)
{
#ifdef LACE_POSIX_SOURCE
  return fdopen (fd, mode);
#else
  return _fdopen (fd, mode);
#endif
}

  int
setfd_nonblock_sysCx(fd_t fd)
{
#ifdef LACE_POSIX_SOURCE
  int istat;
  istat = fcntl(fd, F_GETFD);
  if (istat < 0) {
    return istat;
  }
  return fcntl(fd, F_SETFD, istat | O_NONBLOCK);
#else
  unsigned long arg = 1;
  return ioctlsocket(fd, FIONBIO, &arg);
#endif
}

  int
poll_sysCx(struct pollfd* pollfds, size_t npollfds, int timeout)
{
#ifdef LACE_POSIX_SOURCE
  return poll(pollfds, npollfds, timeout);
#else
  return WSAPoll(pollfds, npollfds, timeout);
#endif
}

  static void
oput_execvp_args (const char* fn, char* const* argv)
{
  OFile* of = stderr_OFile ();
  uint i;
  oput_cstr_OFile (of, fn);
  oput_char_OFile (of, ':');
  for (i = 0; argv[i]; ++i)
  {
    oput_char_OFile (of, ' ');
    oput_cstr_OFile (of, argv[i]);
  }
  oput_char_OFile (of, '\n');
  flush_OFile (of);
}

  pid_t
spawnvp_sysCx (char* const* argv)
{
  pid_t pid;
#ifdef LACE_POSIX_SOURCE
  pid = fork ();
  if (pid == 0)
  {
    if (eql_cstr (argv[0], exename_of_sysCx ()) &&
        eql_cstr (argv[1], MagicArgv1_sysCx))
    {
      int argc = 0;
      DeclTable( cstr, t );
      uint i;
      while (argv[argc])  ++ argc;

      SizeTable( t, argc+1 );
      for (i = 0; i < t.sz; ++i) {
        t.s[i] = argv[i];
      }
      parse_args_sysCx (&argc, &t.s);
      execvp_sysCx (t.s);
    }
    execvp_sysCx (argv);
  }
#else
  pid = _spawnvp (_P_NOWAIT, argv[0], argv);
#endif
  if (pid < 0)
  {
    DBog0( "spawn() failed!" );
    oput_execvp_args ("spawnvp_sysCx()", argv);
  }
  return pid;
}

  void
execvp_sysCx (char* const* argv)
{
  if (0) {
    uint i;
    for (i = 0; argv[i]; ++i) {
      DBog2( "argv[%u] = %s", i, argv[i] );
    }
  }
#ifdef LACE_POSIX_SOURCE
  execvp (argv[0], argv);
#else
  pid_t pid = -1;
  pid = spawnvp_sysCx (argv);
  if (pid >= 0)
  {
    int status = 1;
    if (!waitpid_sysCx (pid, &status))
      failout_sysCx ("Failed to wait for process.");
    exit (status);
  }
#endif
  DBog0( "execvp() failed!" );
  /* DBog1( "PATH=%s", getenv ("PATH") ); */
  oput_execvp_args ("execvp_sysCx()", argv);
  failout_sysCx ("execvp() failed!");
}

  bool
waitpid_sysCx (pid_t pid, int* status)
{
  int ret = -1;
#ifdef LACE_POSIX_SOURCE
  ret = waitpid (pid, status, 0);
  if (status)
    *status = WEXITSTATUS( *status );
#else
  ret = _cwait (status, pid, 0);
#endif
  return (ret >= 0);
}

/**
 * \param path  Return value. Can come in as a hint for the path name.
 **/
  void
mktmppath_sysCx (AlphaTab* path)
{
  const char* v = 0;
#ifdef LACE_POSIX_SOURCE
  pid_t pid = getpid ();
#else
  pid_t pid = _getpid ();
#endif
  OFile of[1];
  zuint i;
  init_OFile (of);

#ifdef LACE_POSIX_SOURCE
  v = getenv ("TMPDIR");
  if (!v)  v = "/tmp";
#else
  v = getenv ("TEMP");
#endif

  if (!v)
  {
    path->sz = 0;
    return;
  }
  oput_cstr_OFile (of, v);
  oput_char_OFile (of, '/');
  oput_AlphaTab (of, path);
  oput_char_OFile (of, '-');
  oput_luint_OFile (of, pid);
  oput_char_OFile (of, '-');

  path->sz = 0;
  for (i = 0; i < SIZE_MAX; ++i) {
    zuint off = of->off;
    oput_luint_OFile (of, i);

    if (mkdir_sysCx (cstr1_OFile (of, 0))) {
      copy_AlphaTab_OFile (path, of);
      break;
    }

    of->off = off;
  }
  lose_OFile (of);
}

  void
setenv_sysCx (const char* key, const char* val)
{
#ifdef LACE_POSIX_SOURCE
  setenv (key, val, 1);
#else
  SetEnvironmentVariable (key, val);
  /* DBog2( "key:%s val:%s", key, val ); */
#endif
}

  void
tacenv_sysCx (const char* key, const char* val)
{
#ifdef LACE_POSIX_SOURCE
  const char* sep = ":";
#else
  const char* sep = ";";
#endif
  char* v;
  DecloStack1( AlphaTab, dec, cons1_AlphaTab (val) );

  v = getenv (key);
  if (v)
  {
    cat_cstr_AlphaTab (dec, sep);
    cat_cstr_AlphaTab (dec, v);
  }

  setenv_sysCx (key, cstr_AlphaTab (dec));
  lose_AlphaTab (dec);
}

  void
cloexec_sysCx (fd_t fd, bool b)
{
#ifdef LACE_POSIX_SOURCE
  int flags = fcntl (fd, F_GETFD);

  if (flags == -1)
  {
    DBog0( "fcntl() GET failed." );
    return;
  }

  if (b == (0 != (flags & FD_CLOEXEC)))  return;

  if (b)  flags |= FD_CLOEXEC;
  else    flags ^= FD_CLOEXEC;

  if (fcntl (fd, F_SETFD, flags) == -1)
    DBog0( "fcntl() SET failed." );
#else
  SetHandleInformation ((HANDLE) _get_osfhandle (fd),
      HANDLE_FLAG_INHERIT,
      b ?  HANDLE_FLAG_INHERIT : 0);
#endif
}

  bool
chmodu_sysCx (const char* pathname, bool r, bool w, bool x)
{
  int ret = -1;
#ifdef LACE_POSIX_SOURCE
  chmod (pathname, (r ? S_IRUSR : 0) | (w ? S_IWUSR : 0) | (x ? S_IXUSR : 0));
#else
  (void) x;
  _chmod (pathname, (r ? _S_IREAD : 0) | (w ? _S_IWRITE : 0));
#endif
  return (ret == 0);
}

  bool
mkdir_sysCx (const char* pathname)
{
  int ret = -1;
#ifdef LACE_POSIX_SOURCE
  ret = mkdir (pathname, 0700);
#else
  ret = _mkdir (pathname);
#endif
  return (ret == 0);
}

  bool
rmdir_sysCx (const char* pathname)
{
  int ret = -1;
#ifdef LACE_POSIX_SOURCE
  ret = rmdir (pathname);
#else
  ret = _rmdir (pathname);
#endif
  return (ret == 0);
}

  bool
chdir_sysCx (const char* pathname)
{
  int ret = -1;
#ifdef LACE_POSIX_SOURCE
  ret = chdir (pathname);
#else
  ret = _chdir (pathname);
#endif
  return (ret == 0);
}

  Bool
randomize_sysCx(void* p, uint size)
{
#ifdef LACE_POSIX_SOURCE
  static byte buf[4096];
  static const uint buf_size = sizeof(buf);
  static uint static_off = sizeof(buf);
  uint off = static_off;  /* Prevent race conditions.*/
  ssize_t nbytes;

  if (size == 0)  return 1;
  if (size + off <= buf_size) {
    memcpy(p, CastOff(void, buf ,+, off), size);
    off += size;
    static_off = off;
    return 1;
  }
  if (off < buf_size) {
    size -= buf_size - off;
    memcpy(CastOff(void, p ,+, size),
        CastOff(void, buf ,+, off),
        buf_size - off);
  }
  off = 0;

  {
    fd_t urandom_fd = open("/dev/urandom", O_RDONLY);
    if (urandom_fd < 0)
      BailOut(0, "Failed to open /dev/urandom");

    nbytes = read(urandom_fd, buf, buf_size);
    nbytes += read(urandom_fd, p, size);
    close(urandom_fd);
  }

  if (nbytes != (int)(buf_size+size))
    BailOut(0, "Failed to read from /dev/urandom");

  static_off = off;
  return 1;
#else
  BailOut(0, "Not available in Windows yet");
#endif
}

