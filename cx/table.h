
#ifndef TABLE_H_
#define TABLE_H_
#include "def.h"

#define TableT( S )  TableT_##S

#define DeclTableT( S, T ) \
    typedef struct TableT_##S TableT_##S; \
    typedef struct CSeqT_##S CSeqT_##S; \
    typedef T TableElT_##S; \
    typedef uint TableSzT_##S; \
    struct CSeqT_##S { \
        TableSzT_##S sz; \
        TableElT_##S* s; \
    }; \
    struct TableT_##S { \
        TableSzT_##S sz; \
        TableSzT_##S alloc_sz; \
        TableElT_##S* s; \
    }; \
    qual_inline \
        void \
    grow_nodep_TableT_##S (TableT_##S* table, TableSzT_##S capac, \
                          void* (*f) (void*, size_t)) \
    { \
        table->sz += capac; \
        if (table->sz >= table->alloc_sz) \
        { \
            if (table->alloc_sz == 0) \
            { \
                table->s = 0; \
                table->alloc_sz = 4; \
            } \
            while (table->sz >= table->alloc_sz) \
                table->alloc_sz *= 2; \
            table->s = (TableElT_##S*) \
                f (table->s, table->alloc_sz * sizeof (TableElT_##S)); \
        } \
    } \
    qual_inline \
        TableElT_##S* \
    grow1_nodep_TableT_##S (TableT_##S* table, \
                           void* (*f) (void*, size_t)) \
    { \
        grow_nodep_TableT_##S (table, 1, f); \
        return &table->s[table->sz-1]; \
    } \
    qual_inline \
        CSeqT_##S \
    CSeqT_TableT_##S (TableT_##S* table) \
    { \
        CSeqT_##S c; \
        c.sz = table->sz; \
        c.s = table->s; \
        return c; \
    } \
    typedef TableSzT_##S CSeqSzT_##S

#define DeclTable( S, table ) \
    TableT_##S table = { 0, 0, 0 }

#define InitTable( S, table )  do \
{ \
    (table).sz = 0; \
    (table).alloc_sz = 0; \
    (table).s = 0; \
} while (0)

#define GrowTable( S, table, capac ) \
    grow_nodep_TableT_##S (&(table), capac, realloc)

#define Grow1Table( S, table ) \
    grow1_nodep_TableT_##S (&(table), realloc)

#define DeclGrow1Table( S, table, x ) \
    TableElT_##S* const x = Grow1Table( S, table )

#define MPopTable( S, table, capac )  do \
{ \
    (table).sz -= (capac); \
    if ((table).sz < (table).alloc_sz / 4 && (table).alloc_sz > 4) \
    { \
        do \
        { \
            (table).alloc_sz /= 2; \
        } while ((table).sz < (table).alloc_sz / 4 && (table).alloc_sz > 4); \
        (table).s = (TableElT_##S*) \
            realloc ((table).s, (table).alloc_sz * sizeof (TableElT_##S)); \
    } \
} while (0)

#define SizeTable( S, table, capac )  do \
{ \
    if ((table).sz <= (capac))  GrowTable( S, table, (capac) - (table).sz ); \
    else                        MPopTable( S, table, (table).sz - (capac) ); \
} while (0)

    /** Never downsize.**/
#define SizeUpTable( S, table, capac ) do \
{ \
    if ((table).sz < (capac))  GrowTable( S, table, (capac) - (table).sz ); \
} while (0)

#define PushTable( S, table, x ) \
    *(Grow1Table( S, table )) = (x)

#define PackTable( S, table )  do if ((table).sz < (table).alloc_sz) \
{ \
    if ((table).sz == 0) \
    { \
        free ((table).s); \
        (table).s = 0; \
    } \
    else \
    { \
        (table).s = (TableElT_##S*) \
            realloc ((table).s, (table).alloc_sz * sizeof (TableElT_##S)); \
    } \
} while (0)

#define LoseTable( S, table )  do if ((table).alloc_sz > 0) \
{ \
    free ((table).s); \
} while (0)

#define IndexInTable( S, table, e ) \
    IndexOf( TableElT_##S, (table).s, e )

#endif

