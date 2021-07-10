#ifndef LACE_TOOL_H_
#define LACE_TOOL_H_
#include <stddef.h>

#define LACE_TOOL_PIPEM_CALLBACK(name, T, arg) \
  static void name##_lace_tool_pipem_callback(T arg); \
  static void name(void* voidarg) { \
    name##_lace_tool_pipem_callback((T) voidarg); \
  } \
  void name##_lace_tool_pipem_callback(T arg)

#define LACE_TOOL_PIPEM_NULLARY_CALLBACK(name) \
  static void name##_lace_tool_pipem_callback(); \
  static void name(void* voidarg) { \
    (void) voidarg; \
    name##_lace_tool_pipem_callback(); \
  } \
  void name##_lace_tool_pipem_callback()

size_t
lace_tool_pipem(size_t input_size, const char* input_data, int const source_fd,
                void (*fn)(void*), void* arg,
                int const sink_fd, char** output_storage);
#endif
