#ifndef FILDESH_H_
#define FILDESH_H_
#include <limits.h>
#include <stddef.h>
#include <stdint.h>

#if defined(__cplusplus) || defined(__OPENCL_VERSION__)
/* These already have bool.*/
#elif defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901L)
#include <stdbool.h>
#else
# if !defined(true)
#  define true 1
#  define false 0
typedef char bool;
#  define bool bool
# endif
# if !defined(inline)
#  define inline __inline
# endif
#endif

typedef int fildesh_fd_t;
typedef uint8_t fildesh_lgsize_t;
#define FILDESH_LGSIZE_MAX UCHAR_MAX

#ifdef __cplusplus
extern "C" {
#endif

typedef struct FildeshX_VTable FildeshX_VTable;
typedef struct FildeshX FildeshX;
typedef struct FildeshO_VTable FildeshO_VTable;
typedef struct FildeshO FildeshO;
typedef struct FildeshAlloc FildeshAlloc;
typedef struct FildeshKV FildeshKV;
typedef struct FildeshKVE FildeshKVE;
typedef struct FildeshA FildeshA;


struct FildeshX {
  char* at;
  size_t size;
  size_t off;
  fildesh_lgsize_t alloc_lgsize;
  fildesh_lgsize_t flush_lgsize;
  const FildeshX_VTable* vt;
};
#define DEFAULT_FildeshX  { NULL, 0, 0, 0, 12, NULL }
#define DEFAULT1_FildeshX(vt)  { NULL, 0, 0, 0, 12, vt }


size_t read_FildeshX(FildeshX*);
void close_FildeshX(FildeshX*);
char* grow_FildeshX(FildeshX*, size_t);
void flush_FildeshX(FildeshX*);
void maybe_flush_FildeshX(FildeshX*);
char* slurp_FildeshX(FildeshX*);
void wait_close_FildeshX(FildeshX*);
FildeshX until_char_FildeshX(FildeshX*, char delim);
FildeshX until_bytestring_FildeshX(FildeshX*, const unsigned char*, size_t);
FildeshX until_chars_FildeshX(FildeshX*, const char* delims);
FildeshX while_chars_FildeshX(FildeshX*, const char* span);
bool peek_bytestring_FildeshX(FildeshX*, const unsigned char*, size_t);
bool skip_bytestring_FildeshX(FildeshX*, const unsigned char*, size_t);
FildeshX slicechr_FildeshX(FildeshX*, char delim);
FildeshX sliceline_FildeshX(FildeshX*);
FildeshX slicestr_FildeshX(FildeshX*, const char* delim);
char* getline_FildeshX(FildeshX*);
char* gets_FildeshX(FildeshX*, const char* delim);
bool skipchrs_FildeshX(FildeshX*, const char* span);
bool skipstr_FildeshX(FildeshX*, const char* s);
bool parse_int_FildeshX(FildeshX*, int*);
bool parse_double_FildeshX(FildeshX*, double*);

FildeshX* open_FildeshXA();

fildesh_fd_t fildesh_arg_open_readonly(const char*);
FildeshX* open_FildeshXF(const char* filename);
FildeshX* open_sibling_FildeshXF(const char* sibling, const char* filename);
FildeshX* open_fd_FildeshX(fildesh_fd_t fd);
FildeshX* open_arg_FildeshXF(unsigned argi, char** argv, FildeshX** inputv);

const char* filename_FildeshXF(FildeshX*);

size_t write_FildeshO(FildeshO*);
void close_FildeshO(FildeshO*);
char* grow_FildeshO(FildeshO*, size_t);
void flush_FildeshO(FildeshO*);
void maybe_flush_FildeshO(FildeshO*);
void put_bytestring_FildeshO(FildeshO*, const unsigned char*, size_t);
void putc_FildeshO(FildeshO*, char);
void puts_FildeshO(FildeshO*, const char*);
void print_int_FildeshO(FildeshO*, int);
void print_double_FildeshO(FildeshO*, double);

fildesh_fd_t fildesh_arg_open_writeonly(const char*);
FildeshO* open_FildeshOF(const char* filename);
FildeshO* open_sibling_FildeshOF(const char* sibling, const char* filename);
FildeshO* open_fd_FildeshO(fildesh_fd_t fd);
FildeshO* open_arg_FildeshOF(unsigned argi, char** argv, FildeshO** outputv);

const char* filename_FildeshOF(FildeshO*);


char* fildesh_parse_int(int* ret, const char* in);
char* fildesh_parse_double(double* ret, const char* in);
#define FILDESH_INT_BASE10_SIZE_MAX (1 + (unsigned)(CHAR_BIT*sizeof(int)) / 3 + 1)
unsigned fildesh_encode_int_base10(char*, fildesh_fd_t);
#define FILDESH_FD_PATH_SIZE_MAX (8+FILDESH_INT_BASE10_SIZE_MAX)
unsigned fildesh_encode_fd_path(char*, fildesh_fd_t);


void fildesh_log_errorf(const char*, ...);
void fildesh_log_warningf(const char*, ...);
void fildesh_log_tracef(const char*, ...);
void fildesh_log_error_(
    const char* file, const char* func, unsigned line, const char* msg);
void fildesh_log_warning_(
    const char* file, const char* func, unsigned line, const char* msg);
void fildesh_log_trace_(
    const char* file, const char* func, unsigned line, const char* msg);
#ifndef _MSC_VER
#define fildesh_log_error(s)  fildesh_log_error_(__FILE__,__extension__ __func__,__LINE__, s)
#define fildesh_log_warning(s)  fildesh_log_warning_(__FILE__,__extension__ __func__,__LINE__, s)
#define fildesh_log_trace(s)  fildesh_log_trace_(__FILE__,__extension__ __func__,__LINE__, s)
#else
#define fildesh_log_error(s)  fildesh_log_error_(__FILE__,__FUNCTION__,__LINE__,s)
#define fildesh_log_warning(s)  fildesh_log_warning_(__FILE__,__FUNCTION__,__LINE__,s)
#define fildesh_log_trace(s)  fildesh_log_trace_(__FILE__,__FUNCTION__,__LINE__,s)
#endif


/** Given the memory address of a structure's field,
 * get the address of the structure.
 * \param T      Type.
 * \param field  Name of the field.
 * \param p      Memory address of the field.
 *
 * fildesh_castup( T, field, p ) == container_of(p, T, field)
 **/
#define fildesh_castup( T, field, p ) \
  ((T*) ((uintptr_t) (p) - (ptrdiff_t) offsetof(T, field)))

#if defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 201112L)
# define fildesh_alignof(T)  _Alignof(T)
#elif defined(__cplusplus) && (__cplusplus >= 201103L)
# define fildesh_alignof(T)  alignof(T)
#elif defined(_MSC_VER)
# define fildesh_alignof(T)  __alignof(T)
#else
# define fildesh_alignof(T) ((size_t) offsetof(struct { char a; T b; }, b))
#endif


struct FildeshX_VTable
{
  void (*read_fn)(FildeshX*);
  void (*close_fn)(FildeshX*);
  void (*free_fn)(FildeshX*);
};
#define DEFINE_FildeshX_VTable(T, field) \
  static void read_##T##_FildeshX(FildeshX* x) { \
    read_##T(fildesh_castup(T, field, x)); \
  } \
  static void close_##T##_FildeshX(FildeshX* x) { \
    close_##T(fildesh_castup(T, field, x)); \
  } \
  static void free_##T##_FildeshX(FildeshX* x) { \
    free_##T(fildesh_castup(T, field, x)); \
  } \
  static const FildeshX_VTable DEFAULT_##T##_FildeshX_VTable[1] = {{ \
    read_##T##_FildeshX, \
    close_##T##_FildeshX, \
    free_##T##_FildeshX, \
  }}

