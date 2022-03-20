/** Ensure that builtins return bad statuses for invalid usage.**/
#include "fildesh_compat_fd.h"
#include "fildesh_compat_file.h"
#include <assert.h>
#include <stdlib.h>

static void add_usage_tests(const char* fildesh_exe) {
  int istat;
  istat = fildesh_compat_fd_spawnlp_wait(
      0, 1, 2, NULL, fildesh_exe, "-as", "add",
      "no_arg_is_valid", NULL);
  assert(istat == 64);
}

static void bestmatch_usage_tests(const char* fildesh_exe, const char* bad_filename) {
  int istat;
  /* Need -x-lut.*/
  istat = fildesh_compat_fd_spawnlp_wait(
      0, 1, 2, NULL, fildesh_exe, "-as", "bestmatch",
      "-x", "/dev/null", NULL);
  assert(istat == 64);

  /* Missing -d arg.*/
  istat = fildesh_compat_fd_spawnlp_wait(
      0, 1, 2, NULL, fildesh_exe, "-as", "bestmatch",
      "-x-lut", "/dev/null", "-d", NULL);
  assert(istat == 64);

  /* Bad flag.*/
  istat = fildesh_compat_fd_spawnlp_wait(
      0, 1, 2, NULL, fildesh_exe, "-as", "bestmatch",
      "--invalid-flag", NULL);
  assert(istat == 64);

  /* Can't open files.*/
  istat = fildesh_compat_fd_spawnlp_wait(
      0, 1, 2, NULL, fildesh_exe, "-as", "bestmatch",
      "-x-lut", bad_filename, NULL);
  assert(istat == 66);

  istat = fildesh_compat_fd_spawnlp_wait(
      0, 1, 2, NULL, fildesh_exe, "-as", "bestmatch",
      "-x", bad_filename, NULL);
  assert(istat == 66);

  istat = fildesh_compat_fd_spawnlp_wait(
      0, 1, 2, NULL, fildesh_exe, "-as", "bestmatch",
      "-o", bad_filename, NULL);
  assert(istat == 73);
}

static void
cmp_usage_tests(const char* fildesh_exe, const char* bad_filename)
{
  int istat;
  istat = fildesh_compat_fd_spawnlp_wait(
      0, 1, 2, NULL, fildesh_exe, "-as", "cmp",
      "/dev/null", "/dev/null", NULL);
  assert(istat == 0);

  istat = fildesh_compat_fd_spawnlp_wait(
      0, 1, 2, NULL, fildesh_exe, "-as", "cmp",
      "-", NULL);
  assert(istat == 64);

  istat = fildesh_compat_fd_spawnlp_wait(
      0, 1, 2, NULL, fildesh_exe, "-as", "cmp",
      "/dev/null", "/dev/null", "too_many_files", NULL);
  assert(istat == 64);

  istat = fildesh_compat_fd_spawnlp_wait(
      0, 1, 2, NULL, fildesh_exe, "-as", "cmp",
      "-o", bad_filename, NULL);
  assert(istat == 73);

  /* First open should fail.*/
  istat = fildesh_compat_fd_spawnlp_wait(
      0, 1, 2, NULL, fildesh_exe, "-as", "cmp",
      bad_filename, "-", NULL);
  assert(istat == 66);

  /* Second open should fail.*/
  istat = fildesh_compat_fd_spawnlp_wait(
      0, 1, 2, NULL, fildesh_exe, "-as", "cmp",
      "-", "-", NULL);
  assert(istat == 66);
}

static void
elastic_pthread_usage_tests(const char* fildesh_exe, const char* bad_filename)
{
  int istat;
  istat = fildesh_compat_fd_spawnlp_wait(
      0, 1, 2, NULL, fildesh_exe, "-as", "elastic_pthread",
      "-x", NULL);
  assert(istat == 66);

  istat = fildesh_compat_fd_spawnlp_wait(
      0, 1, 2, NULL, fildesh_exe, "-as", "elastic_pthread",
      "-x", bad_filename, NULL);
  assert(istat == 66);

  istat = fildesh_compat_fd_spawnlp_wait(
      0, 1, 2, NULL, fildesh_exe, "-as", "elastic_pthread",
      "-o", NULL);
  assert(istat == 64);

  istat = fildesh_compat_fd_spawnlp_wait(
      0, 1, 2, NULL, fildesh_exe, "-as", "elastic_pthread",
      "-o", bad_filename, NULL);
  assert(istat == 73);
}

