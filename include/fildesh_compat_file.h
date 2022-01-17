#ifndef FILDESH_COMPAT_FILE_H_
#define FILDESH_COMPAT_FILE_H_

const char*
fildesh_compat_file_basename(const char* filepath);
char*
fildesh_compat_file_abspath(const char* filepath);
char*
fildesh_compat_file_catpath(const char* dir, const char* filename);

int
fildesh_compat_file_chmod_u_rwx(const char* filename, int r, int w, int x);
int
fildesh_compat_file_rm(const char*);
int
fildesh_compat_file_rmdir(const char*);

#endif
