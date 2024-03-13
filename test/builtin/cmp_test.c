#include <assert.h>
#include <stdlib.h>

#include <fildesh/fildesh.h>

#include "include/fildesh/fildesh_compat_fd.h"
#include "include/fildesh/fildesh_compat_file.h"

int
fildesh_builtin_cmp_main(
    unsigned argc, char** argv,
    FildeshX** inputv, FildeshO** outputv);

static void
reset_argv(unsigned argc, char** argv, FildeshX** inputv, FildeshO** outputv) {
  static FildeshX dummy_in[1];
  static FildeshO dummy_out[1];
  unsigned i;
  *dummy_in = FildeshX_of_strlit("any\ncontent\n");
  *dummy_out = default_FildeshO();
  argv[0] = (char*)"cmp";
  inputv[0] = dummy_in;
  outputv[0] = dummy_out;
  for (i = 1; i <= argc; ++i) {
    argv[i] = NULL;
    inputv[i] = NULL;
    outputv[i] = NULL;
  }
}

static void
usage_test(char* bad_filename)
{
  unsigned argc;
  char* argv[5];
  FildeshX* inputv[5];
  FildeshO* outputv[5];
  int exstatus;

  argc = 3;
  reset_argv(argc, argv, inputv, outputv);
  argv[1] = (char*)"/dev/null";
  argv[2] = (char*)"/dev/null";
  exstatus = fildesh_builtin_cmp_main(argc, argv, inputv, outputv);
  assert(exstatus == 0);

  argc = 2;
  reset_argv(argc, argv, inputv, outputv);
  argv[1] = (char*)"-";
  exstatus = fildesh_builtin_cmp_main(argc, argv, inputv, outputv);
  assert(exstatus == 64);

  argc = 4;
  reset_argv(argc, argv, inputv, outputv);
  argv[1] = (char*)"/dev/null";
  argv[2] = (char*)"/dev/null";
  argv[3] = (char*)"too_many_files";
  exstatus = fildesh_builtin_cmp_main(argc, argv, inputv, outputv);
  assert(exstatus == 64);

  argc = 3;
  reset_argv(argc, argv, inputv, outputv);
  argv[1] = (char*)"-o";
  argv[2] = bad_filename;
  exstatus = fildesh_builtin_cmp_main(argc, argv, inputv, outputv);
  assert(exstatus == 73);

  /* First open should fail.*/
  argc = 3;
  reset_argv(argc, argv, inputv, outputv);
  argv[1] = bad_filename;
  argv[2] = (char*)"-";
  exstatus = fildesh_builtin_cmp_main(argc, argv, inputv, outputv);
  assert(exstatus == 66);

  /* Second open should fail.*/
  argc = 3;
  reset_argv(argc, argv, inputv, outputv);
  argv[1] = (char*)"-";
  argv[2] = (char*)"-";
  exstatus = fildesh_builtin_cmp_main(argc, argv, inputv, outputv);
  assert(exstatus == 66);
}

int main(int argc, char** argv) {
  char* bad_filename = fildesh_compat_file_catpath(argv[0], "no_file_here");
  assert(argc == 1);

  usage_test(bad_filename);

  free(bad_filename);
  return 0;
}
