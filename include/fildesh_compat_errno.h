#ifndef FILDESH_COMPAT_ERRNO_H_
#define FILDESH_COMPAT_ERRNO_H_

int
fildesh_compat_errno_clear();
int
fildesh_compat_errno_trace_(
    const char* file, const char* func, unsigned line);

#ifdef _MSC_VER
#define fildesh_compat_errno_trace() \
  fildesh_compat_errno_trace_(__FILE__,__FUNCTION__,__LINE__)
#else
#define fildesh_compat_errno_trace() \
  fildesh_compat_errno_trace_(__FILE__,__extension__ __func__,__LINE__)
#endif

#endif
