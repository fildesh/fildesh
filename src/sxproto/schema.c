#include <fildesh/sxproto.h>

#include <assert.h>
#include <stdlib.h>
#include <string.h>

static
  int
compare_FildeshSxprotoField_(const void* a, const void* b)
{
  const size_t m = strlen(((const FildeshSxprotoField*)a)->name);
  const size_t n = strlen(((const FildeshSxprotoField*)b)->name);
  if (m < n) {return -1;}
  if (m > n) {return 1;}
  if (n == 0) {return 0;}
  return memcmp(((const FildeshSxprotoField*)a)->name,
                ((const FildeshSxprotoField*)b)->name,
                n);
}

static
  const FildeshSxprotoField*
subfield_of_FildeshSxprotoField_FIELDS_(
    const FildeshSxprotoField* subfields, unsigned n, const char* name)
{
  FildeshSxprotoField needle = {"", FILL_DEFAULT_FildeshSxprotoField_BOOL};
  needle.name = name;
  assert(subfields);
  return (const FildeshSxprotoField*) bsearch(
      &needle, subfields, n, sizeof(const FildeshSxprotoField),
      compare_FildeshSxprotoField_);
}

  const FildeshSxprotoField*
subfield_of_FildeshSxprotoField(const FildeshSxprotoField* schema, const char* name)
{
  const FildeshSxprotoField* subfield = subfield_of_FildeshSxprotoField_FIELDS_(
      schema->subfields, schema->hi, name);
  if (subfield && subfield->kind == FildeshSxprotoFieldKind_UNKNOWN) {
    subfield = subfield_of_FildeshSxprotoField_FIELDS_(
        schema->subfields, schema->hi, (const char*)subfield->hi);
  }
  return subfield;
}

static
  void
subfield_initialization_FildeshSxprotoField(const FildeshSxprotoField* path)
{
  unsigned i;
  const unsigned n = path->hi;
  const char* aliased_subfield_name = NULL;
  for (i = 0; i < n; ++i) {
    FildeshSxprotoField* subfield = (FildeshSxprotoField*)&path->subfields[i];
    if (subfield->subfields == path->subfields) {return;}
    if (subfield->kind == FildeshSxprotoFieldKind_UNKNOWN) {
      if ((const char*)subfield->hi) {return;}
      assert(aliased_subfield_name);
      subfield->hi = (uintptr_t)aliased_subfield_name;
      continue;
    }
    if (subfield->kind == FildeshSxprotoFieldKind_MESSAGE ||
        subfield->kind == FildeshSxprotoFieldKind_MANYOF) {
      if (subfield->kind == path->kind && !subfield->subfields) {
        subfield->hi = n;
        subfield->subfields = path->subfields;
      }
    }
    aliased_subfield_name = subfield->name;

    if (subfield->subfields) {
      FildeshSxprotoField frontier = *subfield;
      const FildeshSxprotoField* past;
      if (frontier.kind == FildeshSxprotoFieldKind_ARRAY) {
        frontier.kind = FildeshSxprotoFieldKind_MESSAGE;
      }
      frontier.lo = (intptr_t)path;
      for (past = path; past; past = (const FildeshSxprotoField*)past->lo) {
        if (frontier.subfields == past->subfields) {
          break;
        }
      }
      if (!past) {
        subfield_initialization_FildeshSxprotoField(&frontier);
      }
    }
  }
  qsort(
      (FildeshSxprotoField*)path->subfields, n, sizeof(FildeshSxprotoField),
      compare_FildeshSxprotoField_);
}

  bool
lone_toplevel_initialization_FildeshSxprotoField(FildeshSxprotoField* schema)
{
  static const char toplevel_name[] = "";
  if (schema->name) {return false;}
  schema->name = toplevel_name;
  assert(!(FildeshSxprotoField*)schema->lo);
  subfield_initialization_FildeshSxprotoField(schema);
  return true;
}

