/**
 * Compare exit status with expectation.
 **/

#include <assert.h>
#include <string.h>

#define FILDESH_LOG_TRACE_ON 1
#include <fildesh/fildesh.h>

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
  FildeshO* out = NULL;
  char custom_message_logtype = '\0';
  const char* custom_message = NULL;

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
        fildesh_log_errorf("Failed to open for reading: %s", argv[argi]);
        exstatus = 66;
      }
    }
    else if (0 == strcmp(argv[argi], "-propagate")) {
      propagating_exit_status = true;
    }
    else if (0 == strcmp(argv[argi], "-o?")) {
      out = open_arg_FildeshOF(++argi, argv, outputv);
      if (!out) {
        fildesh_log_errorf("Failed to open for writing: %s", argv[argi]);
        exstatus = 73;
      }
    }
    else if (0 == strcmp(argv[argi], "-status")) {
      const char* arg = argv[++argi];
      if (!arg || !fildesh_parse_int(&expect_status, arg)) {
        fildesh_log_error("Cannot parse expected status from -status flag.");
        exstatus = 65;
      }
    }
    else if (0 == strcmp(argv[argi], "-error") ||
             0 == strcmp(argv[argi], "-warning") ||
             0 == strcmp(argv[argi], "-info") ||
             0 == strcmp(argv[argi], "-trace"))
    {
      custom_message_logtype = argv[argi][1];
      argi += 1;
      custom_message = argv[argi];
      if (!custom_message) {
        fildesh_log_errorf("Provide a message after %s flag.", argv[argi-1]);
        exstatus = 64;
      }
    }
    else {
      fildesh_log_errorf("Unknown arg: ", argv[argi]);
      exstatus = 64;
    }
  }

  if (exstatus != 0) {
  }
  else if (custom_message && !out) {
    fildesh_log_error("Custom status message must be paired with -o? fallthru output flag.");
    exstatus = 64;
  }
  else if (propagating_exit_status) {
    if (out) {
      fildesh_log_error("-propagate and -o? cannot be used together.");
      exstatus = 64;
    }
    else if (expect_status >= 0 && result_status >= 0) {
      fildesh_log_error("Can only -propagate one -status or -x? value.");
      exstatus = 64;
    }
    else if (expect_status >= 0) {
      exstatus = expect_status;
    }
    else if (result_status >= 0) {
      exstatus = result_status;
    }
    else {
      fildesh_log_error("Need a -status or -x? value to -propagate.");
      exstatus = 64;
    }
  }
  else if (result_status < 0) {
    fildesh_log_error("Missing -x? input argument.");
    exstatus = 64;
  }
  else if (out) {
    if (expect_status == result_status ||
        (expect_status < 0 && result_status > 0))
    {
      switch (custom_message_logtype) {
        case 'e': {fildesh_log_errorf("%s", custom_message); break;}
        case 'w': {fildesh_log_warningf("%s", custom_message); break;}
        case 'i': {fildesh_log_infof("%s", custom_message); break;}
        case 't': {fildesh_log_tracef("%s", custom_message); break;}
        default: {assert(custom_message_logtype == '\0'); break;}
      }
      putc_FildeshO(out, '0');
    }
    else {
      print_int_FildeshO(out, result_status);
    }
  }
  else {
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
  close_FildeshO(out);
  return exstatus;
}

#if !defined(FILDESH_BUILTIN_LIBRARY) && !defined(UNIT_TESTING)
int main(int argc, char** argv) {
  return fildesh_builtin_expect_failure_main((unsigned)argc, argv, NULL, NULL);
}
#endif
