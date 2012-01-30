
#ifndef TABLE_H_
#define TABLE_H_
#include "def.h"

#define DeclTableT( S, T ) \
    typedef T TableT_##S; \
    typedef struct Table_##S Table_##S; \
    struct Table_##S { \
        uint sz; \
        uint alloc_sz; \
        TableT_##S* s; \
    }

#define Table( S )  Table_##S

#define DeclTable( S, table ) \
    Table_##S table = { 0, 0, 0 }

#define InitTable( S, table )  do \
{ \
    (table).sz = 0; \
    (table).alloc_sz = 0; \
    (table).s = 0; \
} while (0)

#define GrowTable( S, table, capac )  do \
{ \
    (table).sz += (capac); \
    if ((table).sz >= (table).alloc_sz) \
    { \
        if ((table).alloc_sz == 0) \
            (table).alloc_sz = 4; \
        while ((table).sz >= (table).alloc_sz) \
            (table).alloc_sz *= 2; \
        (table).s = (TableT_##S*) \
            realloc ((table).s, (table).alloc_sz * sizeof (TableT_##S)); \
    } \
} while (0)

#define MPopTable( S, table, capac )  do \
{ \
    (table).sz -= (capac); \
    if ((table).sz < (table).alloc_sz / 4 && (table).alloc_sz > 4) \
    { \
        do \
        { \
            (table).alloc_sz /= 2; \
        } while ((table).sz < (table).alloc_sz / 4 && (table).alloc_sz > 4); \
        (table).s = (TableT_##S*) \
            realloc ((table).s, (table).alloc_sz * sizeof (TableT_##S)); \
    } \
} while (0)

#define PushTable( S, table, x )  do \
{ \
    GrowTable( S, table, 1 ); \
    (table).s[(table).sz-1] = (x); \
} while (0)

#define PackTable( S, table )  do if ((table).sz < (table).alloc_sz) \
{ \
    if ((table).sz == 0) \
    { \
        free ((table).s); \
        (table).s = 0; \
    } \
    else \
    { \
        (table).s = (TableT_##S*) \
            realloc ((table).s, (table).alloc_sz * sizeof (TableT_##S)); \
    } \
} while (0)
    
#define LoseTable( S, table )  do if ((table).alloc_sz > 0) \
{ \
    free ((table).s); \
} while (0)

#define IndexInTable( S, table, e ) \
    IndexOf( TableT_##S, (table).s, e )

#endif

