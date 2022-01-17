/* Readers-writer lock for use within fd.c routines.*/

#ifndef _MSC_VER
#include <pthread.h>

#ifdef PTHREAD_RWLOCK_INITIALIZER
static pthread_rwlock_t fildesh_compat_fd_lock = PTHREAD_RWLOCK_INITIALIZER;
#define FILDESH_COMPAT_FD_ENTER_SHARED pthread_rwlock_rdlock(&fildesh_compat_fd_lock)
#define FILDESH_COMPAT_FD_LEAVE_SHARED pthread_rwlock_unlock(&fildesh_compat_fd_lock)
#define FILDESH_COMPAT_FD_ENTER_EXCLUSIVE pthread_rwlock_wrlock(&fildesh_compat_fd_lock)
#define FILDESH_COMPAT_FD_LEAVE_EXCLUSIVE pthread_rwlock_unlock(&fildesh_compat_fd_lock)
#else
static pthread_mutex_t fildesh_compat_fd_lock = PTHREAD_MUTEX_INITIALIZER;
#define FILDESH_COMPAT_FD_ENTER_SHARED pthread_mutex_lock(&fildesh_compat_fd_lock)
#define FILDESH_COMPAT_FD_LEAVE_SHARED pthread_mutex_unlock(&fildesh_compat_fd_lock)
#define FILDESH_COMPAT_FD_ENTER_EXCLUSIVE pthread_mutex_lock(&fildesh_compat_fd_lock)
#define FILDESH_COMPAT_FD_LEAVE_EXCLUSIVE pthread_mutex_unlock(&fildesh_compat_fd_lock)
#endif

#else
#include <synchapi.h>
static SRWLOCK fildesh_compat_fd_lock = SRWLOCK_INIT;

#define FILDESH_COMPAT_FD_ENTER_SHARED AcquireSRWLockShared(&fildesh_compat_fd_lock)
#define FILDESH_COMPAT_FD_LEAVE_SHARED ReleaseSRWLockShared(&fildesh_compat_fd_lock)

#define FILDESH_COMPAT_FD_ENTER_EXCLUSIVE AcquireSRWLockExclusive(&fildesh_compat_fd_lock)
#define FILDESH_COMPAT_FD_LEAVE_EXCLUSIVE ReleaseSRWLockExclusive(&fildesh_compat_fd_lock)

#endif

