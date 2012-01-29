
#include "bstree.h"

    void
init_BSTree (BSTree* t, BSTNode* sentinel,
             Trit (* swapped) (const BSTNode* lhs, const BSTNode* rhs))
{
    sentinel->joint = sentinel;
    sentinel->split[0] = sentinel;
    sentinel->split[1] = sentinel;
    t->sentinel = sentinel;
    t->swapped = swapped;
}

    void
lose_BSTree (BSTree* t, void (* lose) (BSTNode*))
{
    BSTNode* y = root_of_BSTree (t);

    while (y != t->sentinel)
    {
        BSTNode* x = y;
        Bit side;

        while (x != t->sentinel)
        {
            y = x;
            x = x->split[0];
        }

        if (y == t->sentinel)  break;

        do
        {
            x = y;
            y = x->joint;
            side = side_BSTNode (x);
            lose (x);
        } while (y != t->sentinel &&
                 (side == 1 || y->split[1] == t->sentinel));

        if (y != t->sentinel)
            y = y->split[1];
    }
}

    bool
root_BSTree (const BSTree* t, const BSTNode* x)
{
    return (x->joint == t->sentinel);
}

    BSTNode*
root_of_BSTree (BSTree* t)
{
    return t->sentinel->split[1];
}

    void
root_for_BSTree (BSTree* t, BSTNode* x)
{
    x->joint = t->sentinel;
    t->sentinel->split[1] = x;
}

    /**
     * Preorder, postorder, and inorder traversals are supported by
     * values of Nil, Yes, and May for /postorder/ respectively.
     **/
    void
traverse_BSTree (BSTree* t, Trit postorder,
                 void (* f) (BSTNode*, void*), void* dat)
{
    BSTNode* y = root_of_BSTree (t);

    while (y != t->sentinel)
    {
        BSTNode* x = y;
        Bit side;

        while (x != t->sentinel)
        {
            if (postorder == Nil)  f (x, dat);
            y = x;
            x = x->split[0];
        }

        if (y == t->sentinel)  break;
        if (postorder == May)  f (y, dat);

        do
        {
            x = y;
            y = x->joint;
            side = side_BSTNode (x);
            if (postorder == Yes)  f (x, dat);
        } while (y != t->sentinel &&
                 (side == 1 || y->split[1] == t->sentinel));

        if (y != t->sentinel)
        {
            if (postorder == May)  f (y, dat);
            y = y->split[1];
        }
    }
}

    BSTNode*
find_BSTree (BSTree* t, const BSTNode* x)
{
    BSTNode* y = root_of_BSTree (t);

    while (y != t->sentinel)
    {
        switch (t->swapped (x, y))
        {
            case Nil:  y = y->split[0];  break;
            case Yes:  y = y->split[1];  break;
            case May:  return y;
        }
    }
    return 0;
}

    void
insert_BSTree (BSTree* t, BSTNode* x)
{
    BSTNode* a = t->sentinel;
    BSTNode* y = root_of_BSTree (t);
    Bit side = side_BSTNode (y);

    while (y != t->sentinel)
    {
        side = (t->swapped (x, y) == Yes) ? 1 : 0;
        a = y;
        y = y->split[side];
    }

    a->split[side] = x;
    x->joint = a;
    x->split[0] = t->sentinel;
    x->split[1] = t->sentinel;
}

    /** If a node matching /x/ exists, return that node.
     * Otherwise, add /x/ to the tree and return it.
     **/
    BSTNode*
ensure_BSTree (BSTree* t, BSTNode* x)
{
    BSTNode* a = t->sentinel;
    BSTNode* y = root_of_BSTree (t);
    Bit side = side_BSTNode (y);

    while (y != t->sentinel)
    {
        switch (t->swapped (x, y))
        {
            case Nil:  side = 0;  break;
            case Yes:  side = 1;  break;
            case May:  return y;
        }
        a = y;
        y = y->split[side];
    }

    a->split[side] = x;
    x->joint = a;
    x->split[0] = t->sentinel;
    x->split[1] = t->sentinel;
    return x;
}

    /**
     * Ensure /x/ exists in the tree.
     * It replaces a matching node if one exists.
     * The matching node (which was replaced) is returned.
     * If no matching node was replaced, 0 is returned.
     **/
    BSTNode*
setf_BSTree (BSTree* t, BSTNode* x)
{
    BSTNode* y = ensure_BSTree (t, x);
    if (y == x)  return 0;
    x->joint = y->joint;
    x->joint->split[side_BSTNode (y)] = x;
    x->split[0] = y->split[0];
    x->split[0]->joint = x;
    x->split[1] = y->split[1];
    x->split[1]->joint = x;
    return y;
}

    /** Remove a given node from the tree.
     *
     *       b          b
     *      /          / \
     *     a    ==>   x
     *    / \        / \
     *       y          y
     *      / \        / \
     *     x
     *      \
     **/
    void
remove_BSTree (BSTree* t, BSTNode* a)
{
    Bit side = (a->split[0] == t->sentinel) ? 0 : 1;
    BSTNode* x = a->split[side ? 0 : 1];
    BSTNode* y;

    if (x == t->sentinel)
    {
        side = 1;
        x = a;
    }
    else
    {
        do
        {
            y = x;
            x = y->split[side];
        } while (x != t->sentinel);
        x = y;
    }

    y = x->joint;

    side = !side;
    y->split[side_BSTNode (x)] = x->split[side];
    x->split[side]->joint = y;

    if (x != a)
    {
        x->joint = a->joint;
        x->joint->split[side_BSTNode (a)] = x;

        x->split[0] = a->split[0];
        x->split[0]->joint = x;

        x->split[1] = a->split[1];
        x->split[1]->joint = x;
    }

    if (y == a)  y = x;

    a->joint = y;
}

    Bit
side_BSTNode (const BSTNode* x)
{
    return (x == x->joint->split[1]);
}

    /** Do a tree rotation,
     *
     *      b            a
     *     / \          / \
     *    a       ==>      b
     *   / \              / \
     *      y            y
     *
     * When /side/ is 1, opposite direction when 0.
     **/
    void
rotate_BSTNode (BSTNode* b, Bit side)
{
    const int p = side ? 0 : 1;
    const int q = side ? 1 : 0;
    BSTNode* a = b->split[p];
    BSTNode* y = a->split[q];

    b->joint->split[side_BSTNode (b)] = a;
    a->joint = b->joint;

    a->split[q] = b;
    b->joint = a;

    b->split[p] = y;
    y->joint = b;
}

