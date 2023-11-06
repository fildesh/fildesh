#include <fildesh/sxproto.h>

#include <assert.h>
#include <stdlib.h>
#include <string.h>

static
  int
compare_FildeshSxprotoField_(const void* a, const void* b)
{
  return strcmp(((const FildeshSxprotoField*)a)->name,
                ((const FildeshSxprotoField*)b)->name);
}

static
  const FildeshSxprotoField*
subfield_of_FildeshSxprotoField_FIELDS_(
    const FildeshSxprotoField* subfields, unsigned n, const char* name)
{
  FildeshSxprotoField needle = {"", FILL_DEFAULT_FildeshSxprotoField_BOOL};
  needle.name = name;
  return (const FildeshSxprotoField*) bsearch(
      &needle, subfields, n, sizeof(const FildeshSxprotoField),
      compare_FildeshSxprotoField_);
}

  const FildeshSxprotoField*
subfield_of_FildeshSxprotoField(const FildeshSxprotoField* schema, const char* name)
{
  const FildeshSxprotoField* subfield = subfield_of_FildeshSxprotoField_FIELDS_(
      (const FildeshSxprotoField*)schema->hi, schema->lo, name);
  if (subfield && subfield->kind == FildeshSxprotoFieldKind_UNKNOWN) {
    subfield = (const FildeshSxprotoField*)subfield->hi;
  }
  return subfield;
}

  void
alias_FildeshSxprotoField_FIELDS_(
    FildeshSxprotoField* subfields, unsigned n,
    const char* alias, const char* name)
{
  const FildeshSxprotoField* name_field = (FildeshSxprotoField*)
    subfield_of_FildeshSxprotoField_FIELDS_(subfields, n, name);
  FildeshSxprotoField* alias_field = (FildeshSxprotoField*)
    subfield_of_FildeshSxprotoField_FIELDS_(subfields, n, alias);
  assert(name_field);
  assert(alias_field);
  alias_field->hi = (uintptr_t)name_field;
}
