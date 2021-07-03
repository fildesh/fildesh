
#ifndef FileB_H_
#define FileB_H_

#include "syscx.h"
#include "xfile.h"
#include "ofile.h"

#include <stdarg.h>
#include <stdio.h>

extern const XFileVT FileB_XFileVT;
extern const OFileVT FileB_OFileVT;

typedef struct FileB FileB;
typedef struct XFileB XFileB;
typedef struct OFileB OFileB;

enum FileB_Format {
  FileB_Ascii,
  FileB_Raw,
  FileB_NFormats
};
typedef enum FileB_Format FileB_Format;

struct FileB {
  FILE* f;
  fd_t fd;
  bool good;
  bool sink;
  bool byline;
  FileB_Format fmt;
  AlphaTab pathname;
  AlphaTab filename;
};
#define DEFAULT1_FileB(sink) \
{ \
  0, -1, true, \
  sink, false, FileB_Ascii, \
  DEFAULT_AlphaTab, DEFAULT_AlphaTab \
}

struct XFileB
{
  XFile xf;
  FileB fb;
};
#define DEFAULT_XFileB \
{ \
  DEFAULT3_XFile(BUFSIZ, true, &FileB_XFileVT), \
  DEFAULT1_FileB(false) \
}

struct OFileB
{
  OFile of;
  FileB fb;
};
#define DEFAULT_OFileB \
{ \
  DEFAULT3_OFile(BUFSIZ, true, &FileB_OFileVT), \
  DEFAULT1_FileB(true) \
}

void
init_XFileB (XFileB* xfb);
void
init_OFileB (OFileB* ofb);

void
close_XFileB (XFileB* f);
void
close_OFileB (OFileB* f);
void
lose_XFileB (XFileB* xfb);
void
lose_OFileB (OFileB* ofb);

uint
pathname2_AlphaTab (AlphaTab* pathname, const char* opt_dir, const char* filename);
bool
open_FileB (FileB* f, const char* pathname, const char* filename);
bool
openfd_FileB (FileB* fb, fd_t fd);
void
set_FILE_FileB (FileB* fb, FILE* file);
char*
xget_XFileB (XFileB* xfb);

void
oputn_byte_OFileB (OFileB* f, const byte* a, zuint n);

qual_inline
  bool
nullt_FileB (const FileB* f)
{
  return (f->fmt < FileB_Raw);
}

qual_inline
  bool
byline_FileB (const FileB* f)
{
  return f->byline;
}


/* Implemented in syscx.c */
void
mktmppath_sysCx (AlphaTab* path);

#endif

