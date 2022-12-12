#include "fuzz_common.h"
#include "src/parse_fildesh.h"

#include <assert.h>
#include <string.h>
#include <fildesh/fildesh.h>


/** Test that line parsing doesn't crash.
 *
 * A "line" in this case should capture a meaningful command, so the
 * line parsing function is expected to skip comments and blank lines.
 **/
  int
LLVMFuzzerTestOneInput(const uint8_t data[], size_t size) {
  size_t text_nlines = 0;
  FildeshAlloc* alloc = open_FildeshAlloc();
  FildeshO tmp_out[1] = {DEFAULT_FildeshO};
  FildeshX in[1] = {DEFAULT_FildeshX};
  char* line = NULL;

  memcpy(grow_FildeshX(in, size), data, size);

  do {
    size_t old_nlines = text_nlines;
    line = fildesh_syntax_parse_line(in, &text_nlines, alloc, tmp_out);
    if (line) {
      assert(text_nlines > old_nlines);
    }
  } while (line != NULL);

  close_FildeshX(in);
  close_FildeshO(tmp_out);
  close_FildeshAlloc(alloc);
  return 0;
}

