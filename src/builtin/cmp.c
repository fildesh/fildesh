/**
 * Compare two files.
 **/

#include <fildesh/fildesh.h>
#include <assert.h>
#include <string.h>

  int
fildesh_builtin_cmp_main(unsigned argc, char** argv,
                      FildeshX** inputs, FildeshO** outputs)
{
  unsigned argi;
  size_t byte_count;
  FildeshX* lhs = NULL;
  FildeshX* rhs = NULL;
  FildeshO* out = NULL;
  bool equal = true;
  int exstatus = 0;

  for (argi = 1; argi < argc && exstatus == 0; ++argi) {
    if (0 == strcmp(argv[argi], "-o")) {
      out = open_arg_FildeshOF(++argi, argv, outputs);
      if (!out) {
        fildesh_log_errorf("Cannot open -o: %s", argv[argi]);
        exstatus = 73;
      }
    } else if (!lhs) {
      lhs = open_arg_FildeshXF(argi, argv, inputs);
      if (!lhs) {
        fildesh_log_errorf("Cannot open LHS: %s", argv[argi]);
        exstatus = 66;
      }
    } else if (!rhs) {
      rhs = open_arg_FildeshXF(argi, argv, inputs);
      if (!rhs) {
        fildesh_log_errorf("Cannot open RHS: %s", argv[argi]);
        exstatus = 66;
      }
    } else {
      fildesh_log_errorf("Unknown arg: ", argv[argi]);
      exstatus = 64;
    }
  }

  if (!out && outputs && outputs[0]) {
    out = open_arg_FildeshOF(0, argv, outputs);
    assert(out);
    if (!out) {exstatus = 70;}
  }

  if (lhs && !rhs) {
    rhs = open_arg_FildeshXF(0, argv, inputs);
  }

  if (exstatus == 0 && (!lhs || !rhs)) {
    exstatus = 64;
  }

  if (exstatus != 0) {
    close_FildeshX(lhs);
    close_FildeshX(rhs);
    close_FildeshO(out);
    return exstatus;
  }

  byte_count = 0;
  read_FildeshX(lhs);
  read_FildeshX(rhs);
  while (lhs->off < lhs->size && rhs->off < rhs->size) {
    if (lhs->at[lhs->off] != rhs->at[rhs->off]) {
      equal = false;
      break;
    }
    byte_count += 1;
    lhs->off += 1;
    rhs->off += 1;
    if (lhs->off == lhs->size) {
      flush_FildeshX(lhs);
      read_FildeshX(lhs);
    }
    if (rhs->off == rhs->size) {
      flush_FildeshX(rhs);
      read_FildeshX(rhs);
    }
  }

  equal = equal && (lhs->off == lhs->size && rhs->off == rhs->size);
  if (!equal && out) {
    putstrlit_FildeshO(out, "Difference after ");
    print_size_FildeshO(out, byte_count);
    putstrlit_FildeshO(out, " bytes (");
    if (lhs->off < lhs->size) {
      print_size_FildeshO(out, (unsigned)lhs->at[lhs->off]);
    } else {
      putstrlit_FildeshO(out, "EOF");
    }
    putstrlit_FildeshO(out, " != ");
    if (rhs->off < rhs->size) {
      print_size_FildeshO(out, (unsigned)rhs->at[rhs->off]);
    } else {
      putstrlit_FildeshO(out, "EOF");
    }
    putstrlit_FildeshO(out, ").\n");
  }

  close_FildeshX(lhs);
  close_FildeshX(rhs);
  close_FildeshO(out);
  return (equal ? 0 : 1);
}

#if !defined(FILDESH_BUILTIN_LIBRARY) && !defined(UNIT_TESTING)
int main(int argc, char** argv) {
  return fildesh_builtin_cmp_main((unsigned)argc, argv, NULL, NULL);
}
#endif
