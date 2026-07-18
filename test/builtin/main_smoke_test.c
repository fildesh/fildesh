#include <assert.h>

#include "include/fildesh/fildesh_compat_fd.h"
#include "src/builtin/fildesh_builtin.h"

static
  int
prepare_argv(
    char** argv, int argc_max,
    const unsigned char* argstr, size_t n)
{
  int argc = 0;
  FildeshX in[1];
  FildeshX slice;
  *in = FildeshX_of_bytestring(argstr, n+1);
  for (slice = slicechr_FildeshX(in, '\0');
       slice.at;
       slice = slicechr_FildeshX(in, '\0'))
  {
    argv[argc++] = slice.at;
    assert(argc < argc_max);
  }
  argv[argc] = NULL;
  return argc;
}

static void prepare_stdio_fds() {
  FildeshCompat_fd fds[2];
  fildesh_compat_fd_pipe(&fds[0], &fds[1]);
  fildesh_compat_fd_move_to(0, fds[0]);
  fildesh_compat_fd_move_to(1, fds[1]);
}

static
  int
builtin_main_test(const unsigned char* argstr, size_t n)
{
  char* argv[11];
  unsigned argc = prepare_argv(argv, 10, argstr, n);
  prepare_stdio_fds();
  return fildesh_builtin_main(argv[0], argc, argv);
}
#define MAIN_TEST(e, s)  assert(e == builtin_main_test(fildesh_bytestrlit(s)))

static void builtin_add_test() {
  MAIN_TEST(64, "add\0invalid_argument");
}

static void builtin_bestmatch_test() {
  /* Need -x-lut.*/
  MAIN_TEST(64, "bestmatch\0-x\0/dev/null");
  /* Missing -d arg.*/
  MAIN_TEST(64, "bestmatch\0-x-lut\0/dev/null\0-d");
  /* Bad flag.*/
  MAIN_TEST(64, "bestmatch\0--invalid-flag");
  /* Cannot open files.*/
  MAIN_TEST(66, "bestmatch\0-x-lut\0/dev/null/absent");
  MAIN_TEST(66, "bestmatch\0-x\0/dev/null/absent");
  MAIN_TEST(73, "bestmatch\0-o\0/dev/null/absent");
}

static void builtin_capture_string_test() {
  MAIN_TEST(64, "capture_string");
  MAIN_TEST(64, "capture_string\0--");
  MAIN_TEST(66, "capture_string\0-x\0/dev/null/absent");
  MAIN_TEST(73, "capture_string\0-o\0/dev/null/absent");
}

static void builtin_cmp_test() {
  MAIN_TEST(0, "cmp\0/dev/null\0/dev/null");
  MAIN_TEST(0, "cmptxt\0/dev/null\0/dev/null");

  MAIN_TEST(64, "cmp\0/dev/null\0/dev/null\0too_many_files");
  MAIN_TEST(64, "cmptxt\0/dev/null\0/dev/null\0too_many_files");

  MAIN_TEST(64, "cmp\0-");
  MAIN_TEST(64, "cmptxt\0-");
}

static void builtin_command_test() {
  MAIN_TEST(127, "command");
  MAIN_TEST(127, "command\0--");

  MAIN_TEST(127, "builtin");
  MAIN_TEST(127, "builtin\0--");
  MAIN_TEST(127, "builtin\0/dev/null/absent");
  MAIN_TEST(0, "builtin\0--\0void");
  MAIN_TEST(10, "builtin\0expect_failure\0-status\0""10\0-propagate");
}

static void builtin_delimend_test() {
  MAIN_TEST(64, "delimend");
}

static void builtin_elastic_pthread_test() {
  MAIN_TEST(64, "elastic_pthread\0-o");
  MAIN_TEST(66, "elastic_pthread\0-x");
  MAIN_TEST(66, "elastic_pthread\0-x\0/dev/null/absent");
  MAIN_TEST(73, "elastic_pthread\0-o\0/dev/null/absent");
}

static void builtin_execfd_test() {
  MAIN_TEST(64, "execfd");
  MAIN_TEST(64, "execfd\0--");
  MAIN_TEST(64, "execfd\0""3\0--\0missing\0index\0three");
  /* Cannot open files.*/
  MAIN_TEST(66, "execfd\0-stdin\0/dev/null/absent");
  MAIN_TEST(73, "execfd\0-stdout\0/dev/null/absent");
  MAIN_TEST(73, "execfd\0-o?\0/dev/null/absent");
}

