#ifndef FILDESH_COMPAT_FD_H_
#define FILDESH_COMPAT_FD_H_
#include <stddef.h>

typedef int FildeshCompat_fd;
/* Deprecated. Use FildeshCompat_fd.*/
typedef FildeshCompat_fd fildesh_compat_fd_t;
int
fildesh_compat_fd_close(FildeshCompat_fd fd);
int
fildesh_compat_fd_move_to(FildeshCompat_fd dst, FildeshCompat_fd oldfd);
FildeshCompat_fd
fildesh_compat_fd_claim(FildeshCompat_fd fd);
FildeshCompat_fd
fildesh_compat_file_open_readonly(const char*);
FildeshCompat_fd
fildesh_compat_file_open_writeonly(const char*);
int
fildesh_compat_fd_pipe(FildeshCompat_fd* ret_produce,
                       FildeshCompat_fd* ret_consume);
size_t
fildesh_compat_fd_write(FildeshCompat_fd fd,
                        const void* data,
                        size_t data_size);
size_t
fildesh_compat_fd_read(FildeshCompat_fd fd,
                       void* buf,
                       size_t buf_capacity);

int fildesh_compat_fd_spawnvp_wait(FildeshCompat_fd stdin_fd,
                                   FildeshCompat_fd stdout_fd,
                                   FildeshCompat_fd stderr_fd,
                                   const int* fds_to_close,
                                   const char* const* argv);
int fildesh_compat_fd_spawnlp_wait(FildeshCompat_fd stdin_fd,
                                   FildeshCompat_fd stdout_fd,
                                   FildeshCompat_fd stderr_fd,
                                   const int* fds_to_close,
                                   const char* cmd,
                                   ...);

#endif
