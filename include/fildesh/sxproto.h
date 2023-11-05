#include <fildesh/fildesh.h>

BEGIN_EXTERN_C

typedef size_t FildeshSxpb_id;
typedef struct FildeshSxpb FildeshSxpb;
typedef struct FildeshSxpbIT FildeshSxpbIT;
typedef struct FildeshSxprotoValue FildeshSxprotoValue;
typedef struct FildeshSxprotoField FildeshSxprotoField;

enum FildeshSxprotoFieldKind {
  FildeshSxprotoFieldKind_UNKNOWN,
  FildeshSxprotoFieldKind_MESSAGE,
  FildeshSxprotoFieldKind_LITERAL,
  FildeshSxprotoFieldKind_ARRAY,
  FildeshSxprotoFieldKind_MANYOF,
  FildeshSxprotoFieldKind_LITERAL_STRING,
  FildeshSxprotoFieldKind_LITERAL_BOOL,
  FildeshSxprotoFieldKind_LITERAL_INT,
  FildeshSxprotoFieldKind_LITERAL_FLOAT
};
typedef enum FildeshSxprotoFieldKind FildeshSxprotoFieldKind;

struct FildeshSxpb {
  FildeshKV name_by_name[1];
  DECLARE_FildeshAT(FildeshSxprotoValue, values);
};

struct FildeshSxprotoField {
  const char* name;
  FildeshSxprotoFieldKind kind;
  intptr_t lo;
  uintptr_t hi;
};
#define FILL_DEFAULT_FildeshSxprotoField_STRING \
  FildeshSxprotoFieldKind_LITERAL_STRING, 0, 0
#define FILL_DEFAULT_FildeshSxprotoField_BOOL \
  FildeshSxprotoFieldKind_LITERAL_BOOL, 0, 0
#define FILL_DEFAULT_FildeshSxprotoField_FLOAT \
  FildeshSxprotoFieldKind_LITERAL_FLOAT, 0, 0

#define FILL_DEFAULT_FildeshSxprotoField_STRINGS \
  FildeshSxprotoFieldKind_ARRAY, 0, \
  (size_t)FildeshSxprotoFieldKind_LITERAL_STRING
#define FILL_DEFAULT_FildeshSxprotoField_INTS \
  FildeshSxprotoFieldKind_ARRAY, 0, \
  (size_t)FildeshSxprotoFieldKind_LITERAL_INT
#define FILL_DEFAULT_FildeshSxprotoField_FLOATS \
  FildeshSxprotoFieldKind_ARRAY, 0, \
  (size_t)FildeshSxprotoFieldKind_LITERAL_FLOAT

#define FILL_UNKNOWN_FildeshSxprotoField_MESSAGE \
  FildeshSxprotoFieldKind_MESSAGE, 0, (uintptr_t)NULL
#define FILL_UNKNOWN_FildeshSxprotoField_MANYOF \
  FildeshSxprotoFieldKind_MANYOF, 0, (uintptr_t)NULL
#define recurse_unknowns_FildeshSxprotoField_MESSAGE(subfields) \
  recurse_unknowns_FildeshSxprotoField_(\
      subfields, sizeof(subfields)/sizeof(FildeshSxprotoField),\
      FildeshSxprotoFieldKind_MESSAGE)
#define recurse_unknowns_FildeshSxprotoField_MANYOF(subfields) \
  recurse_unknowns_FildeshSxprotoField_(\
      subfields, sizeof(subfields)/sizeof(FildeshSxprotoField),\
      FildeshSxprotoFieldKind_MANYOF)
static inline void recurse_unknowns_FildeshSxprotoField_(
    FildeshSxprotoField* subfields, unsigned n, FildeshSxprotoFieldKind kind) {
  unsigned i;
  for (i = 0; i < n; ++i) {
    if (subfields[i].kind == kind && subfields[i].lo == 0) {
      subfields[i].lo = n;
      subfields[i].hi = (uintptr_t)subfields;
    }
  }
}


#define FILL_FildeshSxprotoField_MESSAGE(subfields) \
  FildeshSxprotoFieldKind_MESSAGE, \
  sizeof(subfields)/sizeof(FildeshSxprotoField), \
  (uintptr_t)subfields
#define FILL_FildeshSxprotoField_MESSAGES(subfields) \
  FildeshSxprotoFieldKind_ARRAY, \
  sizeof(subfields)/sizeof(FildeshSxprotoField), \
  (uintptr_t)subfields
#define FILL_FildeshSxprotoField_MANYOF(subfields) \
  FildeshSxprotoFieldKind_MANYOF, \
  sizeof(subfields)/sizeof(FildeshSxprotoField), \
  (uintptr_t)subfields
#define FILL_FildeshSxprotoField_INT(lo, hi) \
  FildeshSxprotoFieldKind_LITERAL_INT, lo, hi