static void builtin_fildesh_test() {
  MAIN_TEST(64, "fildesh\0-x");
  MAIN_TEST(64, "fildesh\0--");
  MAIN_TEST(127, "fildesh\0-as");
  MAIN_TEST(10, "fildesh\0-as\0builtin\0expect_failure\0-status\0""10\0-propagate");
}

static void builtin_godo_test() {
  MAIN_TEST(64, "godo");
  MAIN_TEST(64, "godo\0.");
  MAIN_TEST(66, "godo\0/dev/null/absent\0/dev/null/absent");
  MAIN_TEST(66, "godo\0/dev/null/absent\0true");
}

static void builtin_replace_string_test() {
  MAIN_TEST(64, "replace_string");
  MAIN_TEST(64, "replace_string\0--");
  MAIN_TEST(66, "replace_string\0-x\0/dev/null/absent");
  MAIN_TEST(73, "replace_string\0-o\0/dev/null/absent");
}

static void builtin_splice_test() {
  /* Help.*/
  MAIN_TEST(1, "splice\0-h");
  /* Need filename after -x.*/
  MAIN_TEST(64, "splice\0-o\0/dev/null\0-x");
  /* Need string after -unless.*/
  MAIN_TEST(64, "splice\0-unless");
  /* Cannot open input file.*/
  MAIN_TEST(66, "splice\0-x\0/dev/null/absent");
  MAIN_TEST(66, "splice\0/dev/null/absent");
  MAIN_TEST(66, "splice\0/dev/null\0/dev/null\0/dev/null/absent");
  MAIN_TEST(66, "splice\0/\0/\0/dev/null\0/dev/null\0/dev/null/absent");
  /* Cannot open output file.*/
  MAIN_TEST(73, "splice\0-o\0/dev/null/absent");
}

static void builtin_sponge_test() {
  MAIN_TEST(64, "sponge\0too\0many");
  /* Cannot open files.*/
  MAIN_TEST(66, "sponge\0-x\0/dev/null/absent");
  MAIN_TEST(73, "sponge\0-x\0/dev/null\0--\0/dev/null/absent");
}

static void builtin_sxpb_test() {
  MAIN_TEST(64, "sxpb2json\0invalid_argument");
  MAIN_TEST(64, "sxpb2sxpb\0invalid_argument");
  MAIN_TEST(64, "sxpb2txtpb\0invalid_argument");
  MAIN_TEST(64, "sxpb2yaml\0invalid_argument");
}

static void builtin_time2sec_test() {
  MAIN_TEST(64, "time2sec\0no_flagless_arg_is_valid");
  MAIN_TEST(64, "time2sec\0-w");
}

static void builtin_transpose_test() {
  MAIN_TEST(64, "transpose\0invalid_argument");
  /* Delimiter flag needs a value.*/
  MAIN_TEST(64, "transpose\0-d");
}

static void builtin_ujoin_test() {
  /* Not quite enough args.*/
  MAIN_TEST(64, "ujoin\0-x\0-");
  /* Invalid argument.*/
  MAIN_TEST(64, "ujoin\0invalid_argument");
  /* Delimiter missing.*/
  MAIN_TEST(64, "ujoin\0-d");
  /* No default record given.*/
  MAIN_TEST(64, "ujoin\0-x-lut\0/dev/null\0-x\0/dev/null\0-p");
  /* Invalid output files.*/
  MAIN_TEST(73, "ujoin\0-o\0/dev/null/absent");
  MAIN_TEST(73, "ujoin\0-o-not-found\0/dev/null/absent");
  MAIN_TEST(73, "ujoin\0-o-conflicts\0/dev/null/absent");
}

int main() {
  builtin_add_test();
  builtin_bestmatch_test();
  builtin_capture_string_test();
  builtin_cmp_test();
  builtin_command_test();
  builtin_delimend_test();
  builtin_elastic_pthread_test();
  builtin_execfd_test();
  builtin_fildesh_test();
  builtin_godo_test();
  builtin_replace_string_test();
  builtin_splice_test();
  builtin_sponge_test();
  builtin_sxpb_test();
  builtin_time2sec_test();
  builtin_transpose_test();
  builtin_ujoin_test();

  /* Need to give a command.*/
  MAIN_TEST(64, "xargz\0--");

  return 0;
}
