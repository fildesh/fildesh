/**
 * \file elastic_pthread.c
 * Echo stdin to stdout with an arbitrary sized buffer.
 **/
#include <fildesh/fildesh.h>
#include "fildesh_posix_thread.h"

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
  FildeshO buf;
  FildeshO* outfile;
  const char* filename;
};

static
  void
init_WritingThreadState(WritingThreadState* st)
{
  st->done = false;
  pthread_cond_init(&st->buf_cond, NULL);
  pthread_mutex_init(&st->buf_mutex, NULL);
  st->buf = default_FildeshO();
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
  close_FildeshO(&st->buf);
  close_FildeshO(st->outfile);
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

FILDESH_POSIX_THREAD_CALLBACK(writing_thread_fn, WritingThreadState*, st)
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
    memcpy(grow_FildeshO(st->outfile, st->buf.size),
           &st->buf.at[0], st->buf.size);
    st->buf.size = 0;
    pthread_mutex_unlock(&st->buf_mutex);

    flush_FildeshO(st->outfile);
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
  close_FildeshO(&st->buf);
  close_FildeshO(st->outfile);
  st->outfile = NULL;
}

/** Reading is actually done on the main thread.**/
static
  void
reading_routine(FildeshX* in, WritingThreadState* wstates, size_t wstate_count)
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
      const size_t nbytes = read_FildeshX(in);
      if (nbytes == 0) {
        break;
      }

      for (i = 0; i < wstate_count; ++i) {
        WritingThreadState* st = &wstates[i];
        pthread_mutex_lock(&st->buf_mutex);
        if (!st->done) {
          StateMsg("catting", st->filename);
          memcpy(grow_FildeshO(&st->buf, nbytes),
                 &in->at[in->off],
                 nbytes);
          pthread_cond_signal(&st->buf_cond);
        }
        pthread_mutex_unlock(&st->buf_mutex);
      }
      in->off += nbytes;
      assert(in->off == in->size);
      maybe_flush_FildeshX(in);
    }
  }
  close_FildeshX(in);
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

static void cleanup_wstates(WritingThreadState* wstates, unsigned wstate_count) {
  unsigned i;
  if (!wstates) {return;}
  for (i = 0; i < wstate_count; ++i) {
    lose_WritingThreadState(&wstates[i]);
  }
  free(wstates);
}

  int
fildesh_builtin_elastic_pthread_main(unsigned argc, char** argv,
                                  FildeshX** inputv, FildeshO** outputv)
{
  WritingThreadState* wstates = NULL;
  unsigned wstate_count = 0;
  FildeshX* in = NULL;
  unsigned argi;

  StateMsg("begin", "main_elastic()");

  /* Upper bound for number of output files.*/
  wstates = (WritingThreadState*) malloc(sizeof(WritingThreadState) * argc);

  /**** BEGIN ARGUMENT_PARSING ****/
  for (argi = 1; argi < argc; ++argi) {
    const char* arg = argv[argi];
    if (0 == strcmp(arg, "-x")) {
      const char* xfilename = argv[++argi];
      in = open_arg_FildeshXF(argi, argv, inputv);
      if (!in) {
        fildesh_log_errorf("failed to open: %s", xfilename);
        cleanup_wstates(wstates, wstate_count);
        return 66;
      }
    } else {
      WritingThreadState* st;
      if (0 == strcmp(arg, "-o")) {
        const char* ofilename = argv[++argi];
        if (!ofilename) {
          fildesh_log_error("Need output file after -o.");
          cleanup_wstates(wstates, wstate_count);
          return 64;
        }
      }

      st = &wstates[wstate_count++];
      init_WritingThreadState(st);

      st->outfile = open_arg_FildeshOF(argi, argv, outputv);
      if (st->outfile) {
        st->filename = filename_FildeshOF(st->outfile);
      } else {
        fildesh_log_errorf("failed to open: %s", argv[argi]);
        cleanup_wstates(wstates, wstate_count);
        return 73;
      }
      st->outfile->flush_lgsize = 0;  /* No automatic flushing.*/
    }
  }

  if (!in) {
    in = open_arg_FildeshXF(0, argv, inputv);
    if (!in) {
      fildesh_log_error("Failed to open: /dev/stdin");
      cleanup_wstates(wstates, wstate_count);
      return 1;
    }
  }

  if (wstate_count == 0) {
    WritingThreadState* st = &wstates[wstate_count++];
    init_WritingThreadState(st);
    st->filename = "-";
    st->outfile = open_arg_FildeshOF(0, argv, outputv);
    if (!st->outfile) {
      fildesh_log_error("Failed to open: /dev/stdout");
      return 1;
    }
    st->outfile->flush_lgsize = 0;  /* No automatic flushing.*/
  }
  else {
    /* Close stdout if it exists. We don't use it.*/
    FildeshO* tmp_out = open_arg_FildeshOF(0, argv, outputv);
    close_FildeshO(tmp_out);
  }
  /**** END ARGUMENT_PARSING ****/

  reading_routine(in, wstates, wstate_count);

  cleanup_wstates(wstates, wstate_count);
  StateMsg("end", "main_elastic()");
  return 0;
}

#ifndef FILDESH_BUILTIN_LIBRARY
int main(int argc, char** argv) {
  return fildesh_builtin_elastic_pthread_main(argc, argv, NULL, NULL);
}
#endif