#define FILL_FildeshSxprotoField_FLOAT(lo, hi) \
  FildeshSxprotoFieldKind_LITERAL_FLOAT, lo, hi
#define FILL_FildeshSxprotoField_STRING(lo, hi) \
  FildeshSxprotoFieldKind_LITERAL_STRING, lo, hi
#define FILL_FildeshSxprotoField_ALIAS(name) \
  FildeshSxprotoFieldKind_UNKNOWN, 0, (uintptr_t)name


struct FildeshSxpbIT {
  FildeshSxprotoFieldKind field_kind;
  FildeshSxpb_id cons_id;
  FildeshSxpb_id elem_id;
};
#define DEFAULT_FildeshSxpbIT \
{ FildeshSxprotoFieldKind_MESSAGE, ~(FildeshSxpb_id)0, ~(FildeshSxpb_id)0 }

FildeshSxpb*
open_FildeshSxpb();
void
close_FildeshSxpb(FildeshSxpb*);

FildeshSxpb*
slurp_sxpb_close_FildeshX(
    FildeshX* in,
    const FildeshSxprotoField* schema,
    FildeshO* err_out);
void
print_json_FildeshO(FildeshO* out, FildeshSxpb* sxpb);
void
print_txtpb_FildeshO(FildeshO* out, FildeshSxpb* sxpb);
/* Leaves output file open.*/
/* Deprecated. Use print_txtpb_FildeshO().*/
bool sxproto2textproto(FildeshX* in, FildeshO* out, FildeshO* err_out);

const char*
ensure_name_FildeshSxpb(FildeshSxpb* sxpb, const char* s, size_t n);

FildeshSxpbIT
lookup_subfield_at_FildeshSxpb(
    const FildeshSxpb* sxpb,
    FildeshSxpbIT m,
    const char* k);
FildeshSxpbIT
first_at_FildeshSxpb(const FildeshSxpb* sxpb, FildeshSxpbIT it);
FildeshSxpbIT
next_at_FildeshSxpb(const FildeshSxpb* sxpb, FildeshSxpbIT it);
void
remove_at_FildeshSxpb(FildeshSxpb* sxpb, FildeshSxpbIT it);

const char*
name_at_FildeshSxpb(const FildeshSxpb* sxpb, FildeshSxpbIT it);
const FildeshSxprotoField*
subfield_of_FildeshSxprotoField(const FildeshSxprotoField* schema, const char* name);


bool
bool_value_at_FildeshSxpb(const FildeshSxpb* sxpb, FildeshSxpbIT it);
unsigned
unsigned_value_at_FildeshSxpb(const FildeshSxpb* sxpb, FildeshSxpbIT it);
float
float_value_at_FildeshSxpb(const FildeshSxpb* sxpb, FildeshSxpbIT it);
const char*
str_value_at_FildeshSxpb(const FildeshSxpb* sxpb, FildeshSxpbIT it);

bool
lone_subfield_at_FildeshSxpb_to_bool(
    bool* dst, const FildeshSxpb* sxpb, FildeshSxpbIT it, const char* name);
bool
lone_subfield_at_FildeshSxpb_to_unsigned(
    unsigned* dst, const FildeshSxpb* sxpb, FildeshSxpbIT it, const char* name);
bool
lone_subfield_at_FildeshSxpb_to_float(
    float* dst, const FildeshSxpb* sxpb, FildeshSxpbIT it, const char* name);
bool
lone_subfield_at_FildeshSxpb_to_str(
    const char** dst, const FildeshSxpb* sxpb, FildeshSxpbIT it, const char* name);

FildeshSxpbIT
ensure_message_subfield_at_FildeshSxpb(
    FildeshSxpb* sxpb, FildeshSxpbIT it, const char* k);
FildeshSxpbIT
assign_bool_subfield_at_FildeshSxpb(
    FildeshSxpb* sxpb, FildeshSxpbIT it, const char* k, bool v);
FildeshSxpbIT
assign_str_subfield_at_FildeshSxpb(
    FildeshSxpb* sxpb, FildeshSxpbIT it, const char* k, const char* v);


static inline bool nullish_FildeshSxpbIT(FildeshSxpbIT pos) {
  return fildesh_nullid(pos.cons_id);
}
static inline FildeshSxpbIT FildeshSxpbIT_of_NULL() {
  FildeshSxpbIT it = DEFAULT_FildeshSxpbIT;
  it.field_kind = FildeshSxprotoFieldKind_UNKNOWN;
  return it;
}
static inline FildeshSxpbIT top_of_FildeshSxpb(const FildeshSxpb* sxpb) {
  FildeshSxpbIT pos = DEFAULT_FildeshSxpbIT;
  (void)sxpb;
  pos.cons_id = 0;
  return pos;
}


END_EXTERN_C
