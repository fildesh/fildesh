/** Simple utility to echo lines until (and including) a matching one.
 **/

#include <fildesh/fildesh.h>

#include <string.h>

  int
fildesh_builtin_delimend_main(
    unsigned argc, char** argv,
    FildeshX** inputv, FildeshO** outputv)
{
  int exstatus = 0;
  FildeshX* in = NULL;
  FildeshO* out = NULL;
  const unsigned char* delim = NULL;
  const unsigned char* term = NULL;
  size_t delim_size = 0;
  size_t term_size = 0;

  in = open_arg_FildeshXF(0, argv, inputv);
  out = open_arg_FildeshOF(0, argv, outputv);

  if (exstatus == 0 && !in) {exstatus = 66;}
  if (exstatus == 0 && !out) {exstatus = 73;}

  if (exstatus == 0 && argc != 3) {
    fildesh_log_error("Require 2 args: A delimiter and a halting condition.");
    exstatus = 64;
  }
  if (exstatus == 0) {
    delim = (const unsigned char*)argv[1];
    term = (const unsigned char*)argv[2];
    delim_size = strlen(argv[1]);
    term_size = strlen(argv[2]);
  }

  if (exstatus == 0) {
    FildeshX slice;
    for (slice = until_bytestring_FildeshX(in, delim, delim_size);
         slice.at;
         slice = until_bytestring_FildeshX(in, delim, delim_size))
    {
      put_bytestring_FildeshO(out, (const unsigned char*)slice.at, slice.size);

      if (skip_bytestring_FildeshX(in, NULL, delim_size)) {
        put_bytestring_FildeshO(out, delim, delim_size);
        flush_FildeshO(out);
      }
      else {
        flush_FildeshO(out);
        break;
      }

      if (slice.size == term_size) {
        if (0 == memcmp(slice.at, term, term_size)) {
          break;
        }
      }
    }
  }

  close_FildeshO(out);
  close_FildeshX(in);
  return exstatus;
}

#ifndef FILDESH_BUILTIN_LIBRARY
int main(int argc, char** argv) {
  return fildesh_builtin_zec_main(argc, argv, NULL, NULL);
}
#endif
