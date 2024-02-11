#ifndef FILDESH_SYMVAL_H_
#define FILDESH_SYMVAL_H_
#include <fildesh/fildesh.h>

enum SymValKind
{
  IDescVal, ODescVal, IODescVal,
  ODescStatusVal,
  IDescArgVal,
  IDescFileVal, ODescFileVal,
  IFutureDescVal, OFutureDescVal,
  IFutureDescFileVal, OFutureDescFileVal,
  HereDocVal, IHereDocFileVal,
  DefVal,
  IOFileVal,
  TmpFileVal,
  NSymValKinds
};

typedef enum SymValKind SymValKind;
typedef struct SymVal SymVal;

struct SymVal
{
  SymValKind kind;
  unsigned arg_idx;  /**< If a file.**/
  unsigned ios_idx;
  unsigned cmd_idx;
  union SymVal_union
  {
    int file_desc;
    char* here_doc;
    const char* iofilename;  /* IOFileVal only.*/
  } as;
};

SymVal*
getf_fildesh_SymVal(FildeshKV* map, const char* s);
SymVal*
declare_fildesh_SymVal(FildeshKV* map, SymValKind kind, const char* name);
SymValKind
peek_fildesh_SymVal_arg(const char* s, bool firstarg);
SymValKind
parse_fildesh_SymVal_arg(char* s, bool firstarg);

#endif
