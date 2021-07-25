#ifndef LACE_H_
#define LACE_H_
#include <limits.h>
#include <stddef.h>
#include <stdint.h>

#if defined(__cplusplus) || defined(__OPENCL_VERSION__)
/* These already have bool.*/
#elif defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901L)
#include <stdbool.h>
#elif !defined(true)
# define true 1
# define false 0
typedef char bool;
# define bool bool
#endif

typedef int lace_fd_t;
typedef uint8_t lace_lgsize_t;
#define LACE_LGSIZE_MAX UCHAR_MAX

typedef struct LaceX_VTable LaceX_VTable;
typedef struct LaceX LaceX;
typedef struct LaceO_VTable LaceO_VTable;
typedef struct LaceO LaceO;
typedef struct LaceKV LaceKV;
typedef struct LaceKVE LaceKVE;

size_t read_LaceX(LaceX*);
void close_LaceX(LaceX*);
char* grow_LaceX(LaceX*, size_t);
void flush_LaceX(LaceX*);
void maybe_flush_LaceX(LaceX*);
char* slurp_LaceX(LaceX*);
LaceX slicechr_LaceX(LaceX*, const char delim);
LaceX sliceline_LaceX(LaceX*);
LaceX slicechrs_LaceX(LaceX*, const char* delims);
LaceX slicespan_LaceX(LaceX*, const char* span);
LaceX slicestr_LaceX(LaceX*, const char* delim);
char* getline_LaceX(LaceX*);
char* gets_LaceX(LaceX*, const char* delim);
bool skipchrs_LaceX(LaceX*, const char* span);
bool skipstr_LaceX(LaceX*, const char* s);
bool parse_int_LaceX(LaceX*, int*);
bool parse_double_LaceX(LaceX*, double*);

LaceX* open_LaceXA();

lace_fd_t lace_arg_open_readonly(const char*);
LaceX* open_LaceXF(const char* filename);
LaceX* open_sibling_LaceXF(const char* sibling, const char* filename);
LaceX* open_fd_LaceX(lace_fd_t fd);
LaceX* open_arg_LaceXF(unsigned argi, char** argv, LaceX** inputv);

const char* filename_LaceXF(LaceX*);

size_t write_LaceO(LaceO*);
void close_LaceO(LaceO*);
char* grow_LaceO(LaceO*, size_t);
void flush_LaceO(LaceO*);
void maybe_flush_LaceO(LaceO*);
void putc_LaceO(LaceO*, char);
void puts_LaceO(LaceO*, const char*);
void print_int_LaceO(LaceO*, int);
void print_double_LaceO(LaceO*, double);

lace_fd_t lace_arg_open_writeonly(const char*);
LaceO* open_LaceOF(const char* filename);
LaceO* open_sibling_LaceOF(const char* sibling, const char* filename);
LaceO* open_fd_LaceO(lace_fd_t fd);
LaceO* open_arg_LaceOF(unsigned argi, char** argv, LaceO** outputv);

const char* filename_LaceOF(LaceO*);


char* lace_parse_int(int* ret, const char* in);
char* lace_parse_double(double* ret, const char* in);
#define LACE_INT_BASE10_SIZE_MAX (1 + (unsigned)(CHAR_BIT*sizeof(int)) / 3 + 1)
unsigned lace_encode_int_base10(char*, lace_fd_t);
#define LACE_FD_PATH_SIZE_MAX (8+LACE_INT_BASE10_SIZE_MAX)
unsigned lace_encode_fd_path(char*, lace_fd_t);


void lace_log_errorf(const char*, ...);
void lace_log_warningf(const char*, ...);
void lace_log_tracef(const char*, ...);
void lace_log_error_(
    const char* file, const char* func, unsigned line, const char* msg);
void lace_log_warning_(
    const char* file, const char* func, unsigned line, const char* msg);
void lace_log_trace_(
    const char* file, const char* func, unsigned line, const char* msg);
#ifndef _MSC_VER
#define lace_log_error(s)  lace_log_error_(__FILE__,__extension__ __func__,__LINE__, s)
#define lace_log_warning(s)  lace_log_warning_(__FILE__,__extension__ __func__,__LINE__, s)
#define lace_log_trace(s)  lace_log_trace_(__FILE__,__extension__ __func__,__LINE__, s)
#else
#define lace_log_error(s)  lace_log_error_(__FILE__,__FUNCTION__,__LINE__,s)
#define lace_log_warning(s)  lace_log_warning_(__FILE__,__FUNCTION__,__LINE__,s)
#define lace_log_trace(s)  lace_log_trace_(__FILE__,__FUNCTION__,__LINE__,s)
#endif


/** Given the memory address of a structure's field,
 * get the address of the structure.
 * \param T      Type.
 * \param field  Name of the field.
 * \param p      Memory address of the field.
 *
 * lace_castup( T, field, p ) == container_of(p, T, field)
 **/
