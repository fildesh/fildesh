
#ifndef Table_H_
#define Table_H_
#include "def.h"

#include <stdlib.h>

typedef size_t TableSz;
typedef byte TableLgSz;
typedef unsigned short TableElSz;

#define TableT( S )  TableT_##S
#define TableSzT( S )  TableSz

#define DeclTableT( S, T ) \
    typedef struct TableT_##S TableT_##S; \
    typedef T TableElT_##S; \
    struct TableT_##S { \
        TableElT_##S* s; \
        TableSz sz; \
        TableElSz elsz; \
        TableLgSz alloc_lgsz; \
    }

DeclTableT( void, void );
typedef TableT(void) Table;


qual_inline
    Table
dflt_TableT (TableElSz elsz)
{
    Table t = { 0, 0, 0, 0 };
    t.elsz = elsz;
    return t;
}
#define DeclTable( S, table ) \
    TableT_##S table = { 0, 0, sizeof(TableElT_##S), 0 }

qual_inline
    void
init_TableT (Table* t, TableElSz elsz)
{
    *t = dflt_TableT (elsz);
}
#define InitTable( t ) \
    init_TableT ((Table*)&(t), sizeof(*(t).s))

qual_inline
    void
lose_Table (Table* t)
{
    if (t->alloc_lgsz > 0)
        free (t->s);
}
#define LoseTable( table ) \
    lose_Table ((Table*) &(table))

qual_inline
    TableSz
allocsz_Table (const Table* t)
{
    if (t->alloc_lgsz == 0)  return 0;
    return (TableSz)1 << (t->alloc_lgsz - 1);
}

qual_inline
    void*
elt_TableT (Table* t, TableSz idx, TableElSz elsz)
{
    return EltZ( t->s, idx, elsz );
}
qual_inline void* elt_Table (Table* t, TableSz idx)
{ return elt_TableT (t, idx, t->elsz); }

qual_inline
    TableSz
idxelt_TableT (const Table* t, const void* el, TableElSz elsz)
{
    return (TableSz) IdxEltZ( t->s, el, elsz );
}
qual_inline TableSz idxelt_Table (const Table* t, const void* el)
{ return idxelt_TableT (t, el, t->elsz); }
#define IdxEltTable( t, el ) \
    idxelt_TableT ((Table*) &(t), el, sizeof(*(t).s))


qual_inline
    void
grow_TableT (Table* t, TableSz capac, TableElSz elsz)
{
    t->sz += capac;
    if ((t->sz << 1) > ((TableSz)1 << t->alloc_lgsz))
    {
        if (t->alloc_lgsz == 0)
        {
            t->s = 0;
            t->alloc_lgsz = 1;
        }
        while (t->sz > ((TableSz)1 << t->alloc_lgsz))
            t->alloc_lgsz += 1;

        t->alloc_lgsz += 1;
        t->s = realloc (t->s, allocsz_Table (t) * elsz);
    }
}
#define GrowTable( t, capac ) \
    grow_TableT ((Table*) &(t), capac, sizeof(*(t).s))
qual_inline void grow_Table (Table* t, TableSz capac)
{ grow_TableT (t, capac, t->elsz); }


qual_inline
    void
mpop_TableT (Table* t, TableSz capac, TableElSz elsz)
{
    t->sz -= capac;
    if ((t->alloc_lgsz >= 3) && ((t->sz >> (t->alloc_lgsz - 3)) == 0))
    {
        while ((t->alloc_lgsz >= 4) && ((t->sz >> (t->alloc_lgsz - 4)) == 0))
            t->alloc_lgsz -= 1;
        t->alloc_lgsz -= 1;
        t->s = realloc (t->s, allocsz_Table (t) * elsz);
    }
}
#define MPopTable( t, capac ) \
    mpop_TableT ((Table*) &(t), capac, sizeof(*(t).s))
qual_inline void mpop_Table (Table* t, TableSz capac)
{ mpop_TableT (t, capac, t->elsz); }


qual_inline
    void*
grow1_TableT (Table* t, TableElSz elsz)
{
    grow_TableT (t, 1, elsz);
    return elt_TableT (t, t->sz - 1, elsz);
}
    /* (grow1_TableT ((Table*) &(t), sizeof(*(t).s)),  */
#define Grow1Table( t ) \
    (grow_TableT ((Table*) &(t), 1, sizeof(*(t).s)), \
     &(t).s[(t).sz-1])
#define DeclGrow1Table( S, t, x ) \
    TableElT_##S* const x = Grow1Table( t )
#define PushTable( table, x ) \
    *(Grow1Table( table )) = (x)
qual_inline void* grow1_Table (Table* t)
{ return grow1_TableT (t, t->elsz); }


qual_inline
    void
size_TableT (Table* t, TableSz capac, TableElSz elsz)
{
    if (t->sz <= capac)  grow_TableT (t, capac - t->sz, elsz);
    else                 mpop_TableT (t, t->sz - capac, elsz);
}
#define SizeTable( t, capac ) \
    size_TableT ((Table*) &(t), capac, sizeof(*(t).s))
qual_inline void size_Table (Table* t, TableSz capac)
{ size_TableT (t, capac, t->elsz); }


    /** Never downsize.**/
qual_inline
    void
sizeup_TableT (Table* t, TableSz capac, TableElSz elsz)
{
    if (t->sz < capac)
        grow_TableT (t, capac - t->sz, elsz);
}
#define SizeUpTable( t, capac ) \
    sizeup_TableT ((Table*) &(t), capac, sizeof(*(t).s))
qual_inline void sizeup_Table (Table* t, TableSz capac)
{ sizeup_TableT (t, capac, t->elsz); }


qual_inline
    void
pack_TableT (Table* t, TableElSz elsz)
{
    if ((t->sz << 1) < ((TableSz) 1 << t->alloc_lgsz))
    {
        if (t->sz == 0)
        {
            free (t->s);
            t->s = 0;
            t->alloc_lgsz = 0;
        }
        else
        {
            t->s = realloc (t->s, t->sz * elsz);
            while ((t->sz << 1) < ((TableSz) 1 << t->alloc_lgsz))
                t->alloc_lgsz -= 1;
        }
    }
}
#define PackTable( t ) \
    pack_TableT ((Table*) &(t), sizeof(*(t).s))
qual_inline void pack_Table (Table* t)
{ pack_TableT (t, t->elsz); }

#endif

