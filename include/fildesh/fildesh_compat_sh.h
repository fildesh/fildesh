#ifndef FILDESH_COMPAT_SH_H_
#define FILDESH_COMPAT_SH_H_

#ifdef _MSC_VER
#include <stdint.h>
typedef intptr_t FildeshCompat_pid;
#else
#include <sys/types.h>
typedef pid_t FildeshCompat_pid;
#endif

char**
fildesh_compat_sh_escape_argv_for_windows(const char* const* argv);
void
fildesh_compat_sh_free_escaped_argv(char** argv);

FildeshCompat_pid
fildesh_compat_sh_spawn(const char* const*);
void
fildesh_compat_sh_exec(const char* const*);
int
fildesh_compat_sh_wait(FildeshCompat_pid);

FildeshCompat_pid
fildesh_compat_fd_spawnvp(int stdin_fd, int stdout_fd, int stderr_fd,
                          const int* fds_to_close, const char* const* argv);
FildeshCompat_pid
fildesh_compat_fd_spawnlp(int stdin_fd, int stdout_fd, int stderr_fd,
                          const int* fds_to_close, const char* cmd, ...);

int
fildesh_compat_sh_chdir(const char*);
int
fildesh_compat_sh_kill(FildeshCompat_pid);
int
fildesh_compat_sh_setenv(const char*, const char*);

#endif
