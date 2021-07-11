#ifndef LACE_COMPAT_FILE_H_
#define LACE_COMPAT_FILE_H_

const char*
lace_compat_file_basename(const char* filepath);
char*
lace_compat_file_abspath(const char* filepath);
char*
lace_compat_file_catpath(const char* dir, const char* filename);

#endif
