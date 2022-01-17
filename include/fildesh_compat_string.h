#ifndef FILDESH_COMPAT_STRING_H_
#define FILDESH_COMPAT_STRING_H_

static const char fildesh_compat_string_blank_bytes[] = " \t\v\r\n";

char*
fildesh_compat_string_byte_translate(
    const char* haystack,
    const char* needles,
    const char* const* replacements,
    const char* lhs, const char* rhs);
char*
fildesh_compat_string_duplicate(const char*);

#endif
