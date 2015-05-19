
#include "cx/syscx.h"
#include "cx/ofile.h"

  int
main (int argc, char** argv)
{
  int argi =
    (init_sysCx (&argc, &argv),
     1);
  OFile* of = stdout_OFile ();
  bool space = true;
  bool newline = true;

  while (argi < argc && argv[argi][0] == '-')
  {
    char* arg = argv[argi++];

    if (eql_cstr (arg, "--"))
    {
      break;
    }
    else if (eql_cstr (arg, "-s"))
    {
      space = false;
    }
    else if (eql_cstr (arg, "-n"))
    {
      newline = false;
    }
    else
    {
      DBog1( "Unrecognized option: %s", arg );
      failout_sysCx ("Unrecognized option!");
    }
  }

  while (argi < argc)
  {
    oput_cstr_OFile (of, argv[argi++]);
    if (space && argi < argc)
      oput_char_OFile (of, ' ');
  }
  if (newline)
    oput_char_OFile (of, '\n');
  lose_sysCx ();
  return 0;
}

