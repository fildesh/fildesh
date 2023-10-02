#include "src/builtin/fildesh_builtin.h"

#include <string.h>

#if defined(FILDESH_PREFER_AIO)
# if !defined(FILDESH_BUILTIN_ELASTIC_AIO_ON)
#  define FILDESH_BUILTIN_ELASTIC_AIO_ON
# endif
#elif defined(FILDESH_PREFER_POLL)
# if !defined(FILDESH_BUILTIN_ELASTIC_POLL_ON)
#  define FILDESH_BUILTIN_ELASTIC_POLL_ON
# endif
#else
# if !defined(FILDESH_PREFER_PTHREAD)
#  define FILDESH_PREFER_PTHREAD
# endif
#endif

#ifdef FILDESH_BUILTIN_ELASTIC_AIO_ON
int main_elastic_aio(unsigned argc, char** argv);
#endif
#ifdef FILDESH_BUILTIN_ELASTIC_POLL_ON
int main_elastic_poll(unsigned argc, char** argv);
#endif
int main_godo(unsigned argc, char** argv);
int main_ssh_all(unsigned argc, char** argv);
int main_waitdo(unsigned argc, char** argv);
int main_xargz(unsigned argc, char** argv);
int main_xpipe(unsigned argc, char** argv);

static int fildesh_main_add(unsigned argc, char** argv) {
  return fildesh_builtin_add_main(argc, argv, NULL, NULL);
}
static int fildesh_main_bestmatch(unsigned argc, char** argv) {
  return fildesh_builtin_bestmatch_main(argc, argv, NULL, NULL);
}
static int fildesh_main_cmp(unsigned argc, char** argv) {
  return fildesh_builtin_cmp_main(argc, argv, NULL, NULL);
}
static int fildesh_main_delimend(unsigned argc, char** argv) {
  return fildesh_builtin_delimend_main(argc, argv, NULL, NULL);
}
static int fildesh_main_elastic_pthread(unsigned argc, char** argv) {
  return fildesh_builtin_elastic_pthread_main(argc, argv, NULL, NULL);
}
static int main_execfd(unsigned argc, char** argv) {
  return fildesh_builtin_execfd_main(argc, argv, NULL, NULL);
}
static int main_expect_failure(unsigned argc, char** argv) {
  return fildesh_builtin_expect_failure_main(argc, argv, NULL, NULL);
}
static int main_fildesh(unsigned argc, char** argv) {
  return fildesh_builtin_fildesh_main(argc, argv, NULL, NULL);
}
static int fildesh_main_replace_string(unsigned argc, char** argv) {
  return fildesh_builtin_replace_string_main(argc, argv, NULL, NULL);
}
static int fildesh_main_seq(unsigned argc, char** argv) {
  return fildesh_builtin_seq_main(argc, argv, NULL, NULL);
}
static int fildesh_main_sponge(unsigned argc, char** argv) {
  return fildesh_builtin_sponge_main(argc, argv, NULL, NULL);
}
static int fildesh_main_oargz(unsigned argc, char** argv) {
  return fildesh_builtin_oargz_main(argc, argv, NULL, NULL);
}
static int fildesh_main_sxpb2txtpb(unsigned argc, char** argv) {
  return fildesh_builtin_sxpb2txtpb_main(argc, argv, NULL, NULL);
}
static int fildesh_main_time2sec(unsigned argc, char** argv) {
  return fildesh_builtin_time2sec_main(argc, argv, NULL, NULL);
}
static int fildesh_main_transpose(unsigned argc, char** argv) {
  return fildesh_builtin_transpose_main(argc, argv, NULL, NULL);
}
static int fildesh_main_ujoin(unsigned argc, char** argv) {
  return fildesh_builtin_ujoin_main(argc, argv, NULL, NULL);
}
static int fildesh_main_void(unsigned argc, char** argv) {
  return fildesh_builtin_void_main(argc, argv, NULL, NULL);
}
static int fildesh_main_zec(unsigned argc, char** argv) {
  return fildesh_builtin_zec_main(argc, argv, NULL, NULL);
}

static int fildesh_main_elastic(unsigned argc, char** argv) {
  /* Ensure that exactly one default for elastic is defined
   * by otherwise throwing a syntax error for zero/multiple returns.
   */
#if defined(FILDESH_PREFER_PTHREAD)
  return fildesh_main_elastic_pthread(argc, argv);
#endif
#if defined(FILDESH_PREFER_AIO)
  return main_elastic_aio(argc, argv);
#endif
#if defined(FILDESH_PREFER_POLL)
  return main_elastic_poll(argc, argv);
#endif
}

