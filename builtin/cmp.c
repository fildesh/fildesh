/**
 * Compare two files.
 **/

#include "lace.h"
#include <assert.h>
#include <string.h>

  int
lace_builtin_cmp_main(unsigned argc, char** argv,
                      LaceX** inputs, LaceO** outputs)
{
  unsigned argi;
  size_t byte_count;
  LaceX* lhs = NULL;
  LaceX* rhs = NULL;
  LaceO* out = NULL;
  bool equal = true;
  int exstatus = 0;

  for (argi = 1; argi < argc && exstatus == 0; ++argi) {
    if (0 == strcmp(argv[argi], "-o")) {
      out = open_arg_LaceOF(++argi, argv, outputs);
      if (!out) {
        lace_log_errorf("Cannot open -o: %s", argv[argi]);
        exstatus = 73;
      }
    } else if (!lhs) {
      lhs = open_arg_LaceXF(argi, argv, inputs);
      if (!lhs) {
        lace_log_errorf("Cannot open LHS: %s", argv[argi]);
        exstatus = 66;
      }
    } else if (!rhs) {
      rhs = open_arg_LaceXF(argi, argv, inputs);
      if (!rhs) {
        lace_log_errorf("Cannot open RHS: %s", argv[argi]);
        exstatus = 66;
      }
    } else {
      lace_log_errorf("Unknown arg: ", argv[argi]);
      exstatus = 64;
    }
  }

  if (!out && outputs && outputs[0]) {
    out = open_arg_LaceOF(0, argv, outputs);
    assert(out);
    if (!out) {exstatus = 70;}
  }

  if (lhs && !rhs) {
    rhs = open_arg_LaceXF(0, argv, inputs);
  }

  if (exstatus == 0 && (!lhs || !rhs)) {
    exstatus = 64;
  }

  if (exstatus != 0) {
    close_LaceX(lhs);
    close_LaceX(rhs);
    close_LaceO(out);
    return exstatus;
  }

  byte_count = 0;
  read_LaceX(lhs);
  read_LaceX(rhs);
  while (lhs->off < lhs->size && rhs->off < rhs->size) {
    if (lhs->at[lhs->off] != rhs->at[rhs->off]) {
      equal = false;
      break;
    }
    byte_count += 1;
    lhs->off += 1;
    rhs->off += 1;
    if (lhs->off == lhs->size) {
      flush_LaceX(lhs);
      read_LaceX(lhs);
    }
    if (rhs->off == rhs->size) {
      flush_LaceX(rhs);
      read_LaceX(rhs);
    }
  }

  equal = equal && (lhs->off == lhs->size && rhs->off == rhs->size);
  if (!equal && out) {
    puts_LaceO(out, "Difference after ");
    print_int_LaceO(out, (int)byte_count);
    puts_LaceO(out, " bytes (");
    if (lhs->off < lhs->size) {
      print_int_LaceO(out, (int)(unsigned)lhs->at[lhs->off]);
    } else {
      puts_LaceO(out, "EOF");
    }
    puts_LaceO(out, " != ");
    if (rhs->off < rhs->size) {
      print_int_LaceO(out, (int)(unsigned)rhs->at[rhs->off]);
    } else {
      puts_LaceO(out, "EOF");
    }
    puts_LaceO(out, ").\n");
  }

  close_LaceX(lhs);
  close_LaceX(rhs);
  close_LaceO(out);
  return (equal ? 0 : 1);
}

#ifndef LACE_BUILTIN_LIBRARY
int main(int argc, char** argv) {
  return lace_builtin_cmp_main((unsigned)argc, argv, NULL, NULL);
}
#endif
