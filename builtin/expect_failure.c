/**
 * Compare two files.
 **/

#include "fildesh.h"
#include <string.h>

  int
fildesh_builtin_expect_failure_main(
    unsigned argc, char** argv,
    FildeshX** inputv, FildeshO** outputv)
{
  unsigned argi;
  bool propagating_exit_status = false;
  int result_status = -1;
  int expect_status = -1;
  int exstatus = 0;
  (void) outputv;  /* Not used.*/

  for (argi = 1; argi < argc && exstatus == 0; ++argi) {
    if (0 == strcmp(argv[argi], "-x?")) {
      FildeshX* in = open_arg_FildeshXF(++argi, argv, inputv);
      if (in) {
        if (!parse_int_FildeshX(in, &result_status)) {
          if (in->size == 0) {
            fildesh_log_error(
                "Cannot parse exit status from -x? input. Stream is empty!");
          } else {
            fildesh_log_error("Cannot parse exit status from -x? input.");
          }
          exstatus = 65;
        }
        close_FildeshX(in);
      } else {
        fildesh_log_errorf("Cannot open -x?: %s", argv[argi]);
        exstatus = 66;
      }
    } else if (0 == strcmp(argv[argi], "-status")) {
      const char* arg = argv[++argi];
      if (!arg || !fildesh_parse_int(&expect_status, arg)) {
        fildesh_log_error("Cannot parse expected status from -status flag.");
        exstatus = 65;
      }
    } else if (0 == strcmp(argv[argi], "-propagate")) {
      propagating_exit_status = true;
    } else {
      fildesh_log_errorf("Unknown arg: ", argv[argi]);
      exstatus = 64;
    }
  }

  if (exstatus == 0 && propagating_exit_status) {
    if (expect_status >= 0 && result_status >= 0) {
      fildesh_log_error("Can only -propagate one -status or -x? value.");
      exstatus = 64;
    } else if (expect_status >= 0) {
      exstatus = expect_status;
    } else if (result_status >= 0) {
      exstatus = result_status;
    } else {
      fildesh_log_error("Need a -status or -x? value to -propagate.");
      exstatus = 64;
    }
  } else if (exstatus == 0 && result_status < 0) {
    fildesh_log_error("Missing -x? input argument.");
    exstatus = 64;
  }

  if (exstatus == 0 && !propagating_exit_status) {
    if (expect_status < 0) {
      if (result_status == 0) {
        fildesh_log_error("Expected failure but got exit status: 0");
        exstatus = 1;
      }
    } else {
      if (expect_status != result_status) {
        fildesh_log_errorf("Expected failure status %d but got: %d",
                           expect_status, result_status);
        exstatus = 1;
      }
    }
  }
  return exstatus;
}

#if !defined(FILDESH_BUILTIN_LIBRARY) && !defined(UNIT_TESTING)
int main(int argc, char** argv) {
  return fildesh_builtin_expect_failure_main((unsigned)argc, argv, NULL, NULL);
}
#endif
