#include <fildesh/fildesh.h>
#include "include/fildesh/fildesh_compat_file.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

int
main_godo(unsigned argc, char** argv);

static void run_godo(const char* expectish_exe, char* filepath) {
  int istat;
  char* exe = NULL;
  char* base = (char*)fildesh_compat_file_basename(filepath);
  const char* directory = ".";
  const char* argv[6] = {
    "godo", /* Dummy exe name.*/
    "replace_with_directory",
    "replace_with_expectish_exe",
    "replace_with_base",
    "hello", /* Expect "hello" in the file.*/
    NULL,
  };

  exe = fildesh_compat_file_abspath(expectish_exe);
  assert(exe);

  if (base != filepath) {
    base[-1] = '\0';
    directory = filepath;
  }
  argv[1] = directory;
  argv[2] = exe;
  argv[3] = base;
  {
    unsigned i;
    for (i = 0; i < 5; ++i) {
      fprintf(stderr, "%s\n", argv[i]);
    }
  }
  istat = main_godo(5, (char**)argv);
  assert(istat == 0);
  free(exe);
}

int main(int argc, char** argv) {
  assert(argc == 3);
  run_godo(argv[1], argv[2]);
  return 0;
}
