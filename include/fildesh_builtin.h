#ifndef FILDESH_BUILTIN_H_
#define FILDESH_BUILTIN_H_

#ifndef FILDESH_H_
#include "fildesh.h"
#endif

int fildesh_builtin_add_main(unsigned, char**, FildeshX**, FildeshO**);
int fildesh_builtin_cmp_main(unsigned, char**, FildeshX**, FildeshO**);
int fildesh_builtin_elastic_pthread_main(unsigned, char**, FildeshX**, FildeshO**);
int fildesh_builtin_execfd_main(unsigned, char**, FildeshX**, FildeshO**);
int fildesh_builtin_expect_failure_main(unsigned, char**, FildeshX**, FildeshO**);
int fildesh_builtin_fildesh_main(unsigned, char**, FildeshX**, FildeshO**);
int fildesh_builtin_seq_main(unsigned, char**, FildeshX**, FildeshO**);
int fildesh_builtin_sponge_main(unsigned, char**, FildeshX**, FildeshO**);
int fildesh_builtin_time2sec_main(unsigned, char**, FildeshX**, FildeshO**);
int fildesh_builtin_transpose_main(unsigned, char**, FildeshX**, FildeshO**);
int fildesh_builtin_ujoin_main(unsigned, char**, FildeshX**, FildeshO**);
int fildesh_builtin_void_main(unsigned, char**, FildeshX**, FildeshO**);
int fildesh_builtin_zec_main(unsigned, char**, FildeshX**, FildeshO**);

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

#endif
