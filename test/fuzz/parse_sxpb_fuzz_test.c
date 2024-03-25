#include "fuzz_common.h"

#include <assert.h>
#include <string.h>

#include <fildesh/sxproto.h>

#ifdef SMOKE_TEST_ON
static
  const uint8_t*
print_expected_error_message(FildeshO* oss, const uint8_t data[], size_t* size)
{
  FildeshX in[1];
  FildeshX slice;
  *in = FildeshX_of_bytestring(data, *size);

  slice = sliceline_FildeshX(in);
  assert(avail_FildeshX(in));
  data = &data[in->off];
  *size -= in->off;

  *in = slice;
  if (in->size > 0) {
    int x = 0;
    putstrlit_FildeshO(oss, "Line ");
    assert(parse_int_FildeshX(in, &x));
    assert(skipstr_FildeshX(in, ","));
    print_int_FildeshO(oss, x);
    putstrlit_FildeshO(oss, " column ");
    assert(parse_int_FildeshX(in, &x));
    assert(skipstr_FildeshX(in, ","));
    print_int_FildeshO(oss, x);
    putstrlit_FildeshO(oss, ": ");
    put_bytestring_FildeshO(oss, bytestring_of_FildeshX(in));
    putc_FildeshO(oss, '\n');
  }
  return data;
}
#endif

  int
LLVMFuzzerTestOneInput(const uint8_t data[], size_t size) {
#ifdef SMOKE_TEST_ON
  FildeshO err_out[1] = {DEFAULT_FildeshX};
  FildeshO out[1] = {DEFAULT_FildeshX};
  FildeshO expect_errmsg[1] = {DEFAULT_FildeshO};
  data = print_expected_error_message(expect_errmsg, data, &size);
#else
  FildeshO* err_out = open_FildeshOF("/dev/null");
  FildeshO* out = open_FildeshOF("/dev/null");
#endif

  if (size > 0 && data[size-1] == 0) {
    /* Trim a NUL byte if it exists.*/
    size -= 1;
  }

  {
    FildeshX in = FildeshX_of_bytestring(data, size);
    FildeshSxpb* sxpb = slurp_sxpb_close_FildeshX(&in, NULL, err_out);
    if (sxpb) {
      print_txtpb_FildeshO(out, sxpb);
      close_FildeshSxpb(sxpb);
    }
  }

#ifdef SMOKE_TEST_ON
  if (expect_errmsg->size == 0) {
    assert(out->size > 0);
    assert(err_out->size == 0);
  }
  else {
    FildeshO* stderr_out = open_FildeshOF("/dev/stderr");
    put_bytestring_FildeshO(stderr_out, bytestring_of_FildeshO(err_out));
    close_FildeshO(stderr_out);
    assert(out->size == 0);
    assert(err_out->size == expect_errmsg->size);
    assert(0 == memcmp(err_out->at, bytestring_of_FildeshO(expect_errmsg)));
  }
  close_FildeshO(expect_errmsg);
#endif
  close_FildeshO(out);
  close_FildeshO(err_out);
  return 0;
}

