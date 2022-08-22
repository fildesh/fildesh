#ifndef _PARSE_FILDESH_H_
#define _PARSE_FILDESH_H_
#include "fildesh.h"

char*
fildesh_syntax_parse_line(FildeshX* xf, size_t* text_nlines);
char*
fildesh_syntax_maybe_concatenate_args(
    unsigned argc, const char* const* argv,
    FildeshAlloc* alloc);
char*
fildesh_syntax_parse_here_doc(
    FildeshX* in, const char* term, size_t* text_nlines,
    FildeshAlloc* alloc, FildeshO* tmp_out);

#endif
