/**
 * Compare a file with the output of a command.
 *
 * Usage example:
 *   comparispawn expected-output.txt ./bin/myprog arg1 arg2
 **/

#include <fildesh/fildesh.h>
#include "include/fildesh/fildesh_compat_fd.h"
#include "fildesh_tool.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct ComparispawnFnArg ComparispawnFnArg;
struct ComparispawnFnArg {
  const char** argv;
  int status;
};

FILDESH_TOOL_PIPEM_CALLBACK(run_fn, in_fd, out_fd, ComparispawnFnArg*, st) {
  st->status = fildesh_compat_fd_spawnvp_wait(
      in_fd, out_fd, 2, NULL, (const char**)st->argv);
}

int main(int argc, char** argv)
{
  FildeshX* expect_in = NULL;
  FildeshX* actual_in = NULL;
  char* output_data = NULL;
  size_t output_size;
  ComparispawnFnArg st[1];
  unsigned line_id;
  bool all_match = true;

  if (argc < 3 || !argv[1] || !argv[2]) {return 64;}

  st->argv = (const char**) &argv[2];
  st->status = -1;
  output_size = fildesh_tool_pipem(
      0, NULL,
      run_fn, st,
      &output_data);

  if (st->status != 0) {
    fprintf(stderr, "ERROR from comparispawn: %s exited with status %d.\n",
            argv[2], st->status);
    if (output_data) {free(output_data);}
    return 1;
  }

  expect_in = open_FildeshXF(argv[1]);
  if (!expect_in) {
    if (output_data) {free(output_data);}
    fprintf(stderr,
            "ERROR from comparispawn: Could not open '%s' for reading.\n",
            argv[1]);
    return 1;
  }
  actual_in = open_FildeshXA();
  memcpy(grow_FildeshX(actual_in, output_size), output_data, output_size);

  for (line_id = 1; all_match; ++line_id) {
    FildeshX expect_slice = sliceline_FildeshX(expect_in);
    FildeshX actual_slice = sliceline_FildeshX(actual_in);

    if (!avail_FildeshX(&expect_slice) || !avail_FildeshX(&actual_slice)) {
      if (avail_FildeshX(&expect_slice)) {
        all_match = false;
        fprintf(stderr,
                "ERROR from comparispawn: No line %u present.\n"
                " Expected: %.*s\n",
                line_id, (int)expect_slice.size, expect_slice.at);
      }
      if (avail_FildeshX(&actual_slice)) {
        all_match = false;
        fprintf(stderr,
                "ERROR from comparispawn: Extra line %u.\n"
                " Got: %.*s\n",
                line_id, (int)actual_slice.size, actual_slice.at);
      }
      break;
    }
    if (0 != fildesh_compare_bytestring(
            bytestring_of_FildeshX(&expect_slice),
            bytestring_of_FildeshX(&actual_slice)))
    {
      all_match = false;
      fprintf(stderr,
              "ERROR from comparispawn: Difference found on line %u.\n"
              " Expected: %.*s\n Got: %.*s\n",
              line_id,
              (int)expect_slice.size, expect_slice.at,
              (int)actual_slice.size, actual_slice.at);
    }
  }
  free(output_data);
  close_FildeshX(actual_in);
  close_FildeshX(expect_in);
  return (all_match ? 0 : 1);
}

