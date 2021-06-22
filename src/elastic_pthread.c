/**
 * \file elastic_pthread.c
 * Echo stdin to stdout with an arbitrary sized buffer.
 **/
#include "lace.h"
#include "compatible_pthread.h"

#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


typedef struct WritingThreadState WritingThreadState;

struct WritingThreadState
{
  bool done;
  bool waiting;
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
  st->waiting = true;
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

static
  void
badnews(const char* msg)
{
  fputs(msg, stderr);
}

static
  void
badnewsf(const char* fmt, const char* arg)
{
  fprintf(stderr, fmt, arg);
}

static
  void*
writing_thread_fn(WritingThreadState* st) {
  bool done = false;

  while (!done) {
    pthread_mutex_lock(&st->buf_mutex);
    StateMsg("loop start", st->filename);
    if (st->buf.size == 0 && !st->done) {
      st->waiting = true;
      StateMsg("waiting", st->filename);
      pthread_cond_wait(&st->buf_cond, &st->buf_mutex);
      StateMsg("done waiting", st->filename);
      st->waiting = false;
    }
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
  return NULL;
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
    istat = pthread_create(
        &st->thread, NULL,
        (void* (*) (void*)) writing_thread_fn,
        st);
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
          if (st->waiting) {
            pthread_cond_signal(&st->buf_cond);
          }
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
      if (st->waiting) {
        StateMsg("signaling done", st->filename);
        pthread_cond_signal(&st->buf_cond);
      }
    }
    pthread_mutex_unlock(&st->buf_mutex);
    StateMsg("joining", st->filename);
    pthread_join(st->thread, &ret);
  }
  StateMsg("end of", "input");
}

  int
main_elastic(int argi, int argc, char** argv)
{
  WritingThreadState* wstates = NULL;
  size_t wstate_count = 0;
  const char* xfilename = "/dev/fd/0";
  LaceX* in = NULL;
  unsigned i;

  StateMsg("begin", "main_elastic()");

  /* Upper bound for number of output files.*/
  wstates = (WritingThreadState*) malloc(sizeof(WritingThreadState) *
                                         (1 + argc - argi));

  /**** BEGIN ARGUMENT_PARSING ****/
  while (argi < argc) {
    const char* arg = argv[argi++];
    WritingThreadState* st;

    if (0 == strcmp(arg, "-x")) {
      if (argi == argc) {
        badnews("Need input file after -x.\n");
        return 1;
      }
      xfilename = argv[argi++];
    } else {
      if (0 == strcmp(arg, "-o")) {
        if (argi == argc) {
          badnews("Need output file after -o.\n");
          return 1;
        }
        arg = argv[argi++];
      }

      st = &wstates[wstate_count++];
      init_WritingThreadState(st);

      st->filename = arg;
      st->outfile = open_LaceOF(st->filename);
      if (!st->outfile) {
        badnewsf("failed to open: %s\n", st->filename);
        return 1;
      }
      st->outfile->flush_lgsize = 0;  /* No automatic flushing.*/
    }
  }

  in = open_LaceXF(xfilename);
  if (!in) {
    badnewsf("failed to open: %s\n", xfilename);
    return 1;
  }

  if (wstate_count == 0) {
    WritingThreadState* st = &wstates[wstate_count++];
    init_WritingThreadState(st);
    st->filename = "-";
    st->outfile = open_LaceOF(st->filename);
    if (!st->outfile) {
      badnews("Failed to open: /dev/stdout\n");
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

#ifndef MAIN_LACE_EXECUTABLE
int main(int argc, char** argv) {
  return main_elastic(1, argc, argv);
}
#endif

