#ifndef FILDESH_SYNTAX_DEFSTR_H_
#define FILDESH_SYNTAX_DEFSTR_H_
#include <fildesh/fildesh.h>

const char*
parse_double_quoted_fildesh_string(FildeshX* in, FildeshO* out, FildeshKV* map);
const char*
parse_fildesh_string_definition(
    FildeshX* in,
    size_t* text_nlines,
    FildeshKV* map,
    FildeshAlloc* alloc,
    FildeshO* tmp_out);

#endif
