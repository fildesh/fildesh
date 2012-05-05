
#ifndef FileB_H_
#define FileB_H_

#include "table.h"

#include <stdio.h>

#ifndef Table_byte
#define Table_byte Table_byte
DeclTableT( byte, byte );
#endif

#ifndef Table_char
#define Table_char Table_char
DeclTableT( char, char );
#endif

#ifndef FileB
#define FileB FileB
typedef struct FileB FileB;
#endif

enum FileB_Format {
    FileB_Ascii,
    FileB_Raw,
    FileB_NFormats
};
typedef enum FileB_Format FileB_Format;

struct FileB
{
    FILE* f;
    TableT(byte) buf;
    TableSzT_byte off;
    bool good;
    bool sink;
    bool byline;
    FileB_Format fmt;
    TableT(char) pathname;
    TableT(char) filename;
};

static const char WhiteSpaceChars[] = " \t\v\r\n";


void
init_FileB (FileB* f);
void
close_FileB (FileB* f);
void
lose_FileB (FileB* f);
void
seto_FileB (FileB* f, bool sink);
byte*
ensure_FileB (FileB* f, TableSzT_byte);
void
setfmt_FileB (FileB* f, FileB_Format fmt);
bool
open_FileB (FileB* f, const char* pathname, const char* filename);
void
olay_FileB (FileB* olay, FileB* source);
char*
load_FileB (FileB* f);
void
flushx_FileB (FileB* f);
char*
getline_FileB (FileB* in);
char*
getlined_FileB (FileB* in, const char* delim);
void
skipds_FileB (FileB* in, const char* delims);
char*
nextds_FileB (FileB* in, char* ret_match, const char* delims);
char*
nextok_FileB (FileB* in, char* ret_match, const char* delims);
void
inject_FileB (FileB* in, FileB* src, const char* delim);
void
skipto_FileB (FileB* in, const char* pos);

bool
flusho_FileB (FileB* f);
void
dump_uint_FileB (FileB* f, uint x);
void
dump_real_FileB (FileB* f, real x);
void
dump_char_FileB (FileB* f, char c);
void
dump_cstr_FileB (FileB* f, const char* s);

void
dumpn_byte_FileB (FileB* f, const byte* a, TableSzT_byte n);
void
dumpn_char_FileB (FileB* f, const char* a, TableSzT_byte n);

char*
load_uint_cstr (uint* ret, const char* in);
char*
load_real_cstr (real* ret, const char* in);

bool
load_uint_FileB (FileB* f, uint* x);
bool
load_real_FileB (FileB* f, real* x);

bool
loadn_byte_FileB (FileB* f, byte* a, TableSzT_byte n);

FileB*
stdout_FileB ();


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

qual_inline
    char*
cstr_FileB (FileB* f)
{
    return (char*) f->buf.s;
}

#ifdef IncludeC
#include "fileb.c"
#endif
#endif

