#ifndef LACE_TOOL_H_
#define LACE_TOOL_H_
#include <stdarg.h>
#include <stddef.h>
int
lace_tool_expectish_main(int argc, char** argv);
int
lace_tool_shout_main(int argc, char** argv);
int
lace_tool_spawn_main(int argc, char** argv);

size_t
lace_tool_pipem(size_t input_size, const char* input_data, int const source_fd,
                void (*fn)(void*), void* arg,
                int const sink_fd, char** output_storage);
int
lace_tool_spawn_close_expect(char** argv, const int* fdv, int expect);
int
lace_tool_vspawnlp(const int* fds_to_close, const char* cmd, va_list argp);
int
lace_tool_spawnlp(const int* fds_to_close, const char* cmd, ...);
#endif
