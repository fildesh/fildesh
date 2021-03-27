
/** This program functions somewhat like /xargs/ but simply spawns a
 * process for each different line of input and forwards that line of input
 * to the spawned process' stdin.
 **/

#include "lace.h"
#include "utilace.h"

#include "cx/ospc.h"

LaceUtilMain(xpipe)
{
  LaceXF xf[1] = {DEFAULT_LaceXF};
  OSPc ospc[] = {DEFAULT_OSPc};
  const char* s;

  if (argi >= argc)
    failout_sysCx ("Need at least one argument.");

  open_LaceXF(xf, "-");
  stdxpipe_OSPc (ospc);

  ospc->cmd = cons1_AlphaTab (argv[argi++]);
  if (lace_specific_util(ccstr_of_AlphaTab(&ospc->cmd))) {
    PushTable( ospc->args, cons1_AlphaTab("-as") );
    PushTable( ospc->args, cons1_AlphaTab(ccstr_of_AlphaTab(&ospc->cmd)) );
    copy_cstr_AlphaTab(&ospc->cmd, exename_of_sysCx());
  }
  while (argi < argc)
    PushTable( ospc->args, cons1_AlphaTab (argv[argi++]) );

  for (s = getline_LaceX(&xf->base);
       s;
       s = getline_LaceX(&xf->base))
  {
    if (!spawn_OSPc (ospc))
      failout_sysCx ("spawn() failed!");

    oput_cstr_OFile (ospc->of, s);
    oput_char_OFile (ospc->of, '\n');
    if (!close_OSPc (ospc))
      failout_sysCx ("Wait failed!");
    if (ospc->status != 0)
    {
      DBog1( "Child exited with status:%d, exiting!", ospc->status );
      failout_sysCx ("");
    }
  }

  close_LaceX(&xf->base);
  lose_OSPc (ospc);
  lose_sysCx ();
  return 0;
}

