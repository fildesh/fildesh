#include <assert.h>
#include <stdlib.h>

#include <fildesh/fildesh.h>

#include "include/fildesh/fildesh_compat_fd.h"
#include "include/fildesh/fildesh_compat_file.h"
#include "fildesh_tool.h"

int
fildesh_builtin_cmptxt_main(
    unsigned argc, char** argv,
    FildeshX** inputv, FildeshO** outputv);

typedef struct CallbackParam CallbackParam;
struct CallbackParam {
  FildeshX lhs[1];
  FildeshX rhs[1];
  int exstatus;
};

FILDESH_TOOL_PIPEM_CALLBACK(run_cmptxt, in_fd, out_fd, CallbackParam*, pa) {
  const unsigned argc = 3;
  char* argv[] = {(char*)"cmptxt", (char*)"", (char*)"", NULL};
  FildeshX* inputv[] = {NULL, NULL, NULL, NULL};
  FildeshO* outputv[] = {NULL, NULL, NULL, NULL};
  (void)in_fd;
  inputv[1] = pa->lhs;
  inputv[2] = pa->rhs;
  outputv[0] = open_fd_FildeshO(out_fd);
  pa->exstatus = fildesh_builtin_cmptxt_main(argc, argv, inputv, outputv);
}

static void
print_skip_output(FildeshO* out, FildeshX* expect, const char* s, size_t n) {
  put_bytestring_FildeshO(out, (const unsigned char*)s, n);
  flush_FildeshO(out);
  skip_bytestring_FildeshX(expect, (const unsigned char*)s, n);
}

static void
comparison_test()
{
  FildeshO* err_out = open_FildeshOF("/dev/stderr");
  char* output_data = NULL;
  size_t output_size;
  CallbackParam pa[1];
  FildeshX expect[1];

  *pa->lhs = FildeshX_of_strlit("a\nb\nc\nd");
  *pa->rhs = FildeshX_of_strlit("a\nb\nc");
  *expect = FildeshX_of_strlit(
      "Difference found. No RHS line 4.\n LHS: d\n");
  output_size = fildesh_tool_pipem(0, NULL, run_cmptxt, pa, &output_data);
  assert(pa->exstatus == 1);
  print_skip_output(err_out, expect, output_data, output_size);
  assert(!avail_FildeshX(expect));

  *pa->lhs = FildeshX_of_strlit("a\nb\nc\n");
  *pa->rhs = FildeshX_of_strlit("a\nb\nc\nd\n");
  *expect = FildeshX_of_strlit(
      "Difference found. No LHS line 4.\n RHS: d\n");
  output_size = fildesh_tool_pipem(0, NULL, run_cmptxt, pa, &output_data);
  assert(pa->exstatus == 1);
  print_skip_output(err_out, expect, output_data, output_size);
  assert(!avail_FildeshX(expect));

  free(output_data);
  close_FildeshO(err_out);
}

static void
reset_argv(unsigned argc, char** argv, FildeshX** inputv, FildeshO** outputv) {
  static FildeshX dummy_in[1];
  static FildeshO dummy_out[1];
  unsigned i;
  *dummy_in = FildeshX_of_strlit("any\ncontent\n");
  *dummy_out = default_FildeshO();
  argv[0] = (char*)"cmptxt";
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
  exstatus = fildesh_builtin_cmptxt_main(argc, argv, inputv, outputv);
  assert(exstatus == 0);

  argc = 2;
  reset_argv(argc, argv, inputv, outputv);
  argv[1] = (char*)"-";
  exstatus = fildesh_builtin_cmptxt_main(argc, argv, inputv, outputv);
  assert(exstatus == 64);

  argc = 4;
  reset_argv(argc, argv, inputv, outputv);
  argv[1] = (char*)"/dev/null";
  argv[2] = (char*)"/dev/null";
  argv[3] = (char*)"too_many_files";
  exstatus = fildesh_builtin_cmptxt_main(argc, argv, inputv, outputv);
  assert(exstatus == 64);

  argc = 3;
  reset_argv(argc, argv, inputv, outputv);
  argv[1] = (char*)"-o";
  argv[2] = bad_filename;
  exstatus = fildesh_builtin_cmptxt_main(argc, argv, inputv, outputv);
  assert(exstatus == 73);
  assert(!inputv[0]);
  assert(!outputv[0]);

  /* First open should fail.*/
  argc = 3;
  reset_argv(argc, argv, inputv, outputv);
  argv[1] = bad_filename;
  argv[2] = (char*)"-";
  exstatus = fildesh_builtin_cmptxt_main(argc, argv, inputv, outputv);
  assert(exstatus == 66);

  /* Second open should fail.*/
  argc = 3;
  reset_argv(argc, argv, inputv, outputv);
  argv[1] = (char*)"-";
  argv[2] = (char*)"-";
  exstatus = fildesh_builtin_cmptxt_main(argc, argv, inputv, outputv);
  assert(exstatus == 66);
}

int main(int argc, char** argv) {
  char* bad_filename = fildesh_compat_file_catpath(argv[0], "no_file_here");
  assert(argc == 1);

  comparison_test();
  usage_test(bad_filename);

  free(bad_filename);
  return 0;
}
