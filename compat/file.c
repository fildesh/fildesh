#include "fildesh_compat_file.h"

#ifndef _MSC_VER
/* For stdlib.h to provide realpath() on Debian.*/
#define _DEFAULT_SOURCE
#endif
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#ifndef _MSC_VER
#include <unistd.h>
#else
#include <windows.h>
#include <io.h>
#endif

  const char*
fildesh_compat_file_basename(const char* filepath)
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
fildesh_compat_file_abspath(const char* filepath)
{
#ifndef _MSC_VER
  /* This also resolves symlinks. Whatever.*/
  return realpath(filepath, NULL);
#else
  return _fullpath(NULL, filepath, _MAX_PATH);
#endif
}

  char*
fildesh_compat_file_catpath(const char* dir, const char* filename)
{
  const size_t dir_length = (dir ? strlen(dir) : 0);
  const size_t add_length = (filename ? strlen(filename) : 0);
  size_t i;
  char* p;
  if (dir_length + add_length == 0) {return NULL;}

  p = (char*) malloc(dir_length + 1 + add_length + 1);
  if (!p) {return NULL;}
  i = 0;
  memcpy(&p[i], dir, dir_length);
  i += dir_length;
  if (dir_length > 0 && dir[dir_length-1] != '/') {
    p[i++] = '/';
  }
  memcpy(&p[i], filename, add_length);
  i += add_length;
  p[i] = '\0';
  return p;
}

  int
fildesh_compat_file_chmod_u_rwx(const char* filename, int r, int w, int x)
{
  int istat, mode;
#ifndef _MSC_VER
  mode = (r ? S_IRUSR : 0) | (w ? S_IWUSR : 0) | (x ? S_IXUSR : 0);
  istat = chmod(filename, mode);
#else
  (void) x;
  mode = (r ? _S_IREAD : 0) | (w ? _S_IWRITE : 0);
  istat = _chmod(filename, mode);
#endif
  return istat;
}

  int
fildesh_compat_file_rm(const char* filepath)
{
#ifndef _MSC_VER
  return unlink(filepath);
#else
  return _unlink(filepath);
#endif
}

  int
fildesh_compat_file_rmdir(const char* dirpath)
{
#ifndef _MSC_VER
  return rmdir(dirpath);
#else
  return _rmdir(dirpath);
#endif
}
