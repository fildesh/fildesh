#include <fildesh/fildesh.h>
#include <fildesh/sxproto.h>
#include <assert.h>
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

static void test_print(FildeshSxpb* sxpb, const char* name, const char* content_dir) {
  const char* formats[] = {"json", "yaml", "txtpb"};
  unsigned i;
  for (i = 0; i < 3; ++i) {
    FildeshX* f = NULL;
    FildeshO out[1] = {DEFAULT_FildeshO};
    char filepath[1024];
    FildeshX expected;
    FildeshX actual;
    size_t j = 0;
    size_t min_len = 0;

    filepath[0] = '\0';
    strcat(filepath, content_dir);
    strcat(filepath, "/");
    strcat(filepath, name);
    strcat(filepath, ".");
    strcat(filepath, formats[i]);

    f = open_FildeshXF(filepath);
    if (!f) {
      fildesh_log_errorf("Failed to open %s", filepath);
      assert(f);
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
    min_len = expected.size < actual.size ? expected.size : actual.size;
    for (j = 0; j < min_len; ++j) {
      if (expected.at[j] != actual.at[j]) {
        fildesh_log_errorf("Mismatch at index %zu for format %s", j, formats[i]);
        fildesh_log_errorf("Expected: %c, Actual: %c", expected.at[j], actual.at[j]);
        assert(0);
      }
    }
    if (expected.size != actual.size) {
      fildesh_log_errorf("Size mismatch. Expected %zu, Actual %zu for format %s", expected.size, actual.size, formats[i]);
    }
    assert(expected.size == actual.size);

    close_FildeshX(f);
    close_FildeshO(out);
  }
  close_FildeshSxpb(sxpb);
}

int main(int argc, char** argv) {
  const char* content_dir = "test/sxproto/content";
  if (argc > 1) {
    content_dir = argv[1];
  }
  test_print(make_array_test_FildeshSxpb(), "array", content_dir);
  test_print(make_loneof_test_FildeshSxpb(), "loneof", content_dir);
  test_print(make_manyof_test_FildeshSxpb(), "manyof", content_dir);
  test_print(make_message_test_FildeshSxpb(), "message", content_dir);
  test_print(make_string_test_FildeshSxpb(), "string", content_dir);
  return 0;
}
