
#ifndef BSTree
#define BSTree BSTree

#include "def.h"

typedef struct BSTNode BSTNode;
typedef struct BSTree BSTree;

struct BSTNode
{
    BSTNode* joint;
    BSTNode* split[2];
};

struct BSTree
{
    BSTNode* sentinel;
    Trit (* swapped) (const BSTNode* lhs, const BSTNode* rhs);
};

void output_dot (BSTree* t);

void
init_BSTree (BSTree* t, BSTNode* sentinel,
             Trit (* swapped) (const BSTNode* lhs, const BSTNode* rhs));
void
lose_BSTree (BSTree* t, void (* lose) (BSTNode*));

bool
root_BSTree (const BSTree* t, const BSTNode* x);
BSTNode*
root_of_BSTree (BSTree* t);
void
root_for_BSTree (BSTree* t, BSTNode* x);

void
traverse_BSTree (BSTree* t, Trit postorder,
                 void (* f) (BSTNode*, void*), void* dat);
BSTNode*
find_BSTree (BSTree* t, const BSTNode* x);
void
insert_BSTree (BSTree* t, BSTNode* x);
BSTNode*
ensure_BSTree (BSTree* t, BSTNode* x);
BSTNode*
setf_BSTree (BSTree* t, BSTNode* x);
void
remove_BSTree (BSTree* t, BSTNode* a);

Bit
side_BSTNode (const BSTNode* x);
void
rotate_BSTNode (BSTNode* b, Bit side);

#endif

