#ifndef LACE_H_
#define LACE_H_
#include "fildesh.h"

typedef fildesh_fd_t lace_fd_t;

#ifdef __cplusplus
extern "C" {
#endif

typedef FildeshX_VTable LaceX_VTable;
typedef FildeshX LaceX;
typedef FildeshO_VTable LaceO_VTable;
typedef FildeshO LaceO;


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

#ifdef __cplusplus
} /* extern "C" */
#endif
#endif
