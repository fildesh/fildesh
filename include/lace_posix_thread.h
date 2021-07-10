#ifndef LACE_POSIX_THREAD_H_
#define LACE_POSIX_THREAD_H_

#ifndef _MSC_VER
#include <pthread.h>

#define LACE_POSIX_THREAD_FUNCTION(name, T, arg) \
  static void name##_lace_posix_thread_function(T arg); \
  static void* name(void* voidarg) { \
    name##_lace_posix_thread_function((T) voidarg); \
    return NULL; \
  } \
  void name##_lace_posix_thread_function(T arg)
#else
#include <windows.h>
#include <process.h>
typedef HANDLE pthread_t;
typedef CONDITION_VARIABLE pthread_cond_t;
typedef CRITICAL_SECTION pthread_mutex_t;

#define LACE_POSIX_THREAD_FUNCTION(name, T, arg) \
  static void name##_lace_posix_thread_function(T arg); \
  static unsigned __stdcall name(void* voidarg) { \
    name##_lace_posix_thread_function((T) voidarg); \
    return 0; \
  } \
  void name##_lace_posix_thread_function(T arg)

static inline int
pthread_create(pthread_t* thread, void* ignored, unsigned (__stdcall *fn)(void*), void* arg) {
  (void) ignored;
  *thread = (pthread_t) _beginthreadex(
      NULL, 0, fn, arg, 0, NULL);
  /* The handle is 0 on error.*/
  return (*thread == 0 ? -1 : 0);
}
static inline int
pthread_cond_init(pthread_cond_t* cond, void* ignored) {
  (void) ignored;
  InitializeConditionVariable(cond);
  return 0;
}
static inline int
pthread_mutex_init(pthread_mutex_t* mutex, void* ignored) {
  (void) ignored;
  InitializeCriticalSection(mutex);
  return 0;
}

static inline int
pthread_mutex_lock(pthread_mutex_t* mutex) {
  EnterCriticalSection(mutex);
  return 0;
}
static inline int
pthread_mutex_unlock(pthread_mutex_t* mutex) {
  LeaveCriticalSection(mutex);
  return 0;
}
static inline int
pthread_cond_wait(pthread_cond_t* cond, pthread_mutex_t* mutex) {
  int good = SleepConditionVariableCS(cond, mutex, INFINITE);
  return (good ? 0 : -1);
}
static inline int
pthread_cond_signal(pthread_cond_t* cond) {
  WakeConditionVariable(cond);
  return 0;
}

static inline int
pthread_join(pthread_t thread, void** retval) {
  (void) retval;
  WaitForSingleObject(thread, INFINITE);
  CloseHandle(thread);
  return 0;
}
static inline int
pthread_cond_destroy(pthread_cond_t* cond) {
  (void) cond;
  return 0;
}
static inline int
pthread_mutex_destroy(pthread_mutex_t* mutex) {
  DeleteCriticalSection(mutex);
  return 0;
}
#endif

#endif  /* LACE_POSIX_THREAD_H_ */