#define lace_castup( T, field, p ) \
  ((T*) ((uintptr_t) (p) - (ptrdiff_t) offsetof(T, field)))


struct LaceX_VTable
{
  void (*read_fn)(LaceX*);
  void (*close_fn)(LaceX*);
  void (*free_fn)(LaceX*);
};
#define DEFINE_LaceX_VTable(T, field) \
  static void read_##T##_LaceX(LaceX* x) { \
    read_##T(lace_castup(T, field, x)); \
  } \
  static void close_##T##_LaceX(LaceX* x) { \
    close_##T(lace_castup(T, field, x)); \
  } \
  static void free_##T##_LaceX(LaceX* x) { \
    free_##T(lace_castup(T, field, x)); \
  } \
  static const LaceX_VTable DEFAULT_##T##_LaceX_VTable[1] = {{ \
    read_##T##_LaceX, \
    close_##T##_LaceX, \
    free_##T##_LaceX, \
  }}

struct LaceX {
  char* at;
  size_t size;
  size_t off;
  lace_lgsize_t alloc_lgsize;
  lace_lgsize_t flush_lgsize;
  const LaceX_VTable* vt;
};
#define DEFAULT_LaceX  { NULL, 0, 0, 0, 12, NULL }
#define DEFAULT1_LaceX(vt)  { NULL, 0, 0, 0, 12, vt }

struct LaceO_VTable
{
  void (*write_fn)(LaceO*);
  void (*close_fn)(LaceO*);
  void (*free_fn)(LaceO*);
};
#define DEFINE_LaceO_VTable(T, field) \
  static void write_##T##_LaceO(LaceO* o) { \
    write_##T(lace_castup(T, field, o)); \
  } \
  static void close_##T##_LaceO(LaceO* o) { \
    close_##T(lace_castup(T, field, o)); \
  } \
  static void free_##T##_LaceO(LaceO* o) { \
    free_##T(lace_castup(T, field, o)); \
  } \
  static const LaceO_VTable DEFAULT_##T##_LaceO_VTable[1] = {{ \
    write_##T##_LaceO, \
    close_##T##_LaceO, \
    free_##T##_LaceO, \
  }}

struct LaceO {
  char* at;
  size_t size;
  size_t off;
  lace_lgsize_t alloc_lgsize;
  lace_lgsize_t flush_lgsize;
  const LaceO_VTable* vt;
};
#define DEFAULT_LaceO  { NULL, 0, 0, 0, 12, NULL }
#define DEFAULT1_LaceO(vt)  { NULL, 0, 0, 0, 12, vt }

struct LaceKV {
  LaceKVE* at;
  size_t freelist_head;
  size_t freelist_tail;
  lace_lgsize_t lgcount;
};

#if !defined(ZEROIZE)
# define ZEROIZE(x)  memset(&(x), 0, sizeof(x))
#endif

#if __STDC_VERSION__ < 199901L && !defined(inline)
#define inline __inline
#endif
static inline LaceX default_LaceX() {LaceX tmp = DEFAULT_LaceX; return tmp;}
static inline LaceO default_LaceO() {LaceO tmp = DEFAULT_LaceO; return tmp;}

static inline
  void*
grow_LaceA_(void** p_at, size_t* p_count, lace_lgsize_t* p_allocated_lgcount,
            size_t element_size, size_t difference,
            void* (*realloc_fn) (void*, size_t))
{
  void* at = *p_at;
  const size_t count = *p_count + difference;
  lace_lgsize_t allocated_lgcount = *p_allocated_lgcount;

  if ((count << 1) > ((size_t)1 << allocated_lgcount)) {
    if (allocated_lgcount == 0) {
      allocated_lgcount = 1;
    }
    do {
      allocated_lgcount += 1;
    } while ((count << 1) > ((size_t)1 << allocated_lgcount));
    at = realloc_fn(at, element_size << (allocated_lgcount - 1));
    if (!at) {return NULL;}

    *p_at = at;
    *p_allocated_lgcount = allocated_lgcount;
  }
  *p_count = count;
  return (void*)((uintptr_t)at + (count - difference) * element_size);
}

static inline
  void
mpop_LaceA_(void** p_at, size_t* p_count, lace_lgsize_t* p_allocated_lgcount,
            size_t element_size, size_t difference,
            void* (*realloc_fn) (void*, size_t))
{
  void* at = *p_at;
  const size_t count = *p_count - difference;
  lace_lgsize_t allocated_lgcount = *p_allocated_lgcount;

  *p_count = count;
  if ((allocated_lgcount >= 3) && ((count >> (allocated_lgcount - 3)) == 0)) {
    do {
      allocated_lgcount -= 1;
    } while ((allocated_lgcount >= 3) && ((count >> (allocated_lgcount - 3)) == 0));

    at = realloc_fn(at, element_size << (allocated_lgcount - 1));
    if (at) {*p_at = at;}
    *p_allocated_lgcount = allocated_lgcount;
  }
}

#endif