struct FildeshO_VTable
{
  void (*write_fn)(FildeshO*);
  void (*close_fn)(FildeshO*);
  void (*free_fn)(FildeshO*);
};
#define DEFINE_FildeshO_VTable(T, field) \
  static void write_##T##_FildeshO(FildeshO* o) { \
    write_##T(fildesh_castup(T, field, o)); \
  } \
  static void close_##T##_FildeshO(FildeshO* o) { \
    close_##T(fildesh_castup(T, field, o)); \
  } \
  static void free_##T##_FildeshO(FildeshO* o) { \
    free_##T(fildesh_castup(T, field, o)); \
  } \
  static const FildeshO_VTable DEFAULT_##T##_FildeshO_VTable[1] = {{ \
    write_##T##_FildeshO, \
    close_##T##_FildeshO, \
    free_##T##_FildeshO, \
  }}

struct FildeshO {
  char* at;
  size_t size;
  size_t off;
  fildesh_lgsize_t alloc_lgsize;
  fildesh_lgsize_t flush_lgsize;
  const FildeshO_VTable* vt;
};
#define DEFAULT_FildeshO  { NULL, 0, 0, 0, 12, NULL }
#define DEFAULT1_FildeshO(vt)  { NULL, 0, 0, 0, 12, vt }

#define FILDESH_ALLOC_MIN_BLOCK_LGSIZE 12
#define FILDESH_ALLOC_MAX_BLOCKS \
  (sizeof(size_t)*CHAR_BIT - (size_t)FILDESH_ALLOC_MIN_BLOCK_LGSIZE-1)

