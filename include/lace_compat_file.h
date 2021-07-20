#ifndef LACE_COMPAT_FILE_H_
#define LACE_COMPAT_FILE_H_

const char*
lace_compat_file_basename(const char* filepath);
char*
lace_compat_file_abspath(const char* filepath);
char*
lace_compat_file_catpath(const char* dir, const char* filename);

int
lace_compat_file_chmod_u_rwx(const char* filename, int r, int w, int x);

#endif
