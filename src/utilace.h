
#include "cx/syscx.h"
#include "cx/alphatab.h"

#ifdef MAIN_LACE_EXECUTABLE
int (*lace_specific_util (const char* arg)) (int, int, char**);
int lace_util_main (int argi, int argc, char** argv);
#else
static inline
int (*lace_specific_util (const char* arg)) (int, int, char**)
{
  (void) arg;
  return 0;
}
static inline
int lace_util_main (int argi, int argc, char** argv)
{
  (void) argi;
  (void) argc;
  (void) argv;
  return -1;
}
#endif

