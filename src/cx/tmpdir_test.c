#include "syscx.h"
#include "alphatab.h"
#include "lace_compat_file.h"

int main() {
  AlphaTab tmppath;
  tmppath = cons1_AlphaTab("lace");
  tmppath = dflt1_AlphaTab(mktmppath_sysCx("lace"));
  if (0 != lace_compat_file_rmdir(ccstr_of_AlphaTab(&tmppath))) {
    assert(0 && "failed to create temporary directory");
  }
  lose_AlphaTab(&tmppath);
  return 0;
}
