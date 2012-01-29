
#ifndef ASSOC_H_
#define ASSOC_H_
#include "rbtree.h"

#define DeclAssocNodeField( T ) \
    RBTNode field_AssocNode

#define DeclAssocT( T, swapped ) \
    typedef struct AssocNode_##T AssocNode_##T; \
    typedef struct Assoc_##T Assoc_##T; \
    struct AssocNode_##T { \
        RBTNode rbt; \
    }; \
    struct Assoc_##T { \
        RBTNode sentinel; \
        RBTree rbt; \
    }; \
    static Trit \
    AssocNode_##T##_swapped (const BSTNode* lhs, const BSTNode* rhs) \
    { \
        const RBTNode* a = CastUp( RBTNode, bst, lhs ); \
        const RBTNode* b = CastUp( RBTNode, bst, rhs ); \
        return swapped( CastUp( T, field_AssocNode, a ), \
                        CastUp( T, field_AssocNode, b ) ); \
    } \
    static void \
    init_Assoc_##T (Assoc_##T* a) \
    { \
        init_RBTree (&a->rbt, &a->sentinel, AssocNode_##T##_swapped); \
    } \
    static T* \
    setf_Assoc_##T (Assoc_##T* a, T* e) \
    { \
        RBTNode* node = setf_RBTree (&a->rbt, &e->field_AssocNode); \
        if (!node)  return 0; \
        return CastUp( T, field_AssocNode, node ); \
    } \
    static T* \
    get_Assoc_##T (Assoc_##T* a, T* e) \
    { \
        BSTNode* node; \
        node = find_BSTree (&a->rbt.bst, &e->field_AssocNode.bst); \
        return (node \
                ? CastUp( T, field_AssocNode, \
                          CastUp( RBTNode, bst, node ) ) \
                : 0); \
    } \
    typedef Trit (*Assoc_##T##_func) (const T*, const T*)


#define Assoc( T ) \
    Assoc_##T

#define InitAssoc( T, a ) \
    init_Assoc_##T (&(a))

#define SetfAssoc( T, a, e ) \
    setf_Assoc_##T (&(a), &(e))

#define GetAssoc( T, a, e ) \
    get_Assoc_##T (&(a), &(e))

#endif

