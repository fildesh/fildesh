/**
 * \file syscx.c
 * Interact with the operating system.
 **/
#include "syscx.h"
#include "fildesh_compat_errno.h"
#include "fildesh_compat_fd.h"
#include "fildesh_compat_sh.h"
#include "alphatab.h"

#include <errno.h>
#include <signal.h>

DeclTableT( HookFn, struct { void (*f) (void*); void* x; } );

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
  fildesh_log_errorf("Caught signal: %d", sig);
  lose_sysCx();
  exit(1);
}

/** Initialize the system.
 *
 * \return The number one.
 * \sa lose_sysCx()
 **/
void init_sysCx()
{
  signal (SIGSEGV, signal_hook_sysCx);
#ifndef FILDESH_POSIX_SOURCE
  _setmode(_fileno(stdin), _O_BINARY);
  _setmode(_fileno(stdout), _O_BINARY);
  _setmode(_fileno(stderr), _O_BINARY);
#endif
}

  void
push_losefn_sysCx (void (*f) (void*), void* x)
{
  DeclGrow1Table( HookFn, hook, LoseFns );
  hook->f = f;
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
    hook->f(hook->x);
  }
  LoseTable( LoseFns );
  LoseFns.sz = 0;
}

  bool
kill_please_sysCx(pid_t pid)
{
#ifdef FILDESH_POSIX_SOURCE
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
  char*
mktmppath_sysCx(const char* hint)
{
  const char* v = 0;
#ifdef FILDESH_POSIX_SOURCE
  pid_t pid = getpid ();
#else
  pid_t pid = _getpid ();
#endif
  char* buf;
  const size_t hint_length = (hint ? strlen(hint) : 0);
  size_t offset;
  unsigned long i;

#ifdef FILDESH_POSIX_SOURCE
  v = getenv ("TMPDIR");
  if (!v)  v = "/tmp";
#else
  v = getenv ("TEMP");
#endif

  if (!v) {
    return NULL;
  }
  offset = strlen(v);
  buf = (char*) malloc(offset + hint_length + 50);
  memcpy(buf, v, offset);
  buf[offset++] = '/';
  memcpy(&buf[offset], hint, hint_length);
  offset += hint_length;
  buf[offset++] = '-';
  offset += sprintf(&buf[offset], "-%lu-", (unsigned long) pid);

  for (i = 0; i < ULONG_MAX; ++i) {
    sprintf(&buf[offset], "%lu", (unsigned long) i);
    if (mkdir_sysCx(buf)) {
      return buf;
    }
  }
  return NULL;
}

  void
setenv_sysCx (const char* key, const char* val)
{
#ifdef FILDESH_POSIX_SOURCE
  setenv (key, val, 1);
#else
  SetEnvironmentVariable (key, val);
  /* DBog2( "key:%s val:%s", key, val ); */
#endif
}

  void
tacenv_sysCx (const char* key, const char* val)
{
#ifdef FILDESH_POSIX_SOURCE
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
mkdir_sysCx (const char* pathname)
{
  int ret = -1;
#ifdef FILDESH_POSIX_SOURCE
  ret = mkdir (pathname, 0700);
#else
  ret = _mkdir (pathname);
#endif
  return (ret == 0);
}
