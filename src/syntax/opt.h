#ifndef FILDESH_SYNTAX_OPT_H_
#define FILDESH_SYNTAX_OPT_H_
#include <fildesh/fildesh.h>

int
fildesh_syntax_parse_flags(
    char** args, FildeshKV* map,
    const char*** ret_stdargs, FildeshO* tmp_out);

#endif
