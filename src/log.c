#include <stdarg.h>
#include <stdio.h>
#include <string.h>

  void
lace_log_errorf(const char* fmt, ...)
{
  va_list argp;
  fputs("ERROR: ", stderr);
  va_start(argp, fmt);
  vfprintf(stderr, fmt, argp);
  va_end(argp);
  fputc('\n', stderr);
}

  void
lace_log_warningf(const char* fmt, ...)
{
  va_list argp;
  fputs("WARNING: ", stderr);
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
  return tmp;
}


void
lace_log_error_(
    const char* file, const char* func, unsigned line, const char* msg)
{
  fprintf(
      stderr,
      "ERROR %s(%u) in %s: %s\n",
      filename_to_print(file),
      line, func, msg);
}

void
lace_log_warning_(
    const char* file, const char* func, unsigned line, const char* msg)
{
  fprintf(
      stderr,
      "WARNING %s(%u) in %s: %s\n",
      filename_to_print(file),
      line, func, msg);
}
