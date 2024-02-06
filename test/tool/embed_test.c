#include <assert.h>
#include <stddef.h>
#include <string.h>

static void cembed_test() {
#include "test/tool/embed_test.cembed.h"
  assert(nfiles == 2);
  assert(files_bytes[0][files_nbytes[0]] == '\0');
  assert(files_bytes[1][files_nbytes[1]] == '\0');
  assert(0 == memcmp("127", files_bytes[0], 3));
  assert(0 == memcmp("hello", files_bytes[1], 5));
}

static int item_lemma() {return 10;}
static int item_lemon() {return 11;}
static int item_lima() {return 12;}
static int item_llama() {return 13;}
static int the_item(const char* key) {
  int (*fn)() = NULL;
#include "test/tool/embed_test.cswitch.h"
  if (fn) {return fn();}
  return 0;
}
static void cswitch_test() {
  assert(the_item("lemma") == 10);
  assert(the_item("lemon") == 11);
  assert(the_item("lima") == 12);
  assert(the_item("llama") == 13);
  assert(the_item("llamaa") == 0);
  assert(the_item("") == 0);
}

int main() {
  cembed_test();
  cswitch_test();
  return 0;
}
