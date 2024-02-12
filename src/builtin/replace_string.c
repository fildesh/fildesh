/** Simple utility to replace all instances of a string.**/

#include <fildesh/fildesh.h>

#include <string.h>

  int
fildesh_builtin_replace_string_main(unsigned argc, char** argv,
                                    FildeshX** inputv, FildeshO** outputv)
{
  FildeshX* in = NULL;
  FildeshO* out = NULL;
  int exstatus = 0;
  const unsigned char* needle;
  const unsigned char* replacement = NULL;
  size_t needle_size;
  size_t replacement_size = 0;

  if (argc != 2 && argc != 3) {
    fildesh_log_error("Need 2 or 3 arguments.");
    return 64;
  }
  needle = (const unsigned char*) argv[1];
  needle_size = strlen(argv[1]);
  if (needle_size == 0) {needle_size = 1;}

  if (argc == 3) {
    replacement = (const unsigned char*) argv[2];
    replacement_size = strlen(argv[2]);
    if (replacement_size == 0) {replacement_size = 1;}
  }

  in = open_arg_FildeshXF(0, argv, inputv);
  out = open_arg_FildeshOF(0, argv, outputv);

  if (exstatus == 0 && !in) {
    fildesh_log_error("Cannot open stdin");
    exstatus = 66;
  }
  if (exstatus == 0 && !out) {
    fildesh_log_error("Cannot open stdout");
    exstatus = 70;
  }

  while (exstatus == 0 && 0 < read_FildeshX(in)) {
    FildeshX slice = getslice_FildeshX(in);
    FildeshX span;
    for (span = until_bytestring_FildeshX(&slice, needle, needle_size);
         span.at;
         span = until_bytestring_FildeshX(&slice, needle, needle_size))
    {
      putslice_FildeshO(out, span);
      if (skip_bytestring_FildeshX(&slice, NULL, needle_size)) {
        put_bytestring_FildeshO(out, replacement, replacement_size);
      }
    }
    in->off += slice.off;
    maybe_flush_FildeshX(in);
  }
  if (exstatus == 0) {
    putslice_FildeshO(out, *in);
  }
  close_FildeshX(in);
  close_FildeshO(out);
  return exstatus;
}

#if !defined(FILDESH_BUILTIN_LIBRARY) && !defined(UNIT_TESTING)
  int
main(int argc, char** argv)
{
  return fildesh_builtin_replace_string_main((unsigned)argc, argv, NULL, NULL);
}
#endif
