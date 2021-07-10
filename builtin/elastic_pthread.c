/**
 * \file elastic_pthread.c
 * Echo stdin to stdout with an arbitrary sized buffer.
 **/
#include "lace.h"
#include "lace_posix_thread.h"

#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


typedef struct WritingThreadState WritingThreadState;

struct WritingThreadState
{
  bool done;
  pthread_t thread;
  pthread_cond_t buf_cond;
  pthread_mutex_t buf_mutex;
  LaceO buf;
  LaceO* outfile;
  const char* filename;
};

static
  void
init_WritingThreadState(WritingThreadState* st)
{
  st->done = false;
  pthread_cond_init(&st->buf_cond, NULL);
  pthread_mutex_init(&st->buf_mutex, NULL);
  st->buf = default_LaceO();
  st->outfile = NULL;
  st->filename = NULL;
}

/** Call pthread_join() before this!**/
static
  void
lose_WritingThreadState(WritingThreadState* st)
{
  pthread_cond_destroy(&st->buf_cond);
  pthread_mutex_destroy(&st->buf_mutex);
  close_LaceO(&st->buf);
  close_LaceO(st->outfile);
}

static
  void
StateMsg(const char* msg, const char* name) {
#if 0
  static bool initialized = false;
  static pthread_mutex_t mutex;
  /* You better call this from a single thread.*/
  if (!initialized) {
    pthread_mutex_init(&mutex, NULL);
    initialized = true;
  }
  pthread_mutex_lock(&mutex);
  fprintf(stderr, "%s %s\n", msg, name);
  pthread_mutex_unlock(&mutex);
#else
  (void)msg;
  (void)name;
#endif
}

LACE_POSIX_THREAD_CALLBACK(writing_thread_fn, WritingThreadState*, st)
{
  bool done = false;

  while (!done) {
    pthread_mutex_lock(&st->buf_mutex);
    StateMsg("loop start", st->filename);
    StateMsg("waiting", st->filename);
    while (st->buf.size == 0 && !st->done) {
      pthread_cond_wait(&st->buf_cond, &st->buf_mutex);
    }
    StateMsg("done waiting", st->filename);
    done = st->done;
    memcpy(grow_LaceO(st->outfile, st->buf.size),
           &st->buf.at[0], st->buf.size);
    st->buf.size = 0;
    pthread_mutex_unlock(&st->buf_mutex);

    flush_LaceO(st->outfile);
    if (st->outfile->size > 0) {
      break;
    }
  }
  StateMsg("done writing", st->filename);

  if (!done) {
    StateMsg("setting myself done", st->filename);
    /* The file closed on us!*/
    pthread_mutex_lock(&st->buf_mutex);
    st->done = true;
    pthread_mutex_unlock(&st->buf_mutex);
  }

  StateMsg("end of", st->filename);
  close_LaceO(&st->buf);
  close_LaceO(st->outfile);
  st->outfile = NULL;
}

/** Reading is actually done on the main thread.**/
static
  void
reading_routine(LaceX* in, WritingThreadState* wstates, size_t wstate_count)
{
  int istat;
  unsigned i;
  size_t nspawned = wstate_count;  /* For early cleanup.*/

  for (i = 0; i < wstate_count; ++i) {
    WritingThreadState* st = &wstates[i];
    istat = pthread_create(&st->thread, NULL, writing_thread_fn, st);
    if (istat != 0) {
      StateMsg("error on pthread create", "");
      nspawned = i;
      break;
    }
  }

  if (nspawned == wstate_count) {
    while (true) {
      const size_t nbytes = read_LaceX(in);
      if (nbytes == 0) {
        break;
      }

      for (i = 0; i < wstate_count; ++i) {
        WritingThreadState* st = &wstates[i];
        pthread_mutex_lock(&st->buf_mutex);
        if (!st->done) {
          StateMsg("catting", st->filename);
          memcpy(grow_LaceO(&st->buf, nbytes),
                 &in->at[in->off],
                 nbytes);
          pthread_cond_signal(&st->buf_cond);
        }
        pthread_mutex_unlock(&st->buf_mutex);
      }
      in->off += nbytes;
      assert(in->off == in->size);
      maybe_flush_LaceX(in);
    }
  }
  close_LaceX(in);
  StateMsg("done reading", "input");

  for (i = 0; i < nspawned; ++i) {
    void* ret = NULL;
    WritingThreadState* st = &wstates[i];
    pthread_mutex_lock(&st->buf_mutex);
    if (!st->done) {
      StateMsg("setting done", st->filename);
      st->done = true;
      StateMsg("signaling done", st->filename);
      pthread_cond_signal(&st->buf_cond);
    }
    pthread_mutex_unlock(&st->buf_mutex);
    StateMsg("joining", st->filename);
    pthread_join(st->thread, &ret);
  }
  StateMsg("end of", "input");
}

  int
lace_builtin_elastic_pthread_main(unsigned argc, char** argv,
                                  LaceX** inputv, LaceO** outputv)
{
  WritingThreadState* wstates = NULL;
  size_t wstate_count = 0;
  LaceX* in = NULL;
  unsigned i;
  unsigned argi;

  StateMsg("begin", "main_elastic()");

  /* Upper bound for number of output files.*/
  wstates = (WritingThreadState*) malloc(sizeof(WritingThreadState) * argc);

  /**** BEGIN ARGUMENT_PARSING ****/
  for (argi = 1; argi < argc; ++argi) {
    const char* arg = argv[argi];
    WritingThreadState* st;

    if (0 == strcmp(arg, "-x")) {
      const char* xfilename = argv[++argi];
      in = open_arg_LaceXF(argi, argv, inputv);
      if (!in) {
        lace_log_errorf("failed to open: %s", xfilename);
        return 1;
      }
    } else {
      if (0 == strcmp(arg, "-o")) {
        arg = argv[++argi];
        if (argi == argc) {
          lace_log_error("Need output file after -o.");
          return 1;
        }
      }

      st = &wstates[wstate_count++];
      init_WritingThreadState(st);

      st->outfile = open_arg_LaceOF(argi, argv, outputv);
      if (st->outfile) {
        st->filename = filename_LaceOF(st->outfile);
      } else {
        lace_log_errorf("failed to open: %s", argv[argi]);
        return 1;
      }
      st->outfile->flush_lgsize = 0;  /* No automatic flushing.*/
    }
  }

  if (!in) {
    in = open_arg_LaceXF(0, argv, inputv);
    if (!in) {
      lace_log_error("Failed to open: /dev/stdin");
      return 1;
    }
  }

  if (wstate_count == 0) {
    WritingThreadState* st = &wstates[wstate_count++];
    init_WritingThreadState(st);
    st->filename = "-";
    st->outfile = open_arg_LaceOF(0, argv, outputv);
    if (!st->outfile) {
      lace_log_error("Failed to open: /dev/stdout");
      return 1;
    }
    st->outfile->flush_lgsize = 0;  /* No automatic flushing.*/
  }
  /**** END ARGUMENT_PARSING ****/

  reading_routine(in, wstates, wstate_count);

  for (i = 0; i < wstate_count; ++i) {
    lose_WritingThreadState(&wstates[i]);
  }
  free(wstates);
  StateMsg("end", "main_elastic()");
  return 0;
}

#ifndef LACE_BUILTIN_LIBRARY
int main(int argc, char** argv) {
  return lace_builtin_elastic_pthread_main(argc, argv, NULL, NULL);
}
#endif

