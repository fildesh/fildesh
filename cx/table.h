
#ifndef TABLE_H_
#define TABLE_H_
#include "def.h"

#define DeclTableT( T ) \
    typedef struct Table_##T Table_##T; \
    struct Table_##T { \
        uint sz; \
        uint alloc_sz; \
        T* s; \
    }

#define Table( T )  Table_##T

#define DeclTable( T, table ) \
    Table_##T table = { 0, 0, 0 }

#define InitTable( T, table )  do \
{ \
    (table).sz = 0; \
    (table).alloc_sz = 0; \
    (table).s = 0; \
} while (0)

#define GrowTable( T, table, capac )  do \
{ \
    (table).sz += (capac); \
    if ((table).sz >= (table).alloc_sz) \
    { \
        if ((table).alloc_sz == 0) \
            (table).alloc_sz = 4; \
        while ((table).sz >= (table).alloc_sz) \
            (table).alloc_sz *= 2; \
        (table).s = (T*) realloc ((table).s, (table).alloc_sz * sizeof (T)); \
    } \
} while (0)

#define MPopTable( T, table, capac )  do \
{ \
    (table).sz -= (capac); \
    if ((table).sz < (table).alloc_sz / 4 && (table).alloc_sz > 4) \
    { \
        do \
        { \
            (table).alloc_sz /= 2; \
        } while ((table).sz < (table).alloc_sz / 4 && (table).alloc_sz > 4); \
        (table).s = (T*) realloc ((table).s, (table).alloc_sz * sizeof (T)); \
    } \
} while (0)

#define PushTable( T, table, x )  do \
{ \
    GrowTable( T, table, 1 ); \
    (table).s[(table).sz-1] = (x); \
} while (0)

#define PackTable( T, table )  do if ((table).sz < (table).alloc_sz) \
{ \
    if ((table).sz == 0) \
    { \
        free ((table).s); \
        (table).s = 0; \
    } \
    else \
    { \
        (table).s = (T*) realloc ((table).s, (table).alloc_sz * sizeof (T)); \
    } \
} while (0)
    
#define LoseTable( T, table )  do if ((table).alloc_sz > 0) \
{ \
    free ((table).s); \
} while (0)

#define IndexInTable( T, table, e ) \
    IndexOf( T, (table).s, e )

#endif

