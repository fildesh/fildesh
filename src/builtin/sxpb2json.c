#include <fildesh/sxproto.h>

  int
fildesh_builtin_sxpb2json_main(
    unsigned argc, char** argv,
    FildeshX** inputv, FildeshO** outputv)
{
  int exstatus = 0;
  FildeshX* in = open_arg_FildeshXF(0, argv, inputv);
  FildeshO* out = open_arg_FildeshOF(0, argv, outputv);
  FildeshO* err_out = open_FildeshOF("/dev/stderr");

  if (argc != 1) {
    exstatus = 64;
    fildesh_log_error("No support for args.");
    close_FildeshX(in);
  }

  if (exstatus == 0) {
    FildeshSxpb* sxpb = slurp_sxpb_close_FildeshX(in, NULL, err_out);
    if (!sxpb) {
      exstatus = 1;
    }
    if (exstatus == 0) {
      print_json_FildeshO(out, sxpb);
      close_FildeshSxpb(sxpb);
    }
  }
  close_FildeshO(out);
  close_FildeshO(err_out);
  return exstatus;
}

#ifndef FILDESH_BUILTIN_LIBRARY
int main(int argc, char** argv) {
  return fildesh_builtin_sxpb2json_main(argc, argv, NULL, NULL);
}
#endif
