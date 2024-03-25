/**
 * Compare two text files.
 **/
#include <assert.h>
#include <string.h>

#include <fildesh/fildesh.h>

static
  void
print_diff_error(
    FildeshO* out, size_t line_count,
    const FildeshX* lhs, const FildeshX* rhs)
{
  if (!out) {return;}
  assert(lhs || rhs);
  if (!lhs) {
    putstrlit_FildeshO(out, "Difference found. No LHS line ");
  }
  else if (!rhs) {
    putstrlit_FildeshO(out, "Difference found. No RHS line ");
  }
  else {
    putstrlit_FildeshO(out, "Difference found on line ");
  }
  print_size_FildeshO(out, line_count);
  putc_FildeshO(out, '.');
  if (lhs) {
    putstrlit_FildeshO(out, "\n LHS: ");
    putslice_FildeshO(out, *lhs);
  }
  if (rhs) {
    putstrlit_FildeshO(out, "\n RHS: ");
    putslice_FildeshO(out, *rhs);
  }
  putc_FildeshO(out, '\n');
}

  int
fildesh_builtin_cmptxt_main(
    unsigned argc, char** argv,
    FildeshX** inputv, FildeshO** outputv)
{
  unsigned argi;
  size_t line_count;
  FildeshX* lhs = NULL;
  FildeshX* rhs = NULL;
  FildeshO* out = NULL;
  int exstatus = 0;

  for (argi = 1; argi < argc && exstatus == 0; ++argi) {
    if (0 == strcmp(argv[argi], "-o")) {
      out = open_arg_FildeshOF(++argi, argv, outputv);
      if (!out) {
        fildesh_log_errorf("Cannot open -o: %s", argv[argi]);
        exstatus = 73;
      }
    }
    else if (!lhs) {
      lhs = open_arg_FildeshXF(argi, argv, inputv);
      if (!lhs) {
        fildesh_log_errorf("Cannot open LHS: %s", argv[argi]);
        exstatus = 66;
      }
    }
    else if (!rhs) {
      rhs = open_arg_FildeshXF(argi, argv, inputv);
      if (!rhs) {
        fildesh_log_errorf("Cannot open RHS: %s", argv[argi]);
        exstatus = 66;
      }
    }
    else {
      fildesh_log_errorf("Unknown arg: ", argv[argi]);
      exstatus = 64;
    }
  }

  if (exstatus == 0 && !rhs) {
    rhs = open_arg_FildeshXF(0, argv, inputv);
  }
  else if (inputv && inputv[0]) {
    close_FildeshX(open_arg_FildeshXF(0, argv, inputv));
  }

  if (exstatus == 0 && !out) {
    out = open_arg_FildeshOF(0, argv, outputv);
  }
  else if (outputv && outputv[0]) {
    close_FildeshO(open_arg_FildeshOF(0, argv, outputv));
  }

  if (exstatus == 0 && (!lhs || !rhs)) {
    exstatus = 64;
  }

  for (line_count = 1; exstatus == 0; ++line_count) {
    FildeshX lhs_slice = sliceline_FildeshX(lhs);
    FildeshX rhs_slice = sliceline_FildeshX(rhs);

    if (!avail_FildeshX(&lhs_slice) || !avail_FildeshX(&rhs_slice)) {
      if (avail_FildeshX(&lhs_slice)) {
        exstatus = 1;
        print_diff_error(out, line_count, &lhs_slice, NULL);
      }
      if (avail_FildeshX(&rhs_slice)) {
        exstatus = 1;
        print_diff_error(out, line_count, NULL, &rhs_slice);
      }
      break;
    }
    if (0 != fildesh_compare_bytestring(
            bytestring_of_FildeshX(&lhs_slice),
            bytestring_of_FildeshX(&rhs_slice)))
    {
      exstatus = 1;
      print_diff_error(out, line_count, &lhs_slice, &rhs_slice);
    }
  }

  close_FildeshX(lhs);
  close_FildeshX(rhs);
  close_FildeshO(out);
  return exstatus;
}

#if !defined(FILDESH_BUILTIN_LIBRARY) && !defined(UNIT_TESTING)
int main(int argc, char** argv) {
  return fildesh_builtin_cmptxt_main((unsigned)argc, argv, NULL, NULL);
}
#endif
