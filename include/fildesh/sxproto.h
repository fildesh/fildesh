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
  FildeshSxprotoFieldKind_LONEOF,
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
  unsigned tag_id;
  FildeshSxprotoFieldKind kind;
  const FildeshSxprotoField* subfields;
  uintptr_t hi;
  intptr_t lo;
};
#define FILL_FildeshSxprotoField_MESSAGE(subfields) \
  +0, FildeshSxprotoFieldKind_MESSAGE, \
  subfields, sizeof(subfields)/sizeof(FildeshSxprotoField), \
  0
#define DECLARE_TOPLEVEL_FildeshSxprotoField(schema, subfields) \
  static FildeshSxprotoField schema[] = { \
    {NULL, FILL_FildeshSxprotoField_MESSAGE(subfields)}, \
  }
bool
lone_toplevel_initialization_FildeshSxprotoField(FildeshSxprotoField* schema);
static inline
  unsigned
tag_id_of_FildeshSxprotoField(const FildeshSxprotoField* schema) {
  return schema->tag_id;
}

#define FILL_DEFAULT_FildeshSxprotoField_STRING \
  +0, FildeshSxprotoFieldKind_LITERAL_STRING, NULL, 0, 0
#define FILL_DEFAULT_FildeshSxprotoField_BOOL \
  +0, FildeshSxprotoFieldKind_LITERAL_BOOL, NULL, 0, 0
#define FILL_DEFAULT_FildeshSxprotoField_FLOAT \
  +0, FildeshSxprotoFieldKind_LITERAL_FLOAT, NULL, 0, 0

#define FILL_DEFAULT_FildeshSxprotoField_STRINGS \
  +0, FildeshSxprotoFieldKind_ARRAY, \
  NULL, (size_t)FildeshSxprotoFieldKind_LITERAL_STRING, \
  0
#define FILL_DEFAULT_FildeshSxprotoField_INTS \
  +0, FildeshSxprotoFieldKind_ARRAY, \
  NULL, (size_t)FildeshSxprotoFieldKind_LITERAL_INT, \
  0
#define FILL_DEFAULT_FildeshSxprotoField_FLOATS \
  +0, FildeshSxprotoFieldKind_ARRAY, \
  NULL, (size_t)FildeshSxprotoFieldKind_LITERAL_FLOAT, \
  0

#define FILL_RECURSIVE_FildeshSxprotoField_MESSAGE \
  +0, FildeshSxprotoFieldKind_MESSAGE, NULL, 0, 0
#define FILL_RECURSIVE_FildeshSxprotoField_MANYOF \
  +0, FildeshSxprotoFieldKind_MANYOF, NULL, 0, 0
#define FILL_DEFAULT_FildeshSxprotoField_ALIAS \
  +0, FildeshSxprotoFieldKind_UNKNOWN, NULL, 0, 0

#define FILL_FildeshSxprotoField_LONEOF(subfields) \
  +0, FildeshSxprotoFieldKind_LONEOF, \
  subfields, sizeof(subfields)/sizeof(FildeshSxprotoField), \
  0
#define FILL_FildeshSxprotoField_MESSAGES(subfields) \
  +0, FildeshSxprotoFieldKind_ARRAY, \
  subfields, sizeof(subfields)/sizeof(FildeshSxprotoField), \
  0
#define FILL_FildeshSxprotoField_MANYOF(subfields) \
  +0, FildeshSxprotoFieldKind_MANYOF, \
  subfields, sizeof(subfields)/sizeof(FildeshSxprotoField), \
  0
#define FILL_FildeshSxprotoField_INT(lo, hi) \
  +0, FildeshSxprotoFieldKind_LITERAL_INT, NULL, hi, lo
#define FILL_FildeshSxprotoField_FLOAT(lo, hi) \
  +0, FildeshSxprotoFieldKind_LITERAL_FLOAT, NULL, hi, lo
#define FILL_FildeshSxprotoField_STRING(lo, hi) \
  +0, FildeshSxprotoFieldKind_LITERAL_STRING, NULL, hi, lo


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

const char*
ensure_name_FildeshSxpb(FildeshSxpb* sxpb, const char* s, size_t n);
FildeshSxpbIT ensure_message_subfield_at_FildeshSxpb(FildeshSxpb*, FildeshSxpbIT, const char*);
FildeshSxpbIT ensure_array_subfield_at_FildeshSxpb(FildeshSxpb*, FildeshSxpbIT, const char*);
FildeshSxpbIT ensure_manyof_subfield_at_FildeshSxpb(FildeshSxpb*, FildeshSxpbIT, const char*);
FildeshSxpbIT ensure_loneof_subfield_at_FildeshSxpb(FildeshSxpb*, FildeshSxpbIT, const char*);
FildeshSxpbIT ensure_bool_subfield_at_FildeshSxpb(FildeshSxpb*, FildeshSxpbIT, const char*);
FildeshSxpbIT ensure_int_subfield_at_FildeshSxpb(FildeshSxpb*, FildeshSxpbIT, const char*);
FildeshSxpbIT ensure_float_subfield_at_FildeshSxpb(FildeshSxpb*, FildeshSxpbIT, const char*);
FildeshSxpbIT ensure_string_subfield_at_FildeshSxpb(FildeshSxpb*, FildeshSxpbIT, const char*);

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
assign_bool_subfield_at_FildeshSxpb(
    FildeshSxpb* sxpb, FildeshSxpbIT it, const char* k, bool v);
FildeshSxpbIT
assign_str_subfield_at_FildeshSxpb(
    FildeshSxpb* sxpb, FildeshSxpbIT it, const char* k, const char* v);

void
assign_at_FildeshSxpb(
    FildeshSxpb* sxpb, FildeshSxpbIT it,
    const char* optional_field_name,
    const FildeshSxpb* src_sxpb, FildeshSxpbIT src_it);
FildeshSxpbIT
append_at_FildeshSxpb(
    FildeshSxpb* sxpb, FildeshSxpbIT it,
    const char* optional_field_name,
    const FildeshSxpb* src_sxpb, FildeshSxpbIT src_it);

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
