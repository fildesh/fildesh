
#ifndef ASSOC_H_
#define ASSOC_H_
#include "rbtree.h"

#define DeclAssocNodeField( S, T ) \
    RBTNode field_AssocNode_##S

#define DeclAssocT( S, T, swapped ) \
    typedef struct Assoc_##S Assoc_##S; \
    struct Assoc_##S { \
        RBTNode sentinel; \
        RBTree rbt; \
        T* table; \
    }; \
    static Trit \
    AssocNode_##S##_swapped (const BSTNode* lhs, const BSTNode* rhs) \
    { \
        const RBTNode* a = CastUp( RBTNode, bst, lhs ); \
        const RBTNode* b = CastUp( RBTNode, bst, rhs ); \
        return swapped( CastUp( T, field_AssocNode_##S, a ), \
                        CastUp( T, field_AssocNode_##S, b ) ); \
    } \
    inline void \
    init_Assoc_##S (Assoc_##S* a) \
    { \
        init_RBTree (&a->rbt, &a->sentinel, AssocNode_##S##_swapped); \
        a->table = 0; \
    } \
    inline void \
    fix_refs_Assoc_##S (Assoc_##S* a, T* table, uint n) \
    { \
        uint i; \
        ptrdiff_t diff = (ptrdiff_t) table - (ptrdiff_t) a->table; \
        BSTNode* sentinel; \
        if (diff == 0)  return; \
        sentinel = &a->sentinel.bst; \
        a->table = table; \
        for (i = 0; i <= n; ++i) \
        { \
            BSTNode* x = ((i < n) \
                          ? &table[i].field_AssocNode_##S.bst \
                          : sentinel); \
            if (x->joint != sentinel) \
                x->joint = (BSTNode*) (diff + (ptrdiff_t) x->joint); \
            if (x->split[0] != sentinel) \
                x->split[0] = (BSTNode*) (diff + (ptrdiff_t) x->split[0]); \
            if (x->split[1] != sentinel) \
                x->split[1] = (BSTNode*) (diff + (ptrdiff_t) x->split[1]); \
        } \
    } \
    inline T* \
    setf_Assoc_##S (Assoc_##S* a, T* e) \
    { \
        RBTNode* node = setf_RBTree (&a->rbt, &e->field_AssocNode_##S); \
        if (!node)  return 0; \
        return CastUp( T, field_AssocNode_##S, node ); \
    } \
    inline T* \
    getf_Assoc_##S (Assoc_##S* a, T* e) \
    { \
        RBTNode* x = ensure_RBTree (&a->rbt, &e->field_AssocNode_##S); \
        return CastUp( T, field_AssocNode_##S, x ); \
    } \
    inline T* \
    get_Assoc_##S (Assoc_##S* a, T* e) \
    { \
        BSTNode* node; \
        node = find_BSTree (&a->rbt.bst, &e->field_AssocNode_##S.bst); \
        return (node \
                ? CastUp( T, field_AssocNode_##S, \
                          CastUp( RBTNode, bst, node ) ) \
                : 0); \
    } \
    inline void \
    del_Assoc_##S (Assoc_##S* a, T* e) \
    { \
        remove_RBTree (&a->rbt, &e->field_AssocNode_##S); \
    } \
    typedef struct RBTNode AssocNode_##S


#define Assoc( S ) \
    Assoc_##S

#define InitAssoc( S, a ) \
    init_Assoc_##S (&(a))

#define FixRefsAssoc( S, a, table, n ) \
    fix_refs_Assoc_##S (&a, table, n)

#define SetfAssoc( S, a, e ) \
    setf_Assoc_##S (&(a), &(e))

#define GetfAssoc( S, a, e ) \
    getf_Assoc_##S (&(a), &(e))

#define GetAssoc( S, a, e ) \
    get_Assoc_##S (&(a), &(e))

#define DelAssoc( S, a, e ) \
    del_Assoc_##S (&(a), &(e))

#endif

