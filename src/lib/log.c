#define FILDESH_LOG_TRACE_ON
#include <fildesh/fildesh.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

  void
fildesh_log_errorf(const char* fmt, ...)
{
  va_list argp;
  fputs("ERROR: ", stderr);
  va_start(argp, fmt);
  vfprintf(stderr, fmt, argp);
  va_end(argp);
  fputc('\n', stderr);
}

  void
fildesh_log_warningf(const char* fmt, ...)
{
  va_list argp;
  fputs("WARNING: ", stderr);
  va_start(argp, fmt);
  vfprintf(stderr, fmt, argp);
  va_end(argp);
  fputc('\n', stderr);
}

  void
fildesh_log_infof(const char* fmt, ...)
{
  va_list argp;
  fputs("INFO: ", stderr);
  va_start(argp, fmt);
  vfprintf(stderr, fmt, argp);
  va_end(argp);
  fputc('\n', stderr);
}

  void
fildesh_log_tracef(const char* fmt, ...)
{
  va_list argp;
  fputs("TRACE: ", stderr);
  va_start(argp, fmt);
  vfprintf(stderr, fmt, argp);
  va_end(argp);
  fputc('\n', stderr);
}

static const char* filename_to_print(const char* file) {
  const char* tmp = strrchr(file, '/');
  if (tmp) {file = &tmp[1];}
  tmp = strrchr(file, '\\');
  if (tmp) {file = &tmp[1];}
  return file;
}

  void
fildesh_log_error_(
    const char* file, const char* func, unsigned line, const char* msg)
{
  fprintf(
      stderr,
      "ERROR %s(%u) in %s: %s\n",
      filename_to_print(file),
      line, func, msg);
}

  void
fildesh_log_warning_(
    const char* file, const char* func, unsigned line, const char* msg)
{
  fprintf(
      stderr,
      "WARNING %s(%u) in %s: %s\n",
      filename_to_print(file),
      line, func, msg);
}

  void
fildesh_log_info_(
    const char* file, const char* func, unsigned line, const char* msg)
{
  fprintf(
      stderr,
      "INFO %s(%u) in %s: %s\n",
      filename_to_print(file),
      line, func, msg);
}

  void
fildesh_log_trace_(
    const char* file, const char* func, unsigned line, const char* msg)
{
  fprintf(
      stderr,
      "TRACE %s(%u) in %s: %s\n",
      filename_to_print(file),
      line, func, msg);
}
