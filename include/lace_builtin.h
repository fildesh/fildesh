/* Include lace.h before this file.*/

#ifndef LACE_BUILTIN_H_
#define LACE_BUILTIN_H_
/* #include "lace.h" */

int lace_builtin_add_main(unsigned, char**, LaceX**, LaceO**);
int lace_builtin_cmp_main(unsigned, char**, LaceX**, LaceO**);
int lace_builtin_elastic_pthread_main(unsigned, char**, LaceX**, LaceO**);
int lace_builtin_execfd_main(unsigned, char**, LaceX**, LaceO**);
int lace_builtin_seq_main(unsigned, char**, LaceX**, LaceO**);
int lace_builtin_time2sec_main(unsigned, char**, LaceX**, LaceO**);
int lace_builtin_void_main(unsigned, char**, LaceX**, LaceO**);
int lace_builtin_zec_main(unsigned, char**, LaceX**, LaceO**);

#endif
