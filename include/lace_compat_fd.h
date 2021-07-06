#ifndef LACE_COMPAT_FD_H_
#define LACE_COMPAT_FD_H_
#include <stddef.h>

typedef int lace_compat_fd_t;
int
lace_compat_fd_cloexec(int fd);
int
lace_compat_fd_close(lace_compat_fd_t fd);
int
lace_compat_fd_inherit(int fd);
int
lace_compat_fd_move_to(lace_compat_fd_t dst, lace_compat_fd_t oldfd);
lace_compat_fd_t
lace_compat_fd_move_off_stdio(lace_compat_fd_t fd);
lace_compat_fd_t
lace_compat_fd_reserve();
int
lace_compat_fd_pipe(lace_compat_fd_t* ret_produce,
                    lace_compat_fd_t* ret_consume);
size_t
lace_compat_fd_write(lace_compat_fd_t fd, const void* data, size_t data_size);
size_t
lace_compat_fd_read(lace_compat_fd_t fd, void* buf, size_t buf_capacity);

int lace_compat_fd_spawnvp_wait(const int* fds_to_close, const char* const* argv);
int lace_compat_fd_spawnlp_wait(const int* fds_to_close, const char* cmd, ...);

#endif
