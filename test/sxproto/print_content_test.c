#include <assert.h>
#include <string.h>

#include <fildesh/sxproto.h>

#include "test/sxproto/print_test.h"

FildeshSxpb* make_array_test_FildeshSxpb(void);
FildeshSxpb* make_dict_test_FildeshSxpb(void);
FildeshSxpb* make_loneof_test_FildeshSxpb(void);
FildeshSxpb* make_manyof_test_FildeshSxpb(void);
FildeshSxpb* make_message_test_FildeshSxpb(void);
FildeshSxpb* make_nest_test_FildeshSxpb(void);
FildeshSxpb* make_string_test_FildeshSxpb(void);

static
  bool
print_sxpb_roundtrip_test(FildeshSxpb* sxpb, const char* name)
{
  FildeshO original[1] = {DEFAULT_FildeshO};
  FildeshO printed[1] = {DEFAULT_FildeshO};
  FildeshO reparsed[1] = {DEFAULT_FildeshO};
  FildeshO* err_out = open_FildeshOF("/dev/stderr");
  FildeshX* in = open_FildeshXA();
  FildeshSxpb* roundtrip_sxpb;
  bool passing;

  print_json_FildeshO(original, sxpb);
  print_sxpb_FildeshO(printed, sxpb);
  memcpy(grow_FildeshX(in, printed->size), printed->at, printed->size);
  roundtrip_sxpb = slurp_sxpb_close_FildeshX(in, NULL, err_out);
  assert(roundtrip_sxpb);
  print_json_FildeshO(reparsed, roundtrip_sxpb);
  passing = check_same_printed_format(
      err_out, name, "roundtrip JSON",
      bytestring_of_FildeshO(original),
      bytestring_of_FildeshO(reparsed));
  close_FildeshSxpb(roundtrip_sxpb);

  close_FildeshO(original);
  close_FildeshO(printed);
  close_FildeshO(reparsed);
  close_FildeshO(err_out);
  return passing;
}

static
  bool
print_formats_test(FildeshSxpb* sxpb, const char* name, const char* content_dir)
{
  bool passing = true;
  FildeshO oslice[1] = {DEFAULT_FildeshO};
  FildeshO* err_out = open_FildeshOF("/dev/stderr");
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

    truncate_FildeshO(oslice);
    putstr_FildeshO(oslice, content_dir);
    putc_FildeshO(oslice, '/');
    putstr_FildeshO(oslice, name);
    putc_FildeshO(oslice, '.');
    putstr_FildeshO(oslice, formats[i]);
    putc_FildeshO(oslice, '\0');

    in = open_FildeshXF(oslice->at);
    assert(in);
    truncate_FildeshO(oslice);
    print_fns[i](oslice, sxpb);
    actual = getslice_FildeshO(oslice);
    for (expect_slice = sliceline_FildeshX(in);
         expect_slice.at;
         expect_slice = sliceline_FildeshX(in))
    {
      FildeshX actual_slice = sliceline_FildeshX(&actual);
      assert(actual_slice.at);
      passing = check_same_printed_format(
          err_out, name, formats[i],
          bytestring_of_FildeshX(&expect_slice),
          bytestring_of_FildeshX(&actual_slice))
        && passing;
    }
    expect_slice = sliceline_FildeshX(&actual);
    assert(!expect_slice.at);
    close_FildeshX(in);
  }
  passing = print_sxpb_roundtrip_test(sxpb, name) && passing;
  close_FildeshO(oslice);
  close_FildeshO(err_out);
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
  passing = print_formats_test(make_dict_test_FildeshSxpb(), "dict", content_dirpath) && passing;
  passing = print_formats_test(make_loneof_test_FildeshSxpb(), "loneof", content_dirpath) && passing;
  passing = print_formats_test(make_manyof_test_FildeshSxpb(), "manyof", content_dirpath) && passing;
  passing = print_formats_test(make_message_test_FildeshSxpb(), "message", content_dirpath) && passing;
  passing = print_formats_test(make_nest_test_FildeshSxpb(), "nest", content_dirpath) && passing;
  passing = print_formats_test(make_string_test_FildeshSxpb(), "string", content_dirpath) && passing;

  return passing ? 0 : 1;
}
