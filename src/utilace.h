
#include "cx/syscx.h"
#include "cx/alphatab.h"

#ifndef MAIN_LACE_EXECUTABLE
#define LaceUtilMain(name) \
  int main_##name (int argi, int argc, char** argv); \
  int main (int argc, char** argv) \
  { int argi = init_sysCx (&argc, &argv); return main_##name (argi, argc, argv); } \
  int main_##name (int argi, int argc, char** argv)

qual_inline
int (*lace_specific_util (const char* arg)) (int, int, char**)
{
  (void) arg;
  return 0;
}
qual_inline
int lace_util_main (int argi, int argc, char** argv)
{
  (void) argi;
  (void) argc;
  (void) argv;
  return -1;
}
#else

#define LaceUtilMain(name) \
  int main_##name (int argi, int argc, char** argv)

int (*lace_specific_util (const char* arg)) (int, int, char**);
int lace_util_main (int argi, int argc, char** argv);
#endif