int (* fildesh_builtin_threadsafe_fn_lookup(const char* name)
    )(unsigned, char**, FildeshX**, FildeshO**)
{
  typedef struct FildeshBuiltinMainMap FildeshBuiltinMainMap;
  struct FildeshBuiltinMainMap {
    const char* name;
    int (*main_fn)(unsigned, char**, FildeshX**, FildeshO**);
  };
  static const FildeshBuiltinMainMap builtins[] = {
    {"add", fildesh_builtin_add_main},
    {"bestmatch", fildesh_builtin_bestmatch_main},
    {"cmp", fildesh_builtin_cmp_main},
    {"delimend", fildesh_builtin_delimend_main},
#ifdef FILDESH_PREFER_PTHREAD
    {"elastic", fildesh_builtin_elastic_pthread_main},
#endif
    {"elastic_pthread", fildesh_builtin_elastic_pthread_main},
    {"execfd", fildesh_builtin_execfd_main},
    {"expect_failure", fildesh_builtin_expect_failure_main},
    {"oargz", fildesh_builtin_oargz_main},
    {"replace_string", fildesh_builtin_replace_string_main},
    {"seq", fildesh_builtin_seq_main},
    {"sponge", fildesh_builtin_sponge_main},
    {"sxpb2txtpb", fildesh_builtin_sxpb2txtpb_main},
    {"time2sec", fildesh_builtin_time2sec_main},
    {"transpose", fildesh_builtin_transpose_main},
    {"ujoin", fildesh_builtin_ujoin_main},
    {"void", fildesh_builtin_void_main},
    {"zec", fildesh_builtin_zec_main},
    {NULL, NULL},
  };
  unsigned i;
  for (i = 0; builtins[i].name; ++i) {
    if (0 == strcmp(name, builtins[i].name)) {
      return builtins[i].main_fn;
    }
  }
  return NULL;
}

int (* fildesh_builtin_main_fn_lookup(const char* name)
    )(unsigned, char**)
{
  typedef struct FildeshBuiltinMap FildeshBuiltinMap;
  struct FildeshBuiltinMap {
    const char* name;
    int (*main_fn)(unsigned,char**);
  };
  static const FildeshBuiltinMap builtins[] = {
    {"add", fildesh_main_add},
    {"best-match", fildesh_main_bestmatch},
    {"bestmatch", fildesh_main_bestmatch},
    {"cmp", fildesh_main_cmp},
    {"delimend", fildesh_main_delimend},
    {"elastic", fildesh_main_elastic},
    {"elastic_pthread", fildesh_main_elastic_pthread},
#ifdef FILDESH_BUILTIN_ELASTIC_AIO_ON
    {"elastic_aio", main_elastic_aio},
#endif
#ifdef FILDESH_BUILTIN_ELASTIC_POLL_ON
    {"elastic_poll", main_elastic_poll},
#endif
    {"execfd", main_execfd},
    {"expect_failure", main_expect_failure},
    {"fildesh", main_fildesh},
    {"godo", main_godo},
    {"oargz", fildesh_main_oargz},
    {"replace_string", fildesh_main_replace_string},
    {"seq", fildesh_main_seq},
    {"sponge", fildesh_main_sponge},
    {"ssh-all", main_ssh_all},
    {"sxpb2txtpb", fildesh_main_sxpb2txtpb},
    {"time2sec", fildesh_main_time2sec},
    {"transpose", fildesh_main_transpose},
    {"ujoin", fildesh_main_ujoin},
    {"void", fildesh_main_void},
    {"waitdo", main_waitdo},
    {"xargz", main_xargz},
    {"xpipe", main_xpipe},
    {"zec", fildesh_main_zec},
    {NULL, NULL},
  };
  unsigned i;

  for (i = 0; builtins[i].name; ++i) {
    if (0 == strcmp(builtins[i].name, name)) {
      return builtins[i].main_fn;
    }
  }
  return NULL;
}

int fildesh_builtin_main(const char* name, unsigned argc, char** argv)
{
  int (*f) (unsigned, char**);
  f = fildesh_builtin_main_fn_lookup(name);
  if (!f) {
    fildesh_log_errorf("Unknown builtin: %s", name);
    return -1;
  }
  return f(argc, argv);
}

