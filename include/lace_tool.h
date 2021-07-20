#ifndef LACE_TOOL_H_
#define LACE_TOOL_H_
#include <stddef.h>

#define LACE_TOOL_PIPEM_CALLBACK(name, in_fd, out_fd, T, arg) \
  static void name##_lace_tool_pipem_callback(int in_fd, int out_fd, T arg); \
  static void name(int in_fd, int out_fd, void* voidarg) { \
    name##_lace_tool_pipem_callback(in_fd, out_fd, (T) voidarg); \
  } \
  void name##_lace_tool_pipem_callback(int in_fd, int out_fd, T arg)

#define LACE_TOOL_PIPEM_NULLARY_CALLBACK(name, in_fd, out_fd) \
  static void name##_lace_tool_pipem_callback(int in_fd, int out_fd); \
  static void name(int in_fd, int out_fd, void* voidarg) { \
    (void) voidarg; \
    name##_lace_tool_pipem_callback(in_fd, out_fd); \
  } \
  void name##_lace_tool_pipem_callback(int in_fd, int out_fd)

size_t
lace_tool_pipem(size_t input_size, const char* input_data,
                void (*fn)(int,int,void*), void* arg,
                char** output_storage);
#endif
