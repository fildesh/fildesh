
#include <fildesh/fildesh.h>

#include <assert.h>
#include <stdio.h>
#include <string.h>

typedef struct MockFildeshXF MockFildeshXF;
struct MockFildeshXF {
  FildeshX x;
  unsigned index;
  size_t chunk_size;
};

static void read_MockFildeshXF(MockFildeshXF* xf);
static void close_MockFildeshXF(MockFildeshXF* xf) {(void) xf;}
static void free_MockFildeshXF(MockFildeshXF* xf) {(void) xf;}
DEFINE_FildeshX_VTable(MockFildeshXF, x);
#define DEFAULT_MockFildeshXF { \
  DEFAULT1_FildeshX(DEFAULT_MockFildeshXF_FildeshX_VTable), \
  0, 0, \
}

  void
read_MockFildeshXF(MockFildeshXF* xf)
{
  FildeshX* in = &xf->x;
  unsigned i;
  for (i = 0; i < xf->chunk_size; ++i) {
    char c = (char) (xf->index & 0xFF);
    xf->index += 1;
    *grow_FildeshX(in, 1) = c;
  }
}

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

static
  void
slicechrs_high_byte_test()
{
  DECLARE_STRLIT_FildeshX(in, "  \xEF\xBB\xBF" "ao()");
  FildeshX slice;
  const char delims[] = " o\t";

  slice = while_chars_FildeshX(in, delims);
  assert(slice.size == 2);
  assert(0 == memcmp(slice.at, "  ", 2));
  assert(peek_byte_FildeshX(in, 0xEF));
  assert((delims[1] | (1<<7)) == 0xEF);

  slice = until_chars_FildeshX(in, "(o)");
  assert(slice.size == 4);
  assert(0 == memcmp(slice.at, "\xEF\xBB\xBF" "a", 4));
  assert(peek_char_FildeshX(in, 'o'));

  slice = while_chars_FildeshX(in, "(o)");
  assert(slice.size == 3);
  assert(0 == memcmp(slice.at, "o()", 3));

  slice = while_chars_FildeshX(in, "(o)");
  assert(slice.size == 0);
  assert(!slice.at);
}

static
  void
param2_test_chunked_slicechrs(unsigned chunk_size, Fildesh_lgsize flush_lgsize)
{
  static const char alphabet[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
  MockFildeshXF xf[1] = { DEFAULT_MockFildeshXF };
  FildeshX* const in = &xf->x;
  FildeshX slice;
  xf->chunk_size = chunk_size;
  xf->x.flush_lgsize = flush_lgsize;

  slice = until_chars_FildeshX(in, "Aa");
  assert(slice.size == (size_t)'A');
  assert(peek_char_FildeshX(in, 'A'));

  slice = while_chars_FildeshX(in, alphabet);
  assert(slice.size == 26);
  assert(peek_char_FildeshX(in, (char)('Z' + 1)));

  slice = until_chars_FildeshX(in, "Aa");
  assert(peek_char_FildeshX(in, 'a'));
  slice = while_chars_FildeshX(in, alphabet);
  assert(peek_char_FildeshX(in, (char)('z' + 1)));

  slice = until_chars_FildeshX(in, "Aa");
  assert(peek_char_FildeshX(in, 'A'));
  slice = while_chars_FildeshX(in, alphabet);
  assert(peek_char_FildeshX(in, (char)('Z' + 1)));

  slice = until_chars_FildeshX(in, "Aa");
  assert(peek_char_FildeshX(in, 'a'));
  slice = while_chars_FildeshX(in, alphabet);
  assert(peek_char_FildeshX(in, (char)('z' + 1)));

  close_FildeshX(in);
}

static
  void
slicechrs_chunked_test()
{
  unsigned chunk_size;
  Fildesh_lgsize flush_lgsize;

  for (chunk_size = 1; chunk_size < 20; ++chunk_size) {
    for (flush_lgsize = 0; flush_lgsize < 4; ++flush_lgsize) {
      fprintf(stderr, "chunk_size:%u  flush_lgsize:%u\n",
              chunk_size, (unsigned)flush_lgsize);
      param2_test_chunked_slicechrs(chunk_size, flush_lgsize);
    }
  }
}

int main() {
  slicechrs_easy_test();
  slicechrs_high_byte_test();
  slicechrs_chunked_test();
  return 0;
}
