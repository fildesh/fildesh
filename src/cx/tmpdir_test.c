#include "syscx.h"
#include "alphatab.h"

int main() {
  AlphaTab tmppath;
  tmppath = cons1_AlphaTab("lace");
  tmppath = dflt1_AlphaTab(mktmppath_sysCx("lace"));
  if (!rmdir_sysCx(ccstr_of_AlphaTab(&tmppath))) {
    assert(0 && "failed to create temporary directory");
  }
  lose_AlphaTab(&tmppath);
  return 0;
}
