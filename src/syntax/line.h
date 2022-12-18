#ifndef _FILDESH_SYNTAX_LINE_H_
#define _FILDESH_SYNTAX_LINE_H_
#include <fildesh/fildesh.h>

char*
fildesh_syntax_parse_line(FildeshX* xf, size_t* text_nlines,
                          FildeshAlloc* alloc, FildeshO* tmp_out);
char*
fildesh_syntax_maybe_concatenate_args(
    unsigned argc, const char* const* argv,
    FildeshAlloc* alloc);
char*
fildesh_syntax_parse_here_doc(
    FildeshX* in, const char* term, size_t* text_nlines,
    FildeshAlloc* alloc, FildeshO* tmp_out);
const char*
fildesh_syntax_sep_line(
    char*** args, char* s, FildeshKV* map,
    FildeshAlloc* alloc, FildeshO* tmp_out);

#endif
