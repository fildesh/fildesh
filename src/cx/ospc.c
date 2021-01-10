/**
 * \file ospc.c
 * Spawn and communicate with a process.
 **/
#include "ospc.h"

  bool
close_OSPc (OSPc* ospc)
{
  bool good = false;
  if (ospc->pid < 0)  return 0;
  close_XFileB (&ospc->xfb);
  close_OFileB (&ospc->ofb);
  /* if (ospc->pid > 0)  kill (ospc->pid, SIGKILL); */
  good = waitpid_sysCx (ospc->pid, &ospc->status);
  ospc->pid = -1;
  return good;
}

  void
lose_OSPc (OSPc* ospc)
{
  uint i;
  close_OSPc (ospc);
  lose_XFileB (&ospc->xfb);
  lose_OFileB (&ospc->ofb);
  lose_AlphaTab (&ospc->cmd);
  for (i = 0; i < ospc->args.sz; ++i) {
    lose_AlphaTab (&ospc->args.s[i]);
  }
  LoseTable( ospc->args );
}

/** Make a pipe to process input.
 **/
  void
stdxpipe_OSPc (OSPc* ospc)
{
  Claim( !ospc->of );
  ospc->of = &ospc->ofb.of;
}

/** Make a pipe from process output.
 **/
  void
stdopipe_OSPc (OSPc* ospc)
{
  Claim( !ospc->xf );
  ospc->xf = &ospc->xfb.xf;
}

  bool
spawn_OSPc (OSPc* ospc)
{
  fd_t xfd[2] = { -1, -1 };
  fd_t ofd[2] = { -1, -1 };
  DeclLegit( good );
  DeclTable( cstr, argv );
  uint nfrees = 0;
  uint i;

  if (ospc->of) {
    DoLegitLine( "pipe()" )
      pipe_sysCx (xfd);
  }
  if (ospc->xf) {
    DoLegitLine( "pipe()" )
      pipe_sysCx (ofd);
  }

  if (good)
  {
    PushTable( argv, dup_cstr (exename_of_sysCx ()) );
    PushTable( argv, dup_cstr (MagicArgv1_sysCx) );
    PushTable( argv, dup_cstr ("-exec") );
    PushTable( argv, dup_cstr ("-exe") );
    PushTable( argv, dup_cstr (cstr_AlphaTab (&ospc->cmd)) );

    if (ospc->of)
    {
      cloexec_sysCx (xfd[1], true);
      PushTable( argv, dup_cstr ("-stdxfd") );
      PushTable( argv, itoa_dup_cstr (xfd[0]) );
      /* PushTable( argv, dup_cstr ("-closefd") ); */
      /* PushTable( argv, itoa_dup_cstr (xfd[1]) ); */
    }
    if (ospc->xf)
    {
      cloexec_sysCx (ofd[0], true);
      /* PushTable( argv, dup_cstr ("-closefd") ); */
      /* PushTable( argv, itoa_dup_cstr (ofd[0]) ); */
      PushTable( argv, dup_cstr ("-stdofd") );
      PushTable( argv, itoa_dup_cstr (ofd[1]) );
    }
    PushTable( argv, dup_cstr ("--") );
    nfrees = argv.sz;

    for (i = 0; i < ospc->args.sz; ++i) {
      PushTable( argv, cstr_AlphaTab (&ospc->args.s[i]) );
    }

    PushTable( argv, 0 );
  }

  DoLegitP( ospc->pid >= 0, "spawn" )
    ospc->pid = spawnvp_sysCx (argv.s);

  DoLegit( 0 )
  {
    /* The old switcharoo. Your input is my output and vice-versa.*/
    if (ospc->of)
    {
      closefd_sysCx (xfd[0]);
      ospc->ofb.fb.fd = xfd[1];
      set_FILE_FileB (&ospc->ofb.fb, fdopen_sysCx (ospc->ofb.fb.fd, "wb"));
    }
    if (ospc->xf)
    {
      closefd_sysCx (ofd[1]);
      ospc->xfb.fb.fd = ofd[0];
      set_FILE_FileB (&ospc->xfb.fb, fdopen_sysCx (ospc->xfb.fb.fd, "rb"));
    }
  }

  for (i = 0; i < nfrees; ++i) {
    free (argv.s[i]);
  }
  LoseTable( argv );

  return good;
}

