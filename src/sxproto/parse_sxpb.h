#include "src/sxproto/value.h"

typedef struct FildeshSxpbInfo FildeshSxpbInfo;

struct FildeshSxpbInfo {
  bool unquoted_value_separation_on;
  unsigned line_count;
  unsigned column_count;
  FildeshO* err_out;
};
#define DEFAULT_FildeshSxpbInfo \
{ false, 0, 0, NULL }


bool
parse_concat_string_FildeshSxpbInfo(
    FildeshSxpbInfo* info,
    FildeshX* in,
    FildeshO* oslice);
bool
parse_number_FildeshSxpbInfo(
    FildeshSxpbInfo* info,
    FildeshX* in,
    FildeshO* oslice);
bool
parse_name_FildeshSxpbInfo(
    FildeshSxpbInfo* info,
    FildeshX* in,
    FildeshO* oslice,
    unsigned* ret_nesting_depth,
    FildeshSxprotoFieldKind parent_kind);
bool
parse_field_FildeshSxpbInfo(
    FildeshSxpbInfo* info,
    const FildeshSxprotoField* schema,
    FildeshX* in,
    FildeshSxpb* sxpb,
    FildeshSxpbIT a,
    FildeshO* oslice);
bool
parse_field_content_FildeshSxpbInfo(
    FildeshSxpbInfo* info,
    FildeshX* in,
    FildeshSxpb* sxpb,
    FildeshSxpbIT p_it,
    const FildeshSxprotoField* field,
    FildeshSxprotoFieldKind elem_kind,
    bool nest_discriminator_on,
    FildeshO* oslice);
