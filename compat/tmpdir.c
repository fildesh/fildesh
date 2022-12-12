/** Temporary files.
 *
 * Our strategy is to create a temporary directory,
 * ideally with locked-down permissions,
 * and just work from there.
 *
 **/
#include "include/fildesh/fildesh_compat_file.h"
#include "include/fildesh/fildesh_compat_errno.h"
#include "include/fildesh/fildesh_compat_random.h"
#include "include/fildesh/fildesh_compat_sh.h"
#include "include/fildesh/fildesh_compat_string.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#ifdef _MSC_VER
# include <windows.h>
# include <direct.h>
# include <process.h>
#else
# include <unistd.h>
#endif

#include <sys/stat.h>
#include <sys/types.h>


static size_t pid_to_little_hex(char* s, fildesh_compat_pid_t pid) {
  size_t n = 0;
  do {
    s[n] = fildesh_compat_string_hexchar((unsigned)(pid & 0xf));
    pid = (pid >> 4);
    n += 1;
  } while (pid != 0 && n < 2*sizeof(pid));
  return n;
}

/**
 * \param hint  Can come in as a hint for the dirname.
 **/
  char*
fildesh_compat_file_mktmpdir(const char* hint)
{
  const char* v = 0;
#ifdef _MSC_VER
  fildesh_compat_pid_t pid = _getpid();
#else
  fildesh_compat_pid_t pid = getpid();
#endif
  char* buf;
  size_t hint_length;
  size_t total_size;
  size_t offset;
  int istat;
  if (!hint) {hint = "fildeshtmp";}
  hint_length = strlen(hint);

  v = getenv("TMPDIR");
#ifdef _MSC_VER
  if (!v) {
    v = getenv("TEMP");
  }
#endif
  if (!v) {
    v = "/tmp";
  }

  if (!v) {
    return NULL;
  }
  offset = strlen(v);
  total_size = offset + 1 + hint_length + 2*sizeof(pid) + 2 + 16 + 1;
  buf = (char*) malloc(total_size);
  if (!buf) {return NULL;}
  assert(offset < total_size);
  memcpy(buf, v, offset);
  buf[offset++] = '/';
  memcpy(&buf[offset], hint, hint_length);
  offset += hint_length;
  buf[offset++] = '-';
  offset += pid_to_little_hex(&buf[offset], pid);
  buf[offset++] = '-';
  assert(offset < total_size);
  offset += fildesh_compat_random_hex(&buf[offset], total_size-1-offset);
  assert(offset < total_size);
  buf[offset] = '\0';

#ifdef _MSC_VER
  istat = _mkdir(buf);
#else
  istat = mkdir(buf, 0700);
#endif
  if (istat != 0) {
    fildesh_compat_errno_trace();
    free(buf);
    buf = NULL;
  }
  return buf;
}

