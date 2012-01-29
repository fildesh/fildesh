
#include "rbtree.h"
#include <stdlib.h>
#include <stddef.h>

static Bit
root (const RBTree* t, const RBTNode* x)
{
    return root_BSTree (&t->bst, &x->bst);
}

static RBTNode*
joint (RBTNode* x)
{
    return CastUp( RBTNode, bst, x->bst.joint );
}

static RBTNode*
split (RBTNode* x, Bit side)
{
    return CastUp( RBTNode, bst, x->bst.split[side] );
}

static void
rotate (RBTNode* x, Bit lowup)
{
    rotate_BSTNode (&x->bst, lowup);
}

    void
init_RBTree (RBTree* t, RBTNode* sentinel,
             Trit (* swapped) (const BSTNode* lhs, const BSTNode* rhs))
{
    sentinel->red = Nil;
    init_BSTree (&t->bst, &sentinel->bst, swapped);
}

static void
fixup_insert (RBTNode* x, RBTree* t)
{
    while (1)
    {
        RBTNode* b;
        RBTNode* a;
        Bit xside;

            /* /x/ is root, just set to black!*/
        if (root (t, x))
        {
            x->red = Nil;
            break;
        }
        b = joint (x);

            /* /b/ is black, /x/ is safe to be red!*/
        if (!b->red)  break;

        a = joint (b);
        xside = side_BSTNode (&x->bst);

            /* Case 1.         (continue)
             *
             *    a#              b'+
             *    / \              / \
             *  1*   +b          a#   #x
             *      / \          /|   |\
             *    2#   +'x     w* #2 3# #4
             *        / \
             *      3#   #4
             */
        if (xside == side_BSTNode (&b->bst))
        {
            rotate (a, !xside);
            x->red = Nil;
            x = b;
        }
            /* Case 2.                       (continue)
             *
             *       a#             a#          b'+
             *       / \            / \          / \
             *     b+   *4   =>   x+   *4  =>  x#   #a
             *     / \            / \          /|   |\
             *   1#   +'x       b#   #3      1# #2 3# *4
             *       / \        / \
             *     2#   #3    1#   #2
             */
        else
        {
            rotate (b, !xside);
            b->red = Nil;
            rotate (a, xside);
        }
    }
}

    void
insert_RBTree (RBTree* t, RBTNode* x)
{
    insert_BSTree (&t->bst, &x->bst);
    x->red = Yes;
    fixup_insert (x, t);
}

    /** If a node matching /x/ exists, return that node.
     * Otherwise, add /x/ to the tree and return it.
     **/
    RBTNode*
ensure_RBTree (RBTree* t, RBTNode* x)
{
    BSTNode* y = ensure_BSTree (&t->bst, &x->bst);
    if (y == &x->bst)
    {
        x->red = Yes;
        fixup_insert (x, t);
    }
    else
    {
        x = CastUp( RBTNode, bst, y );
    }
    return x;
}

    /**
     * Ensure /x/ exists in the tree.
     * It replaces a matching node if one exists.
     * The matching node (which was replaced) is returned.
     * If no matching node was replaced, 0 is returned.
     **/
    RBTNode*
setf_RBTree (RBTree* t, RBTNode* x)
{
    BSTNode* y = setf_BSTree (&t->bst, &x->bst);
    if (!y)  return 0;
    x->red = Yes;
    fixup_insert (x, t);
    return CastUp( RBTNode, bst, y );
}

static void
fixup_remove (RBTNode* y, RBTree* t, Bit side)
{
    while (!root (t, y))
    {
        RBTNode* b = joint (y);
        RBTNode* a;
        RBTNode* w;
        RBTNode* x;

            /* Case 1.          (done)
             *
             *      b+            a#
             *      / \           / \
             *    a#   #'y  =>  w*   +b
             *    / \               / \
             *  w*   *x           x*   #y
             */
        if (b->red)
        {
            rotate (b, side);
            break;
        }
        a = split (b, !side);

            /* Case 2.        (continue, match case 1)
             *
             *      b#            a#
             *      / \           / \
             *    a+   #'y  =>  w#   +b
             *    / \               / \
             *  w#   #x           x#   #'y
             */
        if (a->red)
        {
            rotate (b, side);
            a->red = 0;
            b->red = 1;
            continue;  /* Match case 1.*/
        }
        w = split (a, !side);

            /* Case 3.          (done)
             *
             *      b#            a#
             *      / \           / \
             *    a#   #'y  =>  w#   #b
             *    / \               / \
             *  w+   *x           x*   #y
             */
        if (w->red)
        {
            rotate (b, side);
            w->red = 0;
            break;
        }
        x = split (a, side);

            /* Case 4.                        (done)
             *
             *      b#             b#           x#
             *      / \            / \          / \
             *    a#   #'y  =>   x+   #y  =>  a#   #b
             *    / \            / \          /|   |\
             *  w*   +x        a#   #2      w* #1 2# #y
             *       / \       / \
             *     1#   #2   w*   #1
             */
        if (x->red)
        {
            rotate (a, !side);
            rotate (b, side);
            x->red = 0;
            break;
        }

            /* Case 5.       (continue)
             *
             *      b#          b'#
             *      / \          / \
             *    a#   #'y =>  a+   #y
             *    / \          / \
             *  w#   #x      w#   #x
             */
        a->red = 1;
        y = b;
        side = side_BSTNode (&y->bst);
    }
}

    void
remove_RBTree (RBTree* t, RBTNode* y)
{
    RBTNode* x = joint (y);
    Bit side = side_BSTNode (&y->bst);
    Bit red;
    remove_BSTree (&t->bst, &y->bst);
    x = split (x, side);
    if (&x->bst == t->bst.sentinel)
    {
        red = y->red;
    }
    else
    {
        red = x->red;
        x->red = y->red;
    }
    if (red)  return;

        /* Put /y/ as a leaf in the tree to simplify fixup.*/
    y->red = 0;
    y->bst.split[0] = t->bst.sentinel;
    y->bst.split[1] = t->bst.sentinel;

    fixup_remove (y, t, y->bst.joint->split[1] == t->bst.sentinel);
}

