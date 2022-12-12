#include "include/fildesh/fildesh_compat_fd.h"
#include "fildesh_posix_thread.h"
#include "fildesh_tool.h"

#include <assert.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>


typedef struct FildeshToolPipemInput FildeshToolPipemInput;
typedef struct FildeshToolPipemOutput FildeshToolPipemOutput;
struct FildeshToolPipemInput {
  const char* input_data;
  size_t input_size;
  int produce_fd;
};
struct FildeshToolPipemOutput {
  char** output_storage;
  size_t output_size;
  int consume_fd;
};

FILDESH_POSIX_THREAD_CALLBACK(produce_thread_fn, FildeshToolPipemInput*, arg)
{
  size_t i = 0;
  while (i < arg->input_size) {
    size_t n = fildesh_compat_fd_write(
        arg->produce_fd, &arg->input_data[i], arg->input_size - i);
    if (n == 0) {
      break;
    }
    i += n;
  }
  fildesh_compat_fd_close(arg->produce_fd);
}

FILDESH_POSIX_THREAD_CALLBACK(consume_thread_fn, FildeshToolPipemOutput*, arg)
{
  size_t capacity = 0;
  arg->output_size = 0;
  while (1) {
    size_t total;
    char buf[1024];
    size_t n = fildesh_compat_fd_read(arg->consume_fd, buf, sizeof(buf));
    if (n == 0) {
      break;
    }
    total = n + arg->output_size;
    if (arg->output_storage) {
      if (total > capacity) {
        capacity *= 2;
        if (total > capacity) {
          capacity = total;
        }
        *arg->output_storage = (char*) realloc(*arg->output_storage, capacity);
      }
      memcpy(&(*arg->output_storage)[arg->output_size], buf, n);
    }
    arg->output_size = total;
  }
  if (arg->output_storage) {
    *arg->output_storage = (char*)
      realloc(*arg->output_storage, arg->output_size+1);
    /* Add a NUL just in case someone wants to print it.*/
    (*arg->output_storage)[arg->output_size] = '\0';
  }
  fildesh_compat_fd_close(arg->consume_fd);
}

  size_t
fildesh_tool_pipem(
    size_t input_size, const char* input_data,
    void (*fn)(fildesh_compat_fd_t,fildesh_compat_fd_t,void*), void* arg,
    char** output_storage)
{
  int istat;
  fildesh_compat_fd_t source_fd = -1;
  fildesh_compat_fd_t produce_fd = -1;
  pthread_t produce_thread;
  FildeshToolPipemInput produce_thread_arg;
  fildesh_compat_fd_t sink_fd = -1;
  fildesh_compat_fd_t consume_fd = -1;
  pthread_t consume_thread;
  FildeshToolPipemOutput consume_thread_arg;
#ifndef _MSC_VER
  void (*sigpipe_fn)(int) = signal(SIGPIPE, SIG_IGN);
#endif

  assert(input_data || input_size == 0);
  assert(fn);

  if (input_size > 0) {
    istat = fildesh_compat_fd_pipe(&produce_fd, &source_fd);
    assert(istat == 0);
  }
  if (output_storage) {
    istat = fildesh_compat_fd_pipe(&sink_fd, &consume_fd);
    assert(istat == 0);
  }

  assert(source_fd >= 0 || (!input_data && input_size == 0));
  assert(sink_fd >= 0 || !output_storage);

  produce_thread_arg.input_data = input_data;
  produce_thread_arg.input_size = input_size;
  produce_thread_arg.produce_fd = produce_fd;

  consume_thread_arg.output_storage = output_storage;
  consume_thread_arg.output_size = 0;
  consume_thread_arg.consume_fd = consume_fd;

  if (produce_fd >= 0) {
    istat = pthread_create(
        &produce_thread, NULL, produce_thread_fn, &produce_thread_arg);
    assert(istat == 0);
  }
  if (consume_fd >= 0) {
    istat = pthread_create(
        &consume_thread, NULL, consume_thread_fn, &consume_thread_arg);
    assert(istat == 0);
  }

  fn(source_fd, sink_fd, arg);

  if (produce_fd >= 0) {
    istat = pthread_join(produce_thread, NULL);
    assert(istat == 0);
  }
  if (consume_fd >= 0) {
    istat = pthread_join(consume_thread, NULL);
    assert(istat == 0);
  }
#ifndef _MSC_VER
  signal(SIGPIPE, sigpipe_fn);
#endif
  return consume_thread_arg.output_size;
}

