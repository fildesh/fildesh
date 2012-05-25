
#ifndef Table_H_
#define Table_H_
#include "def.h"

#include <stdlib.h>
#include <string.h>

typedef size_t TableSz;
typedef byte TableLgSz;
typedef unsigned short TableElSz;

#define TableT( S )  TableT_##S
#define TableSzT( S )  TableSz

typedef struct Table Table;
struct Table
{
    void* s;
    TableSz sz;
    TableElSz elsz;
    TableLgSz alloc_lgsz;
};

#define DeclTableT( S, T ) \
    typedef struct TableT_##S TableT_##S; \
    typedef T TableElT_##S; \
    struct TableT_##S { \
        TableElT_##S* s; \
        TableSz sz; \
        TableLgSz alloc_lgsz; \
    }


qual_inline
    Table
make_Table (void* s, TableSz sz, TableElSz elsz, TableLgSz alloc_lgsz)
{
    Table t;
    t.s = s;
    t.sz = sz;
    t.elsz = elsz;
    t.alloc_lgsz = alloc_lgsz;
    return t;
}
qual_inline
    Table
dflt_TableT (TableElSz elsz)
{
    return make_Table (0, 0, elsz, 0);
}
#define DeclTable( S, table ) \
    TableT_##S table = { 0, 0, 0 }


#define MakeCastTable( t ) \
    make_Table ((t).s, (t).sz, sizeof(*(t).s), (t).alloc_lgsz)

#define XferCastTable( t, name )  do \
{ \
    memcpy (&(t).s, &(name).s, sizeof(void*)); \
    BSfx( t ,=, name ,.sz ); \
    BSfx( t ,=, name ,.alloc_lgsz ); \
} while (0)


qual_inline
    void
init_TableT (Table* t, TableElSz elsz)
{
    *t = dflt_TableT (elsz);
}
#define InitTable( t )  do \
{ \
    (t).s = 0; \
    (t).sz = 0; \
    (t).alloc_lgsz = 0; \
} while (0)

qual_inline
    void
lose_Table (Table* t)
{
    if (t->alloc_lgsz > 0)
        free (t->s);
}
#define LoseTable( t )  do \
{ \
    Table LoseTable_t = MakeCastTable( t ); \
    lose_Table (&LoseTable_t); \
} while (0)

#define AllocszTable( t ) \
    ((t).alloc_lgsz == 0 ? 0 : (TableSz)1 << ((t).alloc_lgsz - 1))
qual_inline
    TableSz
allocsz_Table (const Table* t)
{
    return AllocszTable( *t );
}

qual_inline
    void*
elt_Table (Table* t, TableSz idx)
{
    return EltZ( t->s, idx, t->elsz );
}
#define DeclEltTable( S, x, t, idx ) \
    TableElT_##S* const x = Elt( (t).s, idx )

qual_inline
    TableSz
idxelt_Table (const Table* t, const void* el)
{
    return (TableSz) IdxEltZ( t->s, el, t->elsz );
}
#define IdxEltTable( t, el ) \
    (TableSz) IdxEltZ( (t).s, el, (t).elsz )


qual_inline
    void
grow_Table (Table* t, TableSz capac)
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
        t->s = realloc (t->s, allocsz_Table (t) * t->elsz);
    }
}
#define GrowTable( t, capac )  do \
{ \
    Table GrowTable_t = MakeCastTable( t ); \
    grow_Table (&GrowTable_t, capac); \
    XferCastTable( t, GrowTable_t ); \
} while (0)


qual_inline
    void
mpop_Table (Table* t, TableSz capac)
{
    t->sz -= capac;
    if ((t->alloc_lgsz >= 3) && ((t->sz >> (t->alloc_lgsz - 3)) == 0))
    {
        while ((t->alloc_lgsz >= 4) && ((t->sz >> (t->alloc_lgsz - 4)) == 0))
            t->alloc_lgsz -= 1;
        t->alloc_lgsz -= 1;
        t->s = realloc (t->s, allocsz_Table (t) * t->elsz);
    }
}
#define MPopTable( t, capac )  do \
{ \
    Table MPopTable_t = MakeCastTable( t ); \
    mpop_Table (&MPopTable_t, capac); \
    XferCastTable( t, MPopTable_t ); \
} while (0)


qual_inline
    void*
grow1_Table (Table* t)
{
    grow_Table (t, 1);
    return elt_Table (t, t->sz - 1);
}

    /** Don't use this... It's a hack for the Grow1Table() macro.**/
qual_inline
    void
synhax_grow1_Table (void* ps, void* s, TableSz* sz,
                    TableElSz elsz, TableLgSz* alloc_lgsz)
{
    Table t = make_Table (s, *sz, elsz, *alloc_lgsz);
    grow1_Table (&t);
    memcpy (ps, &t.s, sizeof(void*));
    *alloc_lgsz = t.alloc_lgsz;
    *sz = t.sz;
}
#define Grow1Table( t ) \
    (synhax_grow1_Table (&(t).s, (t).s, &(t).sz, \
                         sizeof(*(t).s), &(t).alloc_lgsz), \
     &(t).s[(t).sz-1])
#define DeclGrow1Table( S, x, t ) \
    TableElT_##S* const x = Grow1Table( t )
#define PushTable( table, x ) \
    *(Grow1Table( table )) = (x)


qual_inline
    void
size_Table (Table* t, TableSz capac)
{
    if (t->sz <= capac)  grow_Table (t, capac - t->sz);
    else                 mpop_Table (t, t->sz - capac);
}
#define SizeTable( t, capac )  do \
{ \
    Table SizeTable_t = MakeCastTable( t ); \
    size_Table (&SizeTable_t, capac); \
    XferCastTable( t, SizeTable_t ); \
} while (0)

    /** Never downsize.**/
qual_inline
    void
sizeup_Table (Table* t, TableSz capac)
{
    if (t->sz < capac)
        grow_Table (t, capac - t->sz);
}
#define SizeUpTable( t, capac )  do \
{ \
    Table SizeUpTable_t = MakeCastTable( t ); \
    sizeup_Table (&SizeUpTable_t, capac); \
    XferCastTable( t, SizeUpTable_t ); \
} while (0)


qual_inline
    void
pack_Table (Table* t)
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
            t->s = realloc (t->s, t->sz * t->elsz);
            while ((t->sz << 1) < ((TableSz) 1 << t->alloc_lgsz))
                t->alloc_lgsz -= 1;
        }
    }
}
#define PackTable( t )  do \
{ \
    Table PackTable_t = MakeCastTable( t ); \
    pack_Table (&PackTable_t); \
    XferCastTable( t, PackTable_t ); \
} while (0)

#endif