struct FildeshAlloc {
  fildesh_lgsize_t block_count;
  char* blocks[FILDESH_ALLOC_MAX_BLOCKS];
  size_t sizes[FILDESH_ALLOC_MAX_BLOCKS];
};

FildeshAlloc* open_FildeshAlloc();
void close_FildeshAlloc(FildeshAlloc*);
void* reserve_FildeshAlloc(FildeshAlloc*, size_t size, size_t alignment);
char* strdup_FildeshAlloc(FildeshAlloc*, const char*);
char* strdup_FildeshX(const FildeshX*, FildeshAlloc*);
char* strdup_FildeshO(const FildeshO*, FildeshAlloc*);

struct FildeshKV {
  FildeshKVE* at;
  size_t freelist_head;
  fildesh_lgsize_t allocated_lgcount;
};
typedef size_t FildeshKV_id_t;
#define DEFAULT_FildeshKV_SINGLE_LIST  { NULL, 0, 0 }

FildeshKV_id_t lookup_FildeshKV(const FildeshKV*, const void*, FildeshKV_id_t);
FildeshKV_id_t any_id_FildeshKV(const FildeshKV*);
void* lookup_value_FildeshKV(FildeshKV*, const void*, size_t);
FildeshKV_id_t ensure_FildeshKV(FildeshKV*, const void*, size_t);
size_t size_of_key_at_FildeshKV(const FildeshKV*, FildeshKV_id_t);
const void* key_at_FildeshKV(const FildeshKV*, FildeshKV_id_t);
const void* value_at_FildeshKV(const FildeshKV*, FildeshKV_id_t);
void assign_at_FildeshKV(FildeshKV*, FildeshKV_id_t, const void*, size_t);
void remove_at_FildeshKV(FildeshKV*, FildeshKV_id_t);
void close_FildeshKV(FildeshKV*);


struct FildeshA {
  void* at;
  size_t count;
  fildesh_lgsize_t allocated_lgcount;
};
#define DECLARE_FildeshAT(T, name) \
  T* name[3]

void close_FildeshAT(void*);
void clear_FildeshAT(void*);
void*
realloc_more_FildeshA_(void* at, fildesh_lgsize_t* p_allocated_lgcount,
                       const size_t element_size, const size_t count);
void*
realloc_less_FildeshA_(void* at, fildesh_lgsize_t* p_allocated_lgcount,
                       const size_t element_size, const size_t count);


/* Inlines.*/
static inline FildeshX default_FildeshX() {FildeshX tmp = DEFAULT_FildeshX; return tmp;}
static inline FildeshO default_FildeshO() {FildeshO tmp = DEFAULT_FildeshO; return tmp;}
static inline void truncate_FildeshX(FildeshX* x) {x->off = 0; x->size = 0;}
static inline void truncate_FildeshO(FildeshO* o) {o->off = 0; o->size = 0;}
static inline bool peek_char_FildeshX(FildeshX* in, char guess) {
  if (in->off < in->size) {return (in->at[in->off] == guess);}
  if (0 < read_FildeshX(in)) {return (in->at[in->off] == guess);}
  return false;
}
static inline bool peek_byte_FildeshX(FildeshX* in, unsigned char guess) {
  return peek_char_FildeshX(in, (char)guess);
}
static inline FildeshX until_byte_FildeshX(FildeshX* in, unsigned char delim) {
  return until_char_FildeshX(in, (char)delim);
}

