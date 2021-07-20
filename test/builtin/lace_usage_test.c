/** Ensure that builtins return bad statuses for invalid usage.**/
#include "lace_compat_fd.h"
#include "lace_compat_file.h"
#include <assert.h>
#include <stdlib.h>

static void add_usage_tests(const char* lace_exe) {
  int istat;
  istat = lace_compat_fd_spawnlp_wait(
      0, 1, 2, NULL, lace_exe, "-as", "add",
      "no_arg_is_valid", NULL);
  assert(istat == 64);
}

static void bestmatch_usage_tests(const char* lace_exe, const char* bad_filename) {
  int istat;
  istat = lace_compat_fd_spawnlp_wait(
      0, 1, 2, NULL, lace_exe, "-as", "bestmatch",
      "need_one_more_than_this", NULL);
  assert(istat == 64);

  istat = lace_compat_fd_spawnlp_wait(
      0, 1, 2, NULL, lace_exe, "-as", "bestmatch",
      "-", "-", "--invalid-flag", NULL);
  assert(istat == 64);

  istat = lace_compat_fd_spawnlp_wait(
      0, 1, 2, NULL, lace_exe, "-as", "bestmatch",
      bad_filename, "-", NULL);
  assert(istat == 66);
}

static void
cmp_usage_tests(const char* lace_exe, const char* bad_filename)
{
  int istat;
  istat = lace_compat_fd_spawnlp_wait(
      0, 1, 2, NULL, lace_exe, "-as", "cmp",
      "need_one_more_than_thin", NULL);
  assert(istat == 64);

  istat = lace_compat_fd_spawnlp_wait(
      0, 1, 2, NULL, lace_exe, "-as", "cmp",
      "need", "one_less", "than_this", NULL);
  assert(istat == 64);

  /* First open should fail.*/
  istat = lace_compat_fd_spawnlp_wait(
      0, 1, 2, NULL, lace_exe, "-as", "cmp",
      bad_filename, "-", NULL);
  assert(istat == 66);

  /* Second open should fail.*/
  istat = lace_compat_fd_spawnlp_wait(
      0, 1, 2, NULL, lace_exe, "-as", "cmp",
      "-", "-", NULL);
  assert(istat == 66);
}

static void
elastic_pthread_usage_tests(const char* lace_exe, const char* bad_filename)
{
  int istat;
  istat = lace_compat_fd_spawnlp_wait(
      0, 1, 2, NULL, lace_exe, "-as", "elastic_pthread",
      "-x", NULL);
  assert(istat == 66);

  istat = lace_compat_fd_spawnlp_wait(
      0, 1, 2, NULL, lace_exe, "-as", "elastic_pthread",
      "-x", bad_filename, NULL);
  assert(istat == 66);

  istat = lace_compat_fd_spawnlp_wait(
      0, 1, 2, NULL, lace_exe, "-as", "elastic_pthread",
      "-o", NULL);
  assert(istat == 64);

  istat = lace_compat_fd_spawnlp_wait(
      0, 1, 2, NULL, lace_exe, "-as", "elastic_pthread",
      "-o", bad_filename, NULL);
  assert(istat == 73);
}

static void
time2sec_usage_tests(const char* lace_exe) {
  int istat;
  istat = lace_compat_fd_spawnlp_wait(
      0, 1, 2, NULL, lace_exe, "-as", "time2sec",
      "no_flagless_arg_is_valid", NULL);
  assert(istat == 64);

  istat = lace_compat_fd_spawnlp_wait(
      0, 1, 2, NULL, lace_exe, "-as", "time2sec",
      "-w", NULL);
  assert(istat == 64);
}

static void
ujoin_usage_tests(const char* lace_exe, const char* bad_filename) {
  int istat;
  /* Not quite enough args.*/
  istat = lace_compat_fd_spawnlp_wait(
      0, 1, 2, NULL, lace_exe, "-as", "ujoin",
      "need_one_more_than_this", NULL);
  assert(istat == 64);

  /* Second open should fail.*/
  istat = lace_compat_fd_spawnlp_wait(
      0, 1, 2, NULL, lace_exe, "-as", "ujoin",
      "-", "-", NULL);
  assert(istat == 66);

  /* Invalid output file.*/
  istat = lace_compat_fd_spawnlp_wait(
      0, 1, 2, NULL, lace_exe, "-as", "ujoin",
      "-", "-", "-o", bad_filename,  NULL);
  assert(istat == 73);

  /* No default record given.*/
  istat = lace_compat_fd_spawnlp_wait(
      0, 1, 2, NULL, lace_exe, "-as", "ujoin",
      "-", "-", "-p", NULL);
  assert(istat == 64);
}

int main(int argc, char** argv) {
  const char* lace_exe = argv[1];
  char* bad_filename;
  assert(argc == 2 && lace_exe && "Need lace executable.");

  bad_filename = lace_compat_file_catpath(lace_exe, "no_file_here");
  assert(bad_filename);

  add_usage_tests(lace_exe);
  bestmatch_usage_tests(lace_exe, bad_filename);
  cmp_usage_tests(lace_exe, bad_filename);
  elastic_pthread_usage_tests(lace_exe, bad_filename);
  time2sec_usage_tests(lace_exe);
  ujoin_usage_tests(lace_exe, bad_filename);

  free(bad_filename);
  return 0;
}
