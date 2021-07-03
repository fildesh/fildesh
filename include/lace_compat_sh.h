#ifndef LACE_COMPAT_SH_H_
#define LACE_COMPAT_SH_H_
#include <stdarg.h>

#ifdef _MSC_VER
#include <stdint.h>
typedef intptr_t lace_compat_pid_t;
#else
#include <sys/types.h>
typedef pid_t lace_compat_pid_t;
#endif

lace_compat_pid_t
lace_compat_sh_spawn(const char* const*);
void
lace_compat_sh_exec(const char* const*);
int
lace_compat_sh_wait(lace_compat_pid_t);

int
lace_compat_sh_chdir(const char*);

#endif
