#ifndef LACE_COMPAT_ERRNO_H_
#define LACE_COMPAT_ERRNO_H_

int
lace_compat_errno_clear();
int
lace_compat_errno_trace_(
    const char* file, const char* func, unsigned line);

#ifdef _MSC_VER
#define lace_compat_errno_trace() \
  lace_compat_errno_trace_(__FILE__,__FUNCTION__,__LINE__)
#else
#define lace_compat_errno_trace() \
  lace_compat_errno_trace_(__FILE__,__extension__ __func__,__LINE__)
#endif

#endif
