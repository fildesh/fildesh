
#include <fildesh/fildesh.h>

#include <assert.h>
#include <string.h>

static
  void
slicechrs_easy_test()
{
  DECLARE_STRLIT_FildeshX(in, "(i.am some|content.)\0h\0i\0.");
  FildeshX slice;
  const char delims[] = " ().|";

  slice = until_chars_FildeshX(in, delims);
  assert(slice.size == 0);
  assert(peek_char_FildeshX(in, '('));

  slice = while_chars_FildeshX(in, delims);
  assert(slice.size == 1);
  assert(0 == memcmp(slice.at, "(", 1));

  assert(peek_char_FildeshX(in, 'i'));
  slice = until_chars_FildeshX(in, delims);
  assert(slice.size == 1);
  assert(0 == memcmp(slice.at, "i", 1));

  slice = while_chars_FildeshX(in, delims);
  assert(slice.size == 1);
  assert(0 == memcmp(slice.at, ".", 1));

  assert(peek_char_FildeshX(in, 'a'));
  slice = until_chars_FildeshX(in, delims);
  assert(slice.size == 2);
  assert(0 == memcmp(slice.at, "am", 2));

  slice = while_chars_FildeshX(in, delims);
  assert(slice.size == 1);
  assert(0 == memcmp(slice.at, " ", 1));

  slice = until_chars_FildeshX(in, delims);
  assert(slice.size == 4);
  assert(0 == memcmp(slice.at, "some", 4));

  slice = while_chars_FildeshX(in, delims);
  assert(slice.size == 1);
  assert(0 == memcmp(slice.at, "|", 1));

  slice = until_chars_FildeshX(in, delims);
  assert(slice.size == 7);
  assert(0 == memcmp(slice.at, "content", 7));

  slice = while_chars_FildeshX(in, delims);
  assert(slice.size == 2);
  assert(0 == memcmp(slice.at, ".)", 2));

  assert(peek_char_FildeshX(in, '\0'));
  assert(peek_byte_FildeshX(in, 0));
  assert(!peek_chars_FildeshX(in, delims));

  slice = until_chars_FildeshX(in, delims);
  assert(slice.size == 5);
  assert(0 == memcmp(slice.at, "\0h\0i\0", 5));

  slice = while_chars_FildeshX(in, delims);
  assert(slice.size == 1);
  assert(0 == memcmp(slice.at, ".", 1));

  slice = until_chars_FildeshX(in, delims);
  assert(slice.size == 0);
  assert(!slice.at);
  slice = while_chars_FildeshX(in, delims);
  assert(slice.size == 0);
  assert(!slice.at);
  assert(!peek_char_FildeshX(in, ')'));
  assert(!peek_char_FildeshX(in, '\0'));
  assert(!peek_byte_FildeshX(in, 0));
  assert(!peek_chars_FildeshX(in, delims));
}

int main() {
  slicechrs_easy_test();
  return 0;
}
