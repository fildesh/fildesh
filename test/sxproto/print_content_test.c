#include <fildesh/fildesh.h>
#include <fildesh/sxproto.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

FildeshSxpb* make_array_test_FildeshSxpb(void);
FildeshSxpb* make_loneof_test_FildeshSxpb(void);
FildeshSxpb* make_manyof_test_FildeshSxpb(void);
FildeshSxpb* make_message_test_FildeshSxpb(void);
FildeshSxpb* make_string_test_FildeshSxpb(void);

static void slurp_file(FildeshX* f) {
  while (read_FildeshX(f)) {}
}

static int test_print(FildeshSxpb* sxpb, const char* name, const char* content_dir) {
  const char* formats[] = {"json", "yaml", "txtpb"};
  unsigned i;
  int all_passed = 1;

  for (i = 0; i < 3; ++i) {
    FildeshX* f = NULL;
    FildeshO out[1] = {DEFAULT_FildeshO};
    char filepath[1024];
    FildeshX expected;
    FildeshX actual;
    unsigned line_num = 1;

    filepath[0] = '\0';
    strcat(filepath, content_dir);
    strcat(filepath, "/");
    strcat(filepath, name);
    strcat(filepath, ".");
    strcat(filepath, formats[i]);

    f = open_FildeshXF(filepath);
    if (!f) {
      fildesh_log_errorf("Failed to open %s", filepath);
      all_passed = 0;
      continue;
    }
    slurp_file(f);

    if (strcmp(formats[i], "json") == 0) {
      print_json_FildeshO(out, sxpb);
    } else if (strcmp(formats[i], "yaml") == 0) {
      print_yaml_FildeshO(out, sxpb);
    } else if (strcmp(formats[i], "txtpb") == 0) {
      print_txtpb_FildeshO(out, sxpb);
    }

    expected = *f;
    actual = getslice_FildeshO(out);

    while (avail_FildeshX(&expected) || avail_FildeshX(&actual)) {
      FildeshX expected_line = sliceline_FildeshX(&expected);
      FildeshX actual_line = sliceline_FildeshX(&actual);

      if (expected_line.size != actual_line.size || memcmp(expected_line.at, actual_line.at, expected_line.size) != 0) {
        fildesh_log_errorf("Mismatch in %s format %s at line %u", name, formats[i], line_num);
        fildesh_log_errorf("Expected: %.*s", (int)expected_line.size, expected_line.at);
        fildesh_log_errorf("Actual  : %.*s", (int)actual_line.size, actual_line.at);
        all_passed = 0;
        break; /* show first error of this format, then continue to next format */
      }
      line_num++;
    }

    close_FildeshX(f);
    close_FildeshO(out);
  }
  close_FildeshSxpb(sxpb);
  return all_passed;
}

int main(int argc, char** argv) {
  const char* content_dir = "test/sxproto/content";
  int passed = 1;
  if (argc > 1) {
    content_dir = argv[1];
  }

  if (!test_print(make_array_test_FildeshSxpb(), "array", content_dir)) passed = 0;
  if (!test_print(make_loneof_test_FildeshSxpb(), "loneof", content_dir)) passed = 0;
  if (!test_print(make_manyof_test_FildeshSxpb(), "manyof", content_dir)) passed = 0;
  if (!test_print(make_message_test_FildeshSxpb(), "message", content_dir)) passed = 0;
  if (!test_print(make_string_test_FildeshSxpb(), "string", content_dir)) passed = 0;

  return passed ? 0 : 1;
}
