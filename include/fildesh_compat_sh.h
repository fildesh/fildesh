#ifndef FILDESH_COMPAT_SH_H_
#define FILDESH_COMPAT_SH_H_

#ifdef _MSC_VER
#include <stdint.h>
typedef intptr_t fildesh_compat_pid_t;
#else
#include <sys/types.h>
typedef pid_t fildesh_compat_pid_t;
#endif

char**
fildesh_compat_sh_escape_argv_for_windows(const char* const* argv);
void
fildesh_compat_sh_free_escaped_argv(char** argv);

fildesh_compat_pid_t
fildesh_compat_sh_spawn(const char* const*);
void
fildesh_compat_sh_exec(const char* const*);
int
fildesh_compat_sh_wait(fildesh_compat_pid_t);

fildesh_compat_pid_t
fildesh_compat_fd_spawnvp(int stdin_fd, int stdout_fd, int stderr_fd,
                          const int* fds_to_close, const char* const* argv);
fildesh_compat_pid_t
fildesh_compat_fd_spawnlp(int stdin_fd, int stdout_fd, int stderr_fd,
                          const int* fds_to_close, const char* cmd, ...);

int
fildesh_compat_sh_chdir(const char*);

#endif
