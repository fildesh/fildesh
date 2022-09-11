
#include "fildesh_compat_file.h"
#include <assert.h>

int main() {
  char* d = fildesh_compat_file_mktmpdir("fildeshtmpdirtest");
  int istat;
  assert(d);
  istat = fildesh_compat_file_rmdir(d);
  assert(istat == 0 && "tmpdir removal");
  return 0;
}
