#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <fildesh/sxproto.h>

FildeshSxpb* make_array_test_FildeshSxpb(void);
FildeshSxpb* make_loneof_test_FildeshSxpb(void);
FildeshSxpb* make_manyof_test_FildeshSxpb(void);
FildeshSxpb* make_message_test_FildeshSxpb(void);
FildeshSxpb* make_string_test_FildeshSxpb(void);

static
  int
print_formats_test(FildeshSxpb* sxpb, const char* name, const char* content_dir)
{
  bool passing = true;
  FildeshO oslice[1] = {DEFAULT_FildeshO};
  const char* formats[] = {"json", "yaml", "txtpb"};
  void (*print_fns[])(FildeshO*, FildeshSxpb*) = {
    print_json_FildeshO,
    print_yaml_FildeshO,
    print_txtpb_FildeshO,
  };
  unsigned i;

  for (i = 0; i < 3; ++i) {
    FildeshX actual;
    FildeshX expect_slice;
    FildeshX* in;
    unsigned line_count = 0;

    truncate_FildeshO(oslice);
    putstr_FildeshO(oslice, content_dir);
    putc_FildeshO(oslice, '/');
    putstr_FildeshO(oslice, name);
    putc_FildeshO(oslice, '.');
    putstr_FildeshO(oslice, formats[i]);
    putc_FildeshO(oslice, '\0');

    in = open_FildeshXF(oslice->at);
    if (!in) {
      fildesh_log_errorf("Failed to open %s", oslice->at);
      passing = false;
      continue;
    }

    truncate_FildeshO(oslice);
    print_fns[i](oslice, sxpb);

    actual = getslice_FildeshO(oslice);
    for (expect_slice = sliceline_FildeshX(in);
         expect_slice.at;
         expect_slice = sliceline_FildeshX(in))
    {
      FildeshX actual_slice = sliceline_FildeshX(&actual);
      line_count += 1;
      if (!actual_slice.at ||
          0 != fildesh_compare_bytestring(
              bytestring_of_FildeshX(&expect_slice),
              bytestring_of_FildeshX(&actual_slice)))
      {
        fildesh_log_errorf("Mismatch in %s format %s at line %u", name, formats[i], line_count);
        fildesh_log_errorf("Expected: %.*s", (int)expect_slice.size, expect_slice.at);
        fildesh_log_errorf("Actual  : %.*s", (int)actual_slice.size, actual_slice.at);
        passing = false;
        break;
      }
    }

    close_FildeshX(in);
  }
  close_FildeshO(oslice);
  close_FildeshSxpb(sxpb);
  return passing;
}

int main(int argc, char** argv) {
  const char* content_dirpath = "test/sxproto/content";
  bool passing = true;
  if (argc > 1) {
    content_dirpath = argv[1];
  }

  passing = print_formats_test(make_array_test_FildeshSxpb(), "array", content_dirpath) && passing;
  passing = print_formats_test(make_loneof_test_FildeshSxpb(), "loneof", content_dirpath) && passing;
  passing = print_formats_test(make_manyof_test_FildeshSxpb(), "manyof", content_dirpath) && passing;
  passing = print_formats_test(make_message_test_FildeshSxpb(), "message", content_dirpath) && passing;
  passing = print_formats_test(make_string_test_FildeshSxpb(), "string", content_dirpath) && passing;

  return passing ? 0 : 1;
}
