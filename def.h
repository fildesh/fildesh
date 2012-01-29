
#ifndef DEF_H_
#define DEF_H_
#include <stddef.h>

#define CastUp( T, field, p ) \
    ((T*) ((ptrdiff_t) p - offsetof( T, field )))

#define IndexOf( T, a, e ) \
    (((ptrdiff_t) (e) - (ptrdiff_t) (a)) / sizeof (T))

#define inline __inline

#if 1
typedef int bool;
#define true 1
#define false 0
#endif

typedef unsigned int uint;
#define UFor( i, bel )  for (i = 0; i < (bel); ++i)

typedef int Bit;
enum Trit { Nil = 0, Yes = 1, May = 2 };
typedef enum Trit Trit;

#endif

