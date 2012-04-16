
#ifndef RBTree_H_
#define RBTree_H_
#include "bstree.h"

typedef struct RBTNode RBTNode;
typedef struct RBTree  RBTree;

struct RBTNode
{
    BSTNode bst;
    Bit red;
};

struct RBTree
{
    BSTree bst;
};

void
init_RBTree (RBTree* t, RBTNode* sentinel,
             Trit (* swapped) (const BSTNode* lhs, const BSTNode* rhs));
void
insert_RBTree (RBTree* t, RBTNode* x);
RBTNode*
ensure_RBTree (RBTree* t, RBTNode* x);
RBTNode*
setf_RBTree (RBTree* t, RBTNode* x);
void
remove_RBTree (RBTree* t, RBTNode* y);

#ifdef IncludeC
#include "rbtree.c"
#endif
#endif

