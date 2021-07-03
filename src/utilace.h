
#include "lace.h"

#ifdef LACE_BUILTIN_LIBRARY
int (*lace_specific_util (const char* arg)) (unsigned, char**);
int lace_builtin_main(unsigned argc, char** argv);
#else
static inline
int (*lace_specific_util (const char* arg)) (unsigned, char**)
{
  (void) arg;
  return 0;
}
static inline
int lace_builtin_main(unsigned argc, char** argv)
{
  (void) argc;
  (void) argv;
  return -1;
}
#endif

