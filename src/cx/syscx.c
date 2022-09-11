/**
 * \file syscx.c
 * Interact with the operating system.
 **/

#if defined(_MSC_VER)
#elif defined(__APPLE__)
#define FILDESH_POSIX_SOURCE
#else
#define FILDESH_POSIX_SOURCE
#endif

#ifndef FILDESH_POSIX_SOURCE
# include <fcntl.h>
# include <windows.h>
# include <io.h>
#endif

#include <stdio.h>

#include "syscx.h"
#include "fildesh_compat_sh.h"
#include "alphatab.h"

#include <signal.h>

DeclTableT( HookFn, struct { void (*f) (void*); void* x; } );

static DeclTable( HookFn, LoseFns );

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
  (void) _setmode(_fileno(stdin), _O_BINARY);
  (void) _setmode(_fileno(stdout), _O_BINARY);
  (void) _setmode(_fileno(stderr), _O_BINARY);
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

  fildesh_compat_sh_setenv(key, cstr_AlphaTab(dec));
  lose_AlphaTab (dec);
}
