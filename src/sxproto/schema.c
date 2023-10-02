#include <fildesh/sxproto.h>

#include <stdlib.h>
#include <string.h>

static
  int
compare_FildeshSxprotoField_(const void* a, const void* b)
{
  return strcmp(((const FildeshSxprotoField*)a)->name,
                ((const FildeshSxprotoField*)b)->name);
}

  const FildeshSxprotoField*
subfield_of_FildeshSxprotoField(const FildeshSxprotoField* schema, const char* name)
{
  FildeshSxprotoField needle = {"", FILL_DEFAULT_FildeshSxprotoField_BOOL};
  const FildeshSxprotoField* subfield;
  needle.name = name;
  subfield = (const FildeshSxprotoField*) bsearch(
      &needle, (const FildeshSxprotoField*)schema->hi,
      schema->lo, sizeof(const FildeshSxprotoField),
      compare_FildeshSxprotoField_);
  if (subfield && subfield->kind == FildeshSxprotoFieldKind_UNKNOWN) {
    needle.name = (const char*)subfield->hi;
    subfield = (const FildeshSxprotoField*) bsearch(
        &needle, (const FildeshSxprotoField*)schema->hi,
        schema->lo, sizeof(const FildeshSxprotoField),
        compare_FildeshSxprotoField_);
  }
  return subfield;
}

