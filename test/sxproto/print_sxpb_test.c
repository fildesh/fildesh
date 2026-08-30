#include <assert.h>
#include <string.h>

#include <fildesh/sxproto.h>

#include "src/sxproto/value.h"
#include "test/sxproto/print_test.h"

static
  FildeshSxpb*
slurp_sxpb_bytes(const char* s, size_t n, FildeshO* err_out)
{
  FildeshX* in = open_FildeshXA();
  memcpy(grow_FildeshX(in, n), s, n);
  return slurp_sxpb_close_FildeshX(in, NULL, err_out);
}

static
  FildeshSxpb*
slurp_sxpb_str(const char* s, FildeshO* err_out)
{
  return slurp_sxpb_bytes(s, strlen(s), err_out);
}

static
  bool
toplevel_manyof_anonymous_int_print_test()
{
  const char expect_content[] = "(())\n(value 1)\n2\n";
  bool passing = true;
  FildeshO* err_out = open_FildeshOF("/dev/stderr");
  FildeshO oslice[1] = {DEFAULT_FildeshO};
  FildeshSxpb* sxpb = open_FildeshSxpb();
  FildeshSxpbIT it = top_of_FildeshSxpb(sxpb);

  (*sxpb->values)[it.cons_id].field_kind = FildeshSxprotoFieldKind_MANYOF;
  it = direct_insert_first_FildeshSxpb(
      sxpb, it,
      ensure_name_FildeshSxpb(sxpb, "+1", 2),
      FildeshSxprotoFieldKind_LITERAL_INT);
  direct_insert_next_FildeshSxpb(
      sxpb, it,
      ensure_name_FildeshSxpb(sxpb, "+2", 2),
      FildeshSxprotoFieldKind_LITERAL_INT);

  print_sxpb_FildeshO(oslice, sxpb);
  passing = check_same_printed_format(
      err_out, "toplevel_manyof_anonymous_int", "sxpb",
      fildesh_bytestrlit(expect_content),
      bytestring_of_FildeshO(oslice));

  close_FildeshSxpb(sxpb);
  close_FildeshO(oslice);
  close_FildeshO(err_out);
  return passing;
}

static
  bool
expect_sxpb_roundtrip(const char* name, const char* expect_sxpb)
{
  bool passing = true;
  FildeshO printed[1] = {DEFAULT_FildeshO};
  FildeshO original_json[1] = {DEFAULT_FildeshO};
  FildeshO reparsed_json[1] = {DEFAULT_FildeshO};
  FildeshO* err_out = open_FildeshOF("/dev/stderr");
  FildeshSxpb* sxpb = slurp_sxpb_str(expect_sxpb, err_out);
  FildeshSxpb* reparsed_sxpb;

  assert(sxpb);
  print_sxpb_FildeshO(printed, sxpb);
  passing = check_same_printed_format(
      err_out, name, "sxpb",
      (const unsigned char*)expect_sxpb, strlen(expect_sxpb),
      bytestring_of_FildeshO(printed))
    && passing;

  reparsed_sxpb = slurp_sxpb_bytes(printed->at, printed->size, err_out);
  assert(reparsed_sxpb);
  print_json_FildeshO(original_json, sxpb);
  print_json_FildeshO(reparsed_json, reparsed_sxpb);
  passing = check_same_printed_format(
      err_out, name, "roundtrip JSON",
      bytestring_of_FildeshO(original_json),
      bytestring_of_FildeshO(reparsed_json))
    && passing;
  close_FildeshSxpb(reparsed_sxpb);

  close_FildeshSxpb(sxpb);
  close_FildeshO(printed);
  close_FildeshO(original_json);
  close_FildeshO(reparsed_json);
  close_FildeshO(err_out);
  return passing;
}

int main(int argc, char** argv) {
  const char* case_filepath = "test/sxproto/content/idempotent_sxpb2sxpb_test_dict.sxpb";
  FildeshO* err_out = open_FildeshOF("/dev/stderr");
  FildeshSxpb* case_sxpb;
  FildeshSxpbIT top_it;
  FildeshSxpbIT it;
  bool passing = true;

  if (argc > 1) {
    case_filepath = argv[1];
  }

  check_same_printed_format_test();
  passing = toplevel_manyof_anonymous_int_print_test() && passing;
  case_sxpb = slurp_sxpb_close_FildeshX(open_FildeshXF(case_filepath), NULL, err_out);
  assert(case_sxpb);

  top_it = top_of_FildeshSxpb(case_sxpb);
  for (it = first_at_FildeshSxpb(case_sxpb, top_it);
       !nullish_FildeshSxpbIT(it);
       it = next_at_FildeshSxpb(case_sxpb, it))
  {
    passing = expect_sxpb_roundtrip(
        name_at_FildeshSxpb(case_sxpb, it),
        str_value_at_FildeshSxpb(case_sxpb, it))
      && passing;
  }

  close_FildeshSxpb(case_sxpb);
  close_FildeshO(err_out);
  return passing ? 0 : 1;
}
