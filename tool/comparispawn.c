/**
 * Compare a file with the output of a command.
 *
 * Usage example:
 *   comparispawn expected-output.txt ./bin/myprog arg1 arg2
 **/
#include <stdlib.h>

#include <fildesh/fildesh.h>

#include "include/fildesh/fildesh_compat_fd.h"
#include "fildesh_tool.h"

int
fildesh_builtin_cmptxt_main(
    unsigned argc, char** argv,
    FildeshX** inputv, FildeshO** outputv);


typedef struct ComparispawnFnArg ComparispawnFnArg;
struct ComparispawnFnArg {
  const char** argv;
  int status;
};

FILDESH_TOOL_PIPEM_CALLBACK(run_fn, in_fd, out_fd, ComparispawnFnArg*, st) {
  st->status = fildesh_compat_fd_spawnvp_wait(
      in_fd, out_fd, 2, NULL, (const char**)st->argv);
}


typedef struct CustomFildeshO CustomFildeshO;
struct CustomFildeshO {
  FildeshO o;
  FildeshO* err_out;
};
static void write_CustomFildeshO(CustomFildeshO* custom_out);
static void close_CustomFildeshO(CustomFildeshO* custom_out);
static void free_CustomFildeshO(CustomFildeshO* custom_out) {(void)custom_out;}
DEFINE_FildeshO_VTable(CustomFildeshO, o);
#define DEFAULT_CustomFildeshO { \
  DEFAULT1_FildeshO(DEFAULT_CustomFildeshO_FildeshO_VTable), \
  NULL, \
}

void write_CustomFildeshO(CustomFildeshO* custom_out) {
  if (!custom_out->err_out) {
    custom_out->err_out = open_FildeshOF("/dev/stderr");
    if (!custom_out->err_out) {return;}
    putstrlit_FildeshO(custom_out->err_out, "ERROR from comparispawn: ");
  }
  put_bytestring_FildeshO(
      custom_out->err_out,
      bytestring_of_FildeshO(&custom_out->o));
  custom_out->o.off = custom_out->o.size;
}

void close_CustomFildeshO(CustomFildeshO* custom_out) {
  close_FildeshO(custom_out->err_out);
  custom_out->err_out = NULL;
}


int main(int argc, char** argv)
{
  CustomFildeshO custom_out[1] = {DEFAULT_CustomFildeshO};
  FildeshO* out = &custom_out->o;
  char* output_data = NULL;
  size_t output_size;
  int exstatus = 0;

  if (argc < 3 || !argv[1] || !argv[2]) {return 64;}

  if (exstatus == 0) {
    ComparispawnFnArg st[1];
    st->argv = (const char**) &argv[2];
    st->status = -1;
    output_size = fildesh_tool_pipem(
        0, NULL,
        run_fn, st,
        &output_data);
    exstatus = st->status;
  }

  if (exstatus != 0) {
    putstr_FildeshO(out, argv[2]);
    putstrlit_FildeshO(out, " exited with status ");
    print_int_FildeshO(out, exstatus);
    putstrlit_FildeshO(out, ".\n");
    close_FildeshO(out);
    exstatus = 1;
  }
  else {
    FildeshX rhs_slice = FildeshX_of_bytestring(
        (const unsigned char*)output_data, output_size);
    char* cmptxt_argv[] = {(char*)"cmptxt", NULL, NULL};
    FildeshX* cmptxt_inputv[] = {NULL, NULL, NULL};
    FildeshO* cmptxt_outputv[] = {NULL, NULL, NULL};
    cmptxt_argv[1] = argv[1];
    cmptxt_inputv[0] = &rhs_slice;
    cmptxt_outputv[0] = out;
    exstatus = fildesh_builtin_cmptxt_main(
        2, cmptxt_argv, cmptxt_inputv, cmptxt_outputv);
  }

  if (output_data) {free(output_data);}
  return exstatus;
}

