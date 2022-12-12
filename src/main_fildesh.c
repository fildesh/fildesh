
#include <fildesh/fildesh.h>
#include "fildesh_builtin.h"

#include <assert.h>
#include <stdlib.h>
#include <signal.h>

#ifdef _MSC_VER
# include <fcntl.h>
# include <stdio.h>
# include <windows.h>
#endif

static
DECLARE_FildeshAT(
    struct { void (*f) (void*); void* x; },
    exit_callbacks);

void push_fildesh_exit_callback(void (*f) (void*), void* x) {
  grow_FildeshAT(exit_callbacks, 1);
  last_FildeshAT(exit_callbacks).f = f;
  last_FildeshAT(exit_callbacks).x = x;
}

static
  void
fildesh_exit_fn()
{
  static bool called = false;
  const size_t n = count_of_FildeshAT(exit_callbacks);
  size_t i;
  assert(!called);
  called = true;
  for (i = 0; i < n; ++i) {
    /* Do in reverse because it's a stack.*/
    (*exit_callbacks)[n-i-1].f(
        (*exit_callbacks)[n-i-1].x);
  }
  close_FildeshAT(exit_callbacks);
}

static
  void
fildesh_exit_signal_fn(int sig)
{
  fildesh_log_errorf("Caught signal: %d", sig);
  fildesh_exit_fn();
  exit(1);
}


int main(int argc, char** argv)
{
  int exstatus;
  init_FildeshAT(exit_callbacks);

  signal(SIGINT, fildesh_exit_signal_fn);
  signal(SIGSEGV, fildesh_exit_signal_fn);
#ifndef _MSC_VER
  signal(SIGQUIT, fildesh_exit_signal_fn);
  /* We already detect closed pipes when write() returns <= 0.*/
  signal(SIGPIPE, SIG_IGN);
#else
  (void) _setmode(_fileno(stdin), _O_BINARY);
  (void) _setmode(_fileno(stdout), _O_BINARY);
  (void) _setmode(_fileno(stderr), _O_BINARY);
#endif

  exstatus = fildesh_builtin_fildesh_main((unsigned)argc, argv, NULL, NULL);

  fildesh_exit_fn();
  return exstatus;
}

