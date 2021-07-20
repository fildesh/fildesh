#ifndef LACE_COMPAT_SH_H_
#define LACE_COMPAT_SH_H_

#ifdef _MSC_VER
#include <stdint.h>
typedef intptr_t lace_compat_pid_t;
#else
#include <sys/types.h>
typedef pid_t lace_compat_pid_t;
#endif

char**
lace_compat_sh_escape_argv_for_windows(const char* const* argv);
void
lace_compat_sh_free_escaped_argv(char** argv);

lace_compat_pid_t
lace_compat_sh_spawn(const char* const*);
void
lace_compat_sh_exec(const char* const*);
int
lace_compat_sh_wait(lace_compat_pid_t);

lace_compat_pid_t
lace_compat_fd_spawnvp(int stdin_fd, int stdout_fd, int stderr_fd,
                       const int* fds_to_close, const char* const* argv);
lace_compat_pid_t
lace_compat_fd_spawnlp(int stdin_fd, int stdout_fd, int stderr_fd,
                       const int* fds_to_close, const char* cmd, ...);

int
lace_compat_sh_chdir(const char*);

#endif
