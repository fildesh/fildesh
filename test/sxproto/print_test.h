#ifndef FILDESH_TEST_SXPROTO_PRINT_TEST_H_
#define FILDESH_TEST_SXPROTO_PRINT_TEST_H_
#include <fildesh/fildesh.h>

bool
check_same_printed_format(
    FildeshO* err_out,
    const char* name,
    const char* format,
    const unsigned char* expect,
    size_t expect_size,
    const unsigned char* actual,
    size_t actual_size);

void check_same_printed_format_test(void);

#endif
