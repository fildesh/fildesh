
#include "fildesh.h"

#ifdef FILDESH_BUILTIN_LIBRARY
int (*fildesh_specific_util (const char* arg)) (unsigned, char**);
int fildesh_builtin_main(const char* name, unsigned argc, char** argv);
#else
static inline
int (*fildesh_specific_util (const char* arg)) (unsigned, char**)
{
  (void) arg;
  return 0;
}
static inline
int fildesh_builtin_main(const char* name, unsigned argc, char** argv)
{
  (void) name;
  (void) argc;
  (void) argv;
  return -1;
}
#endif

