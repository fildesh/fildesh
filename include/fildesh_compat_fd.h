#ifndef LACE_COMPAT_FD_H_
#define LACE_COMPAT_FD_H_
#include <stddef.h>

typedef int lace_compat_fd_t;
int
lace_compat_fd_close(lace_compat_fd_t fd);
int
lace_compat_fd_move_to(lace_compat_fd_t dst, lace_compat_fd_t oldfd);
lace_compat_fd_t
lace_compat_fd_claim(lace_compat_fd_t fd);
lace_compat_fd_t
lace_compat_file_open_readonly(const char*);
lace_compat_fd_t
lace_compat_file_open_writeonly(const char*);
int
lace_compat_fd_pipe(lace_compat_fd_t* ret_produce,
                    lace_compat_fd_t* ret_consume);
size_t
lace_compat_fd_write(lace_compat_fd_t fd, const void* data, size_t data_size);
size_t
lace_compat_fd_read(lace_compat_fd_t fd, void* buf, size_t buf_capacity);

int lace_compat_fd_spawnvp_wait(lace_compat_fd_t stdin_fd,
                                lace_compat_fd_t stdout_fd,
                                lace_compat_fd_t stderr_fd,
                                const int* fds_to_close,
                                const char* const* argv);
int lace_compat_fd_spawnlp_wait(lace_compat_fd_t stdin_fd,
                                lace_compat_fd_t stdout_fd,
                                lace_compat_fd_t stderr_fd,
                                const int* fds_to_close,
                                const char* cmd,
                                ...);

#endif
