
/** This program functions somewhat like /xargs/ but simply spawns a
 * process for each different line of input and forwards that line of input
 * to the spawned process' stdin.
 **/

#include "lace.h"
#include "utilace.h"

#include "cx/ospc.h"
#ifdef LACE_BUILTIN_FORBID_XPIPE
#include <assert.h>
#endif

static
  void
run_with_line(unsigned argc, char** argv, const char* line, LaceO* out)
{
  unsigned argi = 0;
  OSPc ospc[] = {DEFAULT_OSPc};

  ospc->cmd = cons1_AlphaTab (argv[argi++]);
  if (lace_specific_util(ccstr_of_AlphaTab(&ospc->cmd))) {
    PushTable( ospc->args, cons1_AlphaTab("-as") );
    PushTable( ospc->args, cons1_AlphaTab(ccstr_of_AlphaTab(&ospc->cmd)) );
    copy_cstr_AlphaTab(&ospc->cmd, exename_of_sysCx());
  }
  while (argi < argc)
    PushTable( ospc->args, cons1_AlphaTab (argv[argi++]) );

  stdxpipe_OSPc(ospc);
  stdopipe_OSPc(ospc);
  if (!spawn_OSPc(ospc))
    failout_sysCx("spawn() failed!");

  oput_cstr_OFile(ospc->of, line);
  oput_char_OFile(ospc->of, '\n');
  close_OFile(ospc->of);

  xget_XFile(ospc->xf);
  puts_LaceO(out, ccstr_of_XFile(ospc->xf));
  flush_LaceO(out);

  if (!close_OSPc(ospc))
    failout_sysCx("Wait failed!");
  if (ospc->status != 0) {
    DBog1( "Child exited with status:%d, exiting!", ospc->status );
    failout_sysCx ("");
  }
  lose_OSPc(ospc);
}

  int
main_xpipe(int argi, int argc, char** argv)
{
#ifdef LACE_BUILTIN_FORBID_XPIPE
  assert(false && "xpipe is known not to work on Windows");
  return 1;
#else
  LaceX* in = NULL;
  LaceO* out = NULL;
  const char* s;

  if (argi >= argc)
    failout_sysCx ("Need at least one argument.");

  in = open_LaceXF("-");
  if (!in) {
    failout_sysCx ("open() failed!");
  }
  out = open_LaceOF("-");
  if (!out) {
    failout_sysCx ("open() failed!");
  }

  for (s = getline_LaceX(in); s; s = getline_LaceX(in)) {
    run_with_line(argc - argi, &argv[argi], s, out);
  }

  close_LaceX(in);
  close_LaceO(out);
  return 0;
#endif
}

#ifndef MAIN_LACE_EXECUTABLE
  int
main(int argc, char** argv)
{
  int argi = init_sysCx(&argc, &argv);
  int istat = 1;
  failout_sysCx("This builtin relies on Lace's -as flag.");
  /* istat = main_xpipe(argi, argc, argv); */
  lose_sysCx();
  return istat;
}
#endif
