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
  const unsigned char* replacement;
  size_t needle_size;
  size_t replacement_size;

  if (argc != 3) {
    fildesh_log_error("Need exactly 2 arguments");
    return 64;
  }
  needle = (const unsigned char*) argv[1];
  replacement = (const unsigned char*) argv[2];
  needle_size = strlen(argv[1]);
  replacement_size = strlen(argv[2]);

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
    FildeshX slice = DEFAULT_FildeshX;
    FildeshX span;
    slice.at = &in->at[in->off];
    slice.size = in->size - in->off;
    slice.flush_lgsize = 0;
    for (span = until_bytestring_FildeshX(&slice, needle, needle_size);
         span.at;
         span = until_bytestring_FildeshX(&slice, needle, needle_size))
    {
      put_bytestring_FildeshO(out, (unsigned char*) span.at, span.size);
      if (skip_bytestring_FildeshX(&slice, NULL, needle_size)) {
        put_bytestring_FildeshO(out, replacement, replacement_size);
      }
    }
    in->off += slice.off;
    maybe_flush_FildeshX(in);
  }
  if (exstatus == 0) {
    put_bytestring_FildeshO(out, (unsigned char*) &in->at[in->off], in->size - in->off);
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
