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
#include <stdlib.h>

#include "syscx.h"
#include "fildesh_compat_sh.h"

#include <signal.h>

typedef struct HookFn HookFn;
struct HookFn { void (*f) (void*); void* x; };

static DECLARE_FildeshAT(HookFn, LoseFns);

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
  init_FildeshAT(LoseFns);
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
  HookFn* hook = grow1_FildeshAT(LoseFns);
  hook->f = f;
  hook->x = x;
}

  void
lose_sysCx ()
{
  const size_t n = count_of_FildeshAT(LoseFns);
  static bool called = false;
  size_t i;
  assert(!called);
  called = true;
  for (i = 0; i < n; ++i) {
    /* Do in reverse because it's a stack.*/
    HookFn* hook = &(*LoseFns)[n-i-1];
    hook->f(hook->x);
  }
  close_FildeshAT(LoseFns);
}

