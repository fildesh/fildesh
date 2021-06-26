#include "lace_compat_fd.h"
#include "lace_posix_thread.h"
#include "lace_tool.h"

#include <assert.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>


static void inherit_fd(int fd) {
  int istat = lace_compat_fd_inherit(fd);
  assert(0 == istat);
}

static void create_pipe(int* ret_produce, int* ret_consume) {
  int istat = lace_compat_fd_pipe(ret_produce, ret_consume);
  assert(0 == istat);
}

static int duplicate_fd(int fd) {
  int newfd = lace_compat_fd_duplicate(fd);
  assert(newfd >= 0);
  return newfd;
}

static void move_fd_to(int dst, int oldfd) {
  int istat = lace_compat_fd_move_to(dst, oldfd);
  assert(istat == 0);
}

typedef struct LaceToolPipemInput LaceToolPipemInput;
typedef struct LaceToolPipemOutput LaceToolPipemOutput;
struct LaceToolPipemInput {
  const char* input_data;
  size_t input_size;
  int produce_fd;
};
struct LaceToolPipemOutput {
  char** output_storage;
  size_t output_size;
  int consume_fd;
};

static void* produce_thread_fn(LaceToolPipemInput* arg) {
  size_t i = 0;
  while (i < arg->input_size) {
    size_t n = lace_compat_fd_write(
        arg->produce_fd, &arg->input_data[i], arg->input_size - i);
    if (n == 0) {
      break;
    }
    i += n;
  }
  lace_compat_fd_close(arg->produce_fd);
  return NULL;
}

static void* consume_thread_fn(LaceToolPipemOutput* arg) {
  size_t capacity = 0;
  while (1) {
    size_t total;
    char buf[1024];
    size_t n = lace_compat_fd_read(arg->consume_fd, buf, sizeof(buf));
    if (n == 0) {
      break;
    }
    total = n + arg->output_size;
    if (arg->output_storage) {
      const size_t total = n + arg->output_size;
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
  lace_compat_fd_close(arg->consume_fd);
  return NULL;
}

  size_t
lace_tool_pipem(size_t input_size, const char* input_data, int const source_fd,
                void (*fn)(void*), void* arg,
                int const sink_fd, char** output_storage)
{
  int istat;
  int saved_source_fd = -1;
  int saved_sink_fd = -1;
  int produce_fd = -1;
  pthread_t produce_thread;
  LaceToolPipemInput produce_thread_arg;
  int consume_fd = -1;
  pthread_t consume_thread;
  LaceToolPipemOutput consume_thread_arg;
#ifndef _WIN32
  void (*sigpipe_fn)(int) = signal(SIGPIPE, SIG_IGN);
#endif

  assert(input_data || input_size == 0);
  assert(source_fd >= 0 || (!input_data && input_size == 0));
  assert(fn);
  assert(sink_fd >= 0 || !output_storage);

  if (source_fd >= 0) {
    int tmp_source_fd = -1;
    create_pipe(&produce_fd, &tmp_source_fd);
    saved_source_fd = duplicate_fd(source_fd);
    move_fd_to(source_fd, tmp_source_fd);
    inherit_fd(source_fd);
  }

  if (sink_fd >= 0) {
    int tmp_sink_fd = -1;
    create_pipe(&tmp_sink_fd, &consume_fd);
    saved_sink_fd = duplicate_fd(sink_fd);
    move_fd_to(sink_fd, tmp_sink_fd);
    inherit_fd(sink_fd);
  }

  produce_thread_arg.input_data = input_data;
  produce_thread_arg.input_size = input_size;
  produce_thread_arg.produce_fd = produce_fd;

  consume_thread_arg.output_storage = output_storage;
  consume_thread_arg.output_size = 0;
  consume_thread_arg.consume_fd = consume_fd;

  if (produce_fd >= 0) {
    istat = pthread_create(
        &produce_thread, NULL,
        (void* (*) (void*)) produce_thread_fn,
        &produce_thread_arg);
    assert(istat == 0);
  }
  if (consume_fd >= 0) {
    istat = pthread_create(
        &consume_thread, NULL,
        (void* (*) (void*)) consume_thread_fn,
        &consume_thread_arg);
    assert(istat == 0);
  }

  fn(arg);

  if (produce_fd >= 0) {
    istat = pthread_join(produce_thread, NULL);
    assert(istat == 0);
    move_fd_to(source_fd, saved_source_fd);
  }
  if (consume_fd >= 0) {
    istat = pthread_join(consume_thread, NULL);
    assert(istat == 0);
    move_fd_to(sink_fd, saved_sink_fd);
  }
#ifndef _WIN32
  signal(SIGPIPE, sigpipe_fn);
#endif
  return consume_thread_arg.output_size;
}

