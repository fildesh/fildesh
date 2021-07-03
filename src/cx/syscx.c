/**
 * \file syscx.c
 * Interact with the operating system.
 **/
#include "syscx.h"
#include "lace_compat_errno.h"
#include "lace_compat_fd.h"
#include "lace_compat_sh.h"

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
  lace_log_errorf("Caught signal: %d", sig);
  failout_sysCx ("");
}

static
  fd_t
parse_fd_arg_sysCx (const char* s)
{
  int fd = -1;
  if (!s)  return -1;
  s = lace_parse_int(&fd, s);
  if (!s)  return -1;
  return fd;
}

static
  void
parse_args_sysCx (int* pargc, char*** pargv)
{
  char* execing = NULL;
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
      if (fd >= 0) {
        if (fd != 0) {
          lace_compat_fd_move_to(0, fd);
          lace_compat_fd_inherit(0);
        }
      } else {
        DBog1( "Bad -stdxfd argument: %s", argv[argi-1] );
        failout_sysCx ("");
      }
    }
    else if (eql_cstr (arg, "-stdofd"))
    {
      fd_t fd = parse_fd_arg_sysCx (argv[argi++]);
      if (fd >= 0) {
        if (fd != 1) {
          lace_compat_fd_move_to(1, fd);
          lace_compat_fd_inherit(1);
        }
      } else {
        DBog1( "Bad -stdofd argument: %s", argv[argi-1] );
        failout_sysCx ("");
      }
    }
    if (eql_cstr (arg, "-closefd"))
    {
      fd_t fd = parse_fd_arg_sysCx (argv[argi++]);
      if (fd >= 0) {
        lace_compat_fd_close(fd);
      } else {
        DBog1( "Bad -closefd argument: %s", argv[argi-1] );
        failout_sysCx ("");
      }
    }
    else if (eql_cstr (arg, "-exec"))
    {
      execing = argv[argi++];
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

  if (execing) {
    /* Only exec if it's a different program.*/
    if (0 != strcmp(argv[0], execing)) {
      argv[0] = execing;
      execvp_sysCx(argv);
    }
  }
}

/** Initialize the system.
 *
 * \return The number one.
 * \sa lose_sysCx()
 **/
  int
init_sysCx (int* pargc, char*** pargv)
{
  parse_args_sysCx (pargc, pargv);

  signal (SIGSEGV, signal_hook_sysCx);
#ifndef LACE_POSIX_SOURCE
  _setmode(_fileno(stdin), _O_BINARY);
  _setmode(_fileno(stdout), _O_BINARY);
  _setmode(_fileno(stderr), _O_BINARY);
#endif
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
  static bool called = false;
  unsigned i;
  assert(!called);
  called = true;
  for (i = 0; i < LoseFns.sz; ++i) {
    /* Do in reverse because it's a stack.*/
    DeclEltTable( HookFn, hook, LoseFns, LoseFns.sz-i-1 );
    if (hook->x)
      ((void (*) (void*)) hook->f) (hook->x);
    else
      hook->f ();
  }
  LoseTable( LoseFns );
  LoseFns.sz = 0;
}

  void
failout_sysCx (const char* msg)
{
  if (msg)
  {
    int err = errno;
    /* Use literal stderr just in case we have memory problems.*/
    FILE* f = stderr;

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

  int
open_lace_xfd(const char* filename)
{
  static const char dev_fd_prefix[] = "/dev/fd/";
  int fd = -1;
  if (eq_cstr("-", filename)) {
    fd = 0;
  }
  else if (pfxeq_cstr(dev_fd_prefix, filename)) {
    lace_parse_int(&fd, &filename[strlen(dev_fd_prefix)]);
  }
  else {
#ifdef LACE_POSIX_SOURCE
    fd = open(filename, O_RDONLY);
#else
    fd = _open(filename, _O_RDONLY);
#endif
  }
  if (fd >= 0) {
    lace_compat_fd_cloexec(fd);
  }
  return fd;
}

  int
open_lace_ofd(const char* filename)
{
  static const char dev_fd_prefix[] = "/dev/fd/";
#ifdef LACE_POSIX_SOURCE
  const int flags =  O_WRONLY | O_CREAT | O_TRUNC | O_APPEND;
  const int mode
    = S_IWUSR | S_IWGRP | S_IWOTH
    | S_IRUSR | S_IRGRP | S_IROTH;
#else
  const int flags = _O_WRONLY | _O_CREAT | _O_TRUNC | O_APPEND;
  const int mode = _S_IREAD | _S_IWRITE;
#endif
  int fd = -1;
  if (eq_cstr("-", filename)) {
    fd = 1;
  }
  else if (pfxeq_cstr(dev_fd_prefix, filename)) {
    lace_parse_int(&fd, &filename[strlen(dev_fd_prefix)]);
  }
  else {
#ifdef LACE_POSIX_SOURCE
    fd = open(filename, flags, mode);
#else
    fd = _open(filename, flags, mode);
#endif
  }
  if (fd >= 0) {
    lace_compat_fd_cloexec(fd);
  }
  return fd;
}

#ifndef LACE_POSIX_SOURCE
static
  char*
escape_windows_cmd_arg(const char* arg)
{
  /* We should escape quotes and backslashes!*/
  const uint arglen = strlen(arg);
  DeclTable( char, escaped );
  EnsizeTable( escaped, arglen + 3 );
  escaped.s[0] = '\"';
  memcpy(&escaped.s[1], arg, arglen);
  escaped.s[arglen+1] = '\"';
  escaped.s[arglen+2] = '\0';
  return escaped.s;
}
#endif

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
  DeclTable( cstr, escaped_args );
  PushTable( escaped_args, dup_cstr(argv[0]) );
  while (argv[escaped_args.sz]) {
    char* escaped_arg = escape_windows_cmd_arg(argv[escaped_args.sz]);
    PushTable( escaped_args, escaped_arg );
  }
  PushTable( escaped_args, 0 );

  pid = _spawnvp (_P_NOWAIT, escaped_args.s[0], escaped_args.s);

  CPopTable( escaped_args, 1 );
  while (escaped_args.sz > 0) {
    free(*TopTable(escaped_args));
    CPopTable(escaped_args, 1);
  }
  LoseTable(escaped_args);
#endif
  if (pid < 0) {
    lace_compat_errno_trace();
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
    int status = lace_compat_sh_wait(pid);
    if (status < 0) {
      lace_log_error("Failed to wait for process.");
      exit(1);
    }
    exit (status);
  }
#endif
  DBog0( "execvp() failed!" );
  /* DBog1( "PATH=%s", getenv ("PATH") ); */
  failout_sysCx ("execvp() failed!");
}

  bool
kill_please_sysCx(pid_t pid)
{
#ifdef LACE_POSIX_SOURCE
  return (0 == kill(pid, SIGINT));
#else
  bool success = false;
  HANDLE handle = OpenProcess(PROCESS_TERMINATE, false, pid);
  if (handle) {
    success = !!TerminateProcess(handle, 1);
    CloseHandle(handle);
  }
  return success;
#endif
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
  OFile of[1] = {DEFAULT_OFile};
  zuint i;

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
    lace_compat_fd_close(urandom_fd);
  }

  if (nbytes != (int)(buf_size+size))
    BailOut(0, "Failed to read from /dev/urandom");

  static_off = off;
  return 1;
#else
  BailOut(0, "Not available in Windows yet");
#endif
}