#define fildesh_allocate(T, n, alloc) \
  ((T*) reserve_FildeshAlloc(alloc, (n)*sizeof(T), fildesh_alignof(T)))

#define fildesh_nullid(id)  (!~(id))

static inline
  size_t
fildesh_size_of_lgcount(size_t size, fildesh_lgsize_t lgcount) {
  if (lgcount == 0) {return 0;}
  return size << (lgcount - 1);
}

/* Dynamic array inlines take up the rest of this file.*/

static inline void init_FildeshAT(void* p) {
  void** p_at = (void**) p;
  p_at[0] = NULL;
  ((uintptr_t*)p_at)[1] = 0;
  ((uintptr_t*)p_at)[2] = 0;
}
static inline size_t count_of_FildeshAT(const void* p) {
  return fildesh_castup(FildeshA, at, p)->count;
}
static inline size_t allocated_count_of_FildeshAT(const void* p) {
  return fildesh_size_of_lgcount(
      1, fildesh_castup(FildeshA, at, p)->allocated_lgcount);
}
#define grow_FildeshAT(a, difference)  grow_FildeshAT_(a, sizeof(**a), difference)
#define mpop_FildeshAT(a, difference)  mpop_FildeshAT_(a, sizeof(**a), difference)
#define resize_FildeshAT(a, n)  resize_FildeshAT_(a, sizeof(**a), n)
#define last_FildeshAT(a)  (*(a))[count_of_FildeshAT(a)-1]
#define grow1_FildeshAT(a) (grow_FildeshAT(a, 1), &(*(a))[count_of_FildeshAT(a)-1])
#define push_FildeshAT(a, e)  do { \
  grow_FildeshAT(a, 1); \
  last_FildeshAT(a) = e; \
} while (0)

static inline
  void*
grow_FildeshAT_(void* p, size_t element_size, size_t difference)
{
  FildeshA* const a = fildesh_castup(FildeshA, at, p);
  const size_t count = a->count + difference;
  if ((count << 1) > ((size_t)1 << a->allocated_lgcount)) {
    void* at = realloc_more_FildeshA_(
        a->at, &a->allocated_lgcount, element_size, count);
    if (!at) {return NULL;}
    a->at = at;
  }
  a->count = count;
  return (void*)((uintptr_t)a->at + (count - difference) * element_size);
}

static inline
  void
mpop_FildeshAT_(void* p, size_t element_size, size_t difference)
{
  FildeshA* const a = fildesh_castup(FildeshA, at, p);
  a->count -= difference;
  if ((a->allocated_lgcount >= 3) && ((a->count >> (a->allocated_lgcount - 3)) == 0)) {
    a->at = realloc_less_FildeshA_(
        a->at, &a->allocated_lgcount, element_size, a->count);
  }
}

static inline
  void
resize_FildeshAT_(void* p, size_t element_size, size_t n)
{
  FildeshA* const a = fildesh_castup(FildeshA, at, p);
  if (n >= a->count) {
    grow_FildeshAT_(p, element_size, n - a->count);
  }
  else {
    mpop_FildeshAT_(p, element_size, a->count - n);
  }
}

static inline
  void*
grow_FildeshA_(void** p_at, size_t* p_count,
               fildesh_lgsize_t* p_allocated_lgcount,
               size_t element_size, size_t difference)
{
  void* at = *p_at;
  const size_t count = *p_count + difference;
  if ((count << 1) > ((size_t)1 << *p_allocated_lgcount)) {
    at = realloc_more_FildeshA_(at, p_allocated_lgcount, element_size, count);
    if (!at) {return NULL;}
    *p_at = at;
  }
  *p_count = count;
  return (void*)((uintptr_t)at + (count - difference) * element_size);
}

static inline
  void
mpop_FildeshA_(void** p_at, size_t* p_count,
               fildesh_lgsize_t* p_allocated_lgcount,
               size_t element_size, size_t difference)
{
  const size_t count = *p_count - difference;
  const fildesh_lgsize_t allocated_lgcount = *p_allocated_lgcount;

  *p_count = count;
  if ((allocated_lgcount >= 3) && ((count >> (allocated_lgcount - 3)) == 0)) {
    *p_at = realloc_less_FildeshA_(
        *p_at, p_allocated_lgcount, element_size, count);
  }
}

#ifdef __cplusplus
} /* extern "C" */
#endif
#endif