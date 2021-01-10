/**
 * Compare a file with the output of a command.
 *
 * Usage example:
 *   comparispawn expected-output.txt ./bin/myprog arg1 arg2
 **/

#include "cx/syscx.h"
#include "cx/ospc.h"

  int
main (int argc, char** argv)
{
  int argi = init_sysCx (&argc, &argv);
  DeclLegit( good );
  XFileB xfb[1];
  XFile* xf_expect;
  OSPc ospc[1];
  XFile* xf_result;
  OFile* logf = stderr_OFile ();
  uint nlines;

  init_XFileB (xfb);
  xf_expect = &xfb->xf;

  init_OSPc (ospc);
  stdopipe_OSPc (ospc);
  xf_result = ospc->xf;

  DoLegitLine( "File open" )
    open_FileB (&xfb->fb, 0, argv[argi++]);

  DoLegitLine( "Need a file and a command!" )
    (argi < argc);

  DoLegit( 0 ) {
    ospc->cmd = cons1_AlphaTab (argv[argi++]);
    while (argi < argc) {
      PushTable( ospc->args, cons1_AlphaTab (argv[argi++]) );
    }
  }

  DoLegitLine( "Spawn" )
    spawn_OSPc (ospc);


  for (nlines = 1; good; nlines += 1) {
    const char* line_expect = getline_XFile (xf_expect);
    const char* line_result = getline_XFile (xf_result);
    if (!line_expect && !line_result)  break;

    if (eq_cstr (line_expect, line_result))  continue;

    good = false;

    oput_cstr_OFile (logf, "Difference on line:");
    oput_uint_OFile (logf, nlines);
    if (line_expect) {
      oput_cstr_OFile (logf, "\nexpect:");
      oput_cstr_OFile (logf, line_expect);
    }
    else {
      oput_cstr_OFile (logf, "\nexpect end of file");
    }

    if (line_result) {
      oput_cstr_OFile (logf, "\nresult:");
      oput_cstr_OFile (logf, line_result);
    }
    else {
      oput_cstr_OFile (logf, "\nresult end of file");
    }

    oput_cstr_OFile (logf, "\n");
  }

  flush_OFile (logf);
  lose_XFileB (xfb);
  lose_OSPc (ospc);
  lose_sysCx ();

  return OneIf(!good);
}

