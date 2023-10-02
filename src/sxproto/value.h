#include <fildesh/sxproto.h>

struct FildeshSxprotoValue {
  const char* text;
  FildeshSxprotoFieldKind field_kind;
  FildeshSxpb_id elem;
  FildeshSxpb_id next;
};
#define DEFAULT_FildeshSxprotoValue \
{ NULL, FildeshSxprotoFieldKind_UNKNOWN, \
  ~(FildeshSxpb_id)0, ~(FildeshSxpb_id)0, \
}

