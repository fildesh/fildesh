
#ifndef FUTIL_H_
#define FUTIL_H_

#include "table.h"

#include <stdio.h>

#ifndef DeclTableT_char
#define DeclTableT_char
DeclTableT( char );
#endif

static const char WhiteSpaceChars[] = " \t\v\r\n";

char*
read_FILE (FILE* in);
uint
getline_FILE (FILE* in, Table(char)* line, uint off);

#endif

