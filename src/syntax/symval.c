#include "symval.h"

#include <ctype.h>
#include <string.h>
#include "include/fildesh/fildesh_compat_string.h"

  static unsigned
count_ws (const char* s)
{
  return strspn (s, fildesh_compat_string_blank_bytes);
}
  static unsigned
count_non_ws (const char* s)
{
  return strcspn (s, fildesh_compat_string_blank_bytes);
}

  static void
init_SymVal (SymVal* v)
{
  v->kind = NSymValKinds;
}

  SymVal*
getf_fildesh_SymVal(FildeshKV* map, const char* s)
{
  FildeshKV_id id = ensuref_FildeshKV(map, s, strlen(s)+1);
  SymVal* x = (SymVal*) value_at_FildeshKV(map, id);
  if (!x) {
    x = fildesh_allocate(SymVal, 1, map->alloc);
    init_SymVal(x);
    assign_memref_at_FildeshKV(map, id, x);
  }
  return x;
}

  SymVal*
declare_fildesh_SymVal(FildeshKV* map, SymValKind kind, const char* name)
{
  FildeshKV_id id = ensuref_FildeshKV(map, name, strlen(name)+1);
  SymVal* x = (SymVal*) value_at_FildeshKV(map, id);
  if (!x) {
    x = fildesh_allocate(SymVal, 1, map->alloc);
    init_SymVal(x);
    assign_memref_at_FildeshKV(map, id, x);
  }
  else if (x->kind == IOFileVal || x->kind == TmpFileVal) {
    fildesh_log_errorf("Cannot redefine tmpfile symbol: %s", name);
    return NULL;
  }
  x->kind = kind;
  return x;
}

  SymValKind
parse_fildesh_SymVal_arg(char* s, bool firstarg)
{
  unsigned o;
  SymValKind kind = NSymValKinds;

  if (firstarg && s[0] == '|') {
    if (s[1] == '-')
      kind = IODescVal;
    else if (s[1] == '<')
      kind = ODescVal;
    else if (s[1] == '>')
      kind = IDescVal;

    if (kind != NSymValKinds) {
      s[0] = '-';
      s[1] = '\0';
    }
    return kind;
  }

  if (!(s[0] == '$' && s[1] == '('))  return NSymValKinds;

  /* Offset into string.*/
  o = 2;

  if (s[o] == 'X')
  {
    if (s[o+1] == 'O')
    {
      if (s[o+2] == 'F')  kind = IOFileVal;
      else                kind = IODescVal;
    }
    else if (s[o+1] == 'A')
    {
      kind = IDescArgVal;
    }
    else if (s[o+1] == 'F')
    {
      if (s[o+2] == 'v')  kind = IFutureDescFileVal;
      else                kind = IDescFileVal;
    }
    else
    {
      if (s[o+1] == 'v')  kind = IFutureDescVal;
      else                kind = IDescVal;
    }
  }
  else if (s[o] == 'O')
  {
    if (s[o+1] == '?') {
      kind = ODescStatusVal;
    }
    else if (s[o+1] == 'F') {
      if (s[o+2] == '^')  kind = OFutureDescFileVal;
      else                kind = ODescFileVal;
    }
    else {
      if (s[o+1] == '^')  kind = OFutureDescVal;
      else                kind = ODescVal;
    }
  }
  else if (s[o] == 'H')
  {
    kind = HereDocVal;
  }

  if (kind != NSymValKinds)
  {
    unsigned i, n;
    i = count_non_ws(s);
    i += count_ws (&s[i]);
    n = strcspn (&s[i], ")");
    if (s[i+n] == ')')
    {
      memmove (s, &s[i], n * sizeof (char));
      s[n] = '\0';
    }
    else
    {
      kind = NSymValKinds;
    }
  }
  return kind;
}

