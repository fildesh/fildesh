/**
 * Compare a file with the output of a command.
 *
 * Usage example:
 *   comparispawn expected-output.txt ./bin/myprog arg1 arg2
 **/

#include "lace.h"
#include "lace_compat_fd.h"
#include "lace_tool.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct ComparispawnFnArg ComparispawnFnArg;
struct ComparispawnFnArg {
  const char** argv;
  int status;
};

LACE_TOOL_PIPEM_CALLBACK(run_fn, ComparispawnFnArg*, st) {
  const lace_compat_fd_t fds_to_close[] = {1, -1};
  st->status = lace_compat_fd_spawnvp_wait(
      fds_to_close, (const char**)st->argv);
}

int main(int argc, char** argv)
{
  LaceX* expect_in = NULL;
  LaceX* actual_in = NULL;
  char* output_data = NULL;
  size_t output_size;
  ComparispawnFnArg st[1];
  unsigned line_id;
  bool all_match = true;

  if (argc < 3 || !argv[1] || !argv[2]) {return 64;}

  st->argv = (const char**) &argv[2];
  st->status = -1;
  output_size = lace_tool_pipem(
      0, NULL, -1,
      run_fn, st,
      1, &output_data);

  if (st->status != 0) {
    fprintf(stderr, "ERROR from comparispawn: %s exited with status %d.\n",
            argv[2], st->status);
    if (output_data) {free(output_data);}
    return 1;
  }

  expect_in = open_LaceXF(argv[1]);
  if (!expect_in) {
    if (output_data) {free(output_data);}
    fprintf(stderr,
            "ERROR from comparispawn: Could not open '%s' for reading.\n",
            argv[1]);
    return 1;
  }
  actual_in = open_LaceXA();
  memcpy(grow_LaceX(actual_in, output_size), output_data, output_size);

  for (line_id = 1; all_match; ++line_id) {
    const char* expect_s = getline_LaceX(expect_in);
    const char* actual_s = getline_LaceX(actual_in);

    if (!expect_s || !actual_s) {
      if (expect_s) {
        all_match = false;
        fprintf(stderr,
                "ERROR from comparispawn: No line %u present.\n"
                " Expected: %s\n",
                line_id, expect_s);
      }
      if (actual_s) {
        all_match = false;
        fprintf(stderr,
                "ERROR from comparispawn: Extra line %u.\n"
                " Got: %s\n",
                line_id, actual_s);
      }
      break;
    }
    if (0 != strcmp(expect_s, actual_s)) {
      all_match = false;
      fprintf(stderr,
              "ERROR from comparispawn: Difference found on line %u.\n"
              " Expected: %s\n Got: %s\n",
              line_id, expect_s, actual_s);
    }
  }
  free(output_data);
  close_LaceX(actual_in);
  close_LaceX(expect_in);
  return (all_match ? 0 : 1);
}

