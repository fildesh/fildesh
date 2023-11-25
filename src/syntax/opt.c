#include "opt.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "symval.h"

static
  char**
fildesh_syntax_ensure_string_variable(FildeshKV* map, const char* name)
{
  SymVal* x = getf_fildesh_SymVal(map, name);
  if (x->kind == NSymValKinds) {
    x->kind = HereDocVal;
    x->as.here_doc = NULL;
  }
  if (x->kind == HereDocVal) {
    return &x->as.here_doc;
  }
  fildesh_log_errorf("Cannot overwrite non-string variable: %s", name);
  return NULL;
}

static
  const char*
getopt_FildeshO(FildeshO* out, const char* const flag, const char* const arg)
{
  const char* s;
  if (flag[0] != '-') {return NULL;}
  if (flag[1] == '\0') {return NULL;}

  s = &flag[1];
  if (flag[1] == '-') {
    s = &flag[2];
    assert(s[0] != '\0');
  }
  while (s[0] != '\0') {
    const size_t n = strcspn(s, "-=");
    put_bytestring_FildeshO(out, (const unsigned char*)s, n);
    s = &s[n];
    if (s[0] == '=') {
      return &s[1];
    }
    if (s[0] == '-') {
      putc_FildeshO(out, '_');
      s = &s[1];
    }
  }
  return arg;
}

  int
fildesh_syntax_parse_flags(
    char** args, FildeshKV* map, const char*** ret_stdargs, FildeshO* tmp_out)
{
  unsigned i;
  for (i = 0; args[i]; ++i) {
    char** p;
    const char* k;
    const char* v;
    truncate_FildeshO(tmp_out);
    if (args[i][0] == '-' && args[i][1] == '-' && args[i][2] == '\0') {
      for (i = i+1; args[i]; ++i) {
        push_FildeshAT(ret_stdargs, args[i]);
      }
      break;
    }
    putstr_FildeshO(tmp_out, ".self.opt.");
    v = getopt_FildeshO(tmp_out, args[i], args[i+1]);
    if (!v) {
      fildesh_log_errorf("Expected a value for flag: %s", args[i]);
      return 64;
    }
    if (v == args[i+1]) {
      i += 1;
    }

    k = strdup_FildeshO(tmp_out, map->alloc);
    p = fildesh_syntax_ensure_string_variable(map, k);
    assert(p);
    *p = (char*)v;
  }
  return 0;
}

