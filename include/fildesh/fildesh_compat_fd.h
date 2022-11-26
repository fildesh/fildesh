#ifndef FILDESH_COMPAT_FD_H_
#define FILDESH_COMPAT_FD_H_
#include <stddef.h>

typedef int fildesh_compat_fd_t;
int
fildesh_compat_fd_close(fildesh_compat_fd_t fd);
int
fildesh_compat_fd_move_to(fildesh_compat_fd_t dst, fildesh_compat_fd_t oldfd);
fildesh_compat_fd_t
fildesh_compat_fd_claim(fildesh_compat_fd_t fd);
fildesh_compat_fd_t
fildesh_compat_file_open_readonly(const char*);
fildesh_compat_fd_t
fildesh_compat_file_open_writeonly(const char*);
int
fildesh_compat_fd_pipe(fildesh_compat_fd_t* ret_produce,
                       fildesh_compat_fd_t* ret_consume);
size_t
fildesh_compat_fd_write(fildesh_compat_fd_t fd,
                        const void* data,
                        size_t data_size);
size_t
fildesh_compat_fd_read(fildesh_compat_fd_t fd,
                       void* buf,
                       size_t buf_capacity);

int fildesh_compat_fd_spawnvp_wait(fildesh_compat_fd_t stdin_fd,
                                   fildesh_compat_fd_t stdout_fd,
                                   fildesh_compat_fd_t stderr_fd,
                                   const int* fds_to_close,
                                   const char* const* argv);
int fildesh_compat_fd_spawnlp_wait(fildesh_compat_fd_t stdin_fd,
                                   fildesh_compat_fd_t stdout_fd,
                                   fildesh_compat_fd_t stderr_fd,
                                   const int* fds_to_close,
                                   const char* cmd,
                                   ...);

#endif
