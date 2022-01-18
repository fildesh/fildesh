/* Include lace.h before this file.*/

#ifndef FILDESH_BUILTIN_H_
#define FILDESH_BUILTIN_H_
/* #include "lace.h" */

int fildesh_builtin_add_main(unsigned, char**, FildeshX**, FildeshO**);
int fildesh_builtin_cmp_main(unsigned, char**, FildeshX**, FildeshO**);
int fildesh_builtin_elastic_pthread_main(unsigned, char**, FildeshX**, FildeshO**);
int fildesh_builtin_execfd_main(unsigned, char**, FildeshX**, FildeshO**);
int fildesh_builtin_fildesh_main(unsigned, char**, FildeshX**, FildeshO**);
int fildesh_builtin_seq_main(unsigned, char**, FildeshX**, FildeshO**);
int fildesh_builtin_sponge_main(unsigned, char**, FildeshX**, FildeshO**);
int fildesh_builtin_time2sec_main(unsigned, char**, FildeshX**, FildeshO**);
int fildesh_builtin_void_main(unsigned, char**, FildeshX**, FildeshO**);
int fildesh_builtin_zec_main(unsigned, char**, FildeshX**, FildeshO**);

#endif
