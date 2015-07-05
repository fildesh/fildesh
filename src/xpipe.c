
/** This program functions somewhat like /xargs/ but simply spawns a
 * process for each different line of input and forwards that line of input
 * to the spawned process' stdin.
 **/

#include "cx/syscx.h"

#include "cx/ospc.h"

int main(int argc, char** argv)
{
  int argi = init_sysCx (&argc, &argv);
  XFile* xf = stdin_XFile ();
  DecloStack1( OSPc, ospc, dflt_OSPc () );

  if (argi >= argc)
    failout_sysCx ("Need at least one argument.");

  stdxpipe_OSPc (ospc);

  ospc->cmd = cons1_AlphaTab (argv[argi++]);
  while (argi < argc)
    PushTable( ospc->args, cons1_AlphaTab (argv[argi++]) );

  for (const char* s = getline_XFile (xf);
       s;
       s = getline_XFile (xf))
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

  lose_sysCx ();
  return 0;
}

