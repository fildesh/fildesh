#ifndef LACE_H_
#define LACE_H_
#include <limits.h>
#include <stddef.h>
#include <stdint.h>
#include "fildesh.h"

typedef fildesh_fd_t lace_fd_t;
typedef fildesh_lgsize_t lace_lgsize_t;
#define LACE_LGSIZE_MAX FILDESH_LGSIZE_MAX

#ifdef __cplusplus
extern "C" {
#endif

typedef FildeshX_VTable LaceX_VTable;
typedef FildeshX LaceX;
typedef FildeshO_VTable LaceO_VTable;
typedef FildeshO LaceO;
typedef FildeshKV LaceKV;
typedef FildeshKVE LaceKVE;


#define DEFAULT_LaceX  DEFAULT_FildeshX
#define DEFAULT1_LaceX  DEFAULT1_FildeshX


size_t read_LaceX(LaceX*);
void close_LaceX(LaceX*);
char* grow_LaceX(LaceX*, size_t);
void flush_LaceX(LaceX*);
void maybe_flush_LaceX(LaceX*);
char* slurp_LaceX(LaceX*);
void wait_close_LaceX(LaceX*);
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
#define LACE_INT_BASE10_SIZE_MAX FILDESH_INT_BASE10_SIZE_MAX
unsigned lace_encode_int_base10(char*, lace_fd_t);
#define LACE_FD_PATH_SIZE_MAX FILDESH_FD_PATH_SIZE_MAX
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


#define lace_castup fildesh_castup

#define DEFAULT_LaceO DEFAULT_FildeshO
#define DEFAULT1_LaceO DEFAULT1_FildeshO

static inline LaceX default_LaceX() {LaceX tmp = DEFAULT_LaceX; return tmp;}
static inline LaceO default_LaceO() {LaceO tmp = DEFAULT_LaceO; return tmp;}

#ifdef __cplusplus
} /* extern "C" */
#endif
#endif
