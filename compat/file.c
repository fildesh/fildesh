#include "lace_compat_file.h"

#include <stdlib.h>
#include <string.h>

  const char*
lace_compat_file_basename(const char* filepath)
{
  const char* s;
  if (!filepath) {return NULL;}
  s = strrchr(filepath, '/');
  if (s) {filepath = &s[1];}
#ifdef _WIN32
  s = strrchr(filepath, '\\');
  if (s) {filepath = &s[1];}
  s = strrchr(filepath, ':');
  if (s) {filepath = &s[1];}
#endif
  return filepath;
}

  char*
lace_compat_file_abspath(const char* filepath)
{
#ifdef _WIN32
  return _fullpath(NULL, filepath, _MAX_PATH);
#else
  /* This also resolves symlinks. Whatever.*/
  return realpath(filepath, NULL);
#endif
}
