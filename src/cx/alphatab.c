
#include "lace.h"
#include "alphatab.h"
#include <stdio.h>

  char*
itoa_dup_cstr (int x)
{
  char buf[50];
  sprintf (buf, "%i", x);
  return dup_cstr (buf);
}

  Sign
cmp_AlphaTab (const AlphaTab* a, const AlphaTab* b)
{
  zuint na = a->sz;
  zuint nb = b->sz;
  int ret;
  if (na > 0 && !a->s[na-1])  --na;
  if (nb > 0 && !b->s[nb-1])  --nb;

  if (na <= nb)
  {
    ret = memcmp (a->s, b->s, na * sizeof(char));
    if (ret == 0 && na < nb)
      ret = -1;
  }
  else
  {
    ret = memcmp (a->s, b->s, nb * sizeof(char));
    if (ret == 0)
      ret = 1;
  }
  return sign_of (ret);
}

  Sign
cmp_cstr_loc (const char* const* a, const char* const* b)
{
  return cmp_cstr (*a, *b);
}

  void
cat_uint_AlphaTab (AlphaTab* a, uint x)
{
  char buf[50];
  (void) sprintf(buf, "%u", x);
  cat_cstr_AlphaTab (a, buf);
}