static void
execfd_usage_tests(const char* fildesh_exe, const char* bad_filename)
{
  int istat;
  istat = fildesh_compat_fd_spawnlp_wait(
      0, 1, 2, NULL, fildesh_exe, "-as", "execfd", NULL);
  assert(istat == 64);

  istat = fildesh_compat_fd_spawnlp_wait(
      0, 1, 2, NULL, fildesh_exe, "-as", "execfd",
      "-stdin", bad_filename, NULL);
  assert(istat == 66);

  istat = fildesh_compat_fd_spawnlp_wait(
      0, 1, 2, NULL, fildesh_exe, "-as", "execfd",
      "-stdout", bad_filename, NULL);
  assert(istat == 73);

  istat = fildesh_compat_fd_spawnlp_wait(
      0, 1, 2, NULL, fildesh_exe, "-as", "execfd",
      "-o?", bad_filename, NULL);
  assert(istat == 73);

  istat = fildesh_compat_fd_spawnlp_wait(
      0, 1, 2, NULL, fildesh_exe, "-as", "execfd", "--", NULL);
  assert(istat == 64);

  istat = fildesh_compat_fd_spawnlp_wait(
      0, 1, 2, NULL, fildesh_exe, "-as", "execfd",
      "3", "--", "missing", "index", "three", NULL);
  assert(istat == 64);
}

static void
time2sec_usage_tests(const char* fildesh_exe) {
  int istat;
  istat = fildesh_compat_fd_spawnlp_wait(
      0, 1, 2, NULL, fildesh_exe, "-as", "time2sec",
      "no_flagless_arg_is_valid", NULL);
  assert(istat == 64);

  istat = fildesh_compat_fd_spawnlp_wait(
      0, 1, 2, NULL, fildesh_exe, "-as", "time2sec",
      "-w", NULL);
  assert(istat == 64);
}

static void
ujoin_usage_tests(const char* fildesh_exe, const char* bad_filename) {
  const char* good_filename = "/dev/null";
  int istat;
  /* Not quite enough args.*/
  istat = fildesh_compat_fd_spawnlp_wait(
      0, 1, 2, NULL, fildesh_exe, "-as", "ujoin",
      "-x", "-", NULL);
  assert(istat == 64);

  /* Delimiter missing.*/
  istat = fildesh_compat_fd_spawnlp_wait(
      0, 1, 2, NULL, fildesh_exe, "-as", "ujoin", "-d", NULL);
  assert(istat == 64);

  /* Second open should fail.*/
  istat = fildesh_compat_fd_spawnlp_wait(
      0, 1, 2, NULL, fildesh_exe, "-as", "ujoin",
      "-x-lut", "-", "-x", "-", NULL);
  assert(istat == 66);

  /* Invalid output files.*/
  istat = fildesh_compat_fd_spawnlp_wait(
      0, 1, 2, NULL, fildesh_exe, "-as", "ujoin",
      "-o", bad_filename,  NULL);
  assert(istat == 73);

  istat = fildesh_compat_fd_spawnlp_wait(
      0, 1, 2, NULL, fildesh_exe, "-as", "ujoin",
      "-o-not-found", bad_filename,  NULL);
  assert(istat == 73);

  istat = fildesh_compat_fd_spawnlp_wait(
      0, 1, 2, NULL, fildesh_exe, "-as", "ujoin",
      "-o-conflicts", bad_filename,  NULL);
  assert(istat == 73);

  /* No default record given.*/
  istat = fildesh_compat_fd_spawnlp_wait(
      0, 1, 2, NULL, fildesh_exe, "-as", "ujoin",
      "-x-lut", good_filename, "-x", good_filename, "-p", NULL);
  assert(istat == 64);
}

int main(int argc, char** argv) {
  const char* fildesh_exe = argv[1];
  char* bad_filename;
  assert(argc == 2 && fildesh_exe && "Need fildesh executable.");

  bad_filename = fildesh_compat_file_catpath(fildesh_exe, "no_file_here");
  assert(bad_filename);

  add_usage_tests(fildesh_exe);
  bestmatch_usage_tests(fildesh_exe, bad_filename);
  cmp_usage_tests(fildesh_exe, bad_filename);
  elastic_pthread_usage_tests(fildesh_exe, bad_filename);
  execfd_usage_tests(fildesh_exe, bad_filename);
  time2sec_usage_tests(fildesh_exe);
  ujoin_usage_tests(fildesh_exe, bad_filename);

  free(bad_filename);
  return 0;
}
