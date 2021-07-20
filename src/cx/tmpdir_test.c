#include "syscx.h"
#include "alphatab.h"

int main(int argc, char** argv) {
  AlphaTab tmppath;
  (void) argc;
  (void) argv;
  init_sysCx();

  tmppath = cons1_AlphaTab("lace");
  tmppath = dflt1_AlphaTab(mktmppath_sysCx("lace"));
  if (!rmdir_sysCx(ccstr_of_AlphaTab(&tmppath))) {
    failout_sysCx("failed to create temporary directory");
  }
  lose_AlphaTab(&tmppath);
  lose_sysCx();
  return 0;
}
