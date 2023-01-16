/**
 * \file rbtree.c
 * Red-black tree.
 **/
#include "rbtree.h"
#include <stdlib.h>

#define Nullish(x)  !(x)
#define IsRoot(x)  root_ck_BSTree(&t->bst, &(x)->bst)

static inline RBTNode* JointOf(RBTNode* x) {
  return CastUp( RBTNode, bst, x->bst.joint );
}

static inline RBTNode* SplitOf(RBTNode* x, Bit side) {
  BSTNode* y = x->bst.split[side];
  return y ? CastUp( RBTNode, bst, y ) : 0;
}

static inline unsigned SideOf(RBTNode* x) {
  return side_of_BSTNode(&x->bst);
}

static inline bool RedColorOf(const RBTNode* x) {
  return x->red;
}
static inline void AssignColor(RBTNode* x, unsigned c) {
  x->red = c;
}
static inline void ColorRed(RBTNode* x) {
  AssignColor(x, 1);
}
static inline void ColorBlack(RBTNode* x) {
  AssignColor(x, 0);
}

  static void
Rotate(RBTNode* x, Bit lowup)
{
  rotate_BSTNode (&x->bst, lowup);
}

  RBTree
dflt2_RBTree (RBTNode* sentinel, PosetCmp cmp)
{
  RBTree t;
  sentinel->red = 0;
  cmp = dflt3_PosetCmp (offsetof(RBTNode, bst), cmp.off, cmp.fn);
  t.bst = dflt2_BSTree (&sentinel->bst, cmp);
  return t;
}

  void
init_RBTree (RBTree* t, RBTNode* sentinel, PosetCmp cmp)
{
  *t = dflt2_RBTree (sentinel, cmp);
}

  static void
fixup_insert (RBTNode* x, RBTree* t)
{
  while (1)
  {
    RBTNode* b;
    RBTNode* a;
    unsigned xside;

    /* /x/ is root, just set to black!*/
    if (IsRoot(x)) {
      ColorBlack(x);
      break;
    }
    b = JointOf(x);

    /* {b} is black, {x} is safe to be red!*/
    if (!RedColorOf(b))  break;

    a = JointOf(b);
    xside = SideOf(x);

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
    if (xside == SideOf(b)) {
      Rotate(a, 1-xside);
      ColorBlack(x);
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
    else {
      Rotate(b, 1-xside);
      ColorBlack(b);
      Rotate(a, xside);
    }
  }
}

  void
insert_RBTree (RBTree* t, RBTNode* x)
{
  insert_BSTree (&t->bst, &x->bst);
  ColorRed(x);
  fixup_insert (x, t);
}

/** If a node matching {x} exists, return that node.
 * Otherwise, add {x} to the tree and return it.
 **/
  RBTNode*
ensure_RBTree (RBTree* t, RBTNode* x)
{
  BSTNode* y = ensure_BSTree (&t->bst, &x->bst);
  if (y == &x->bst)
  {
    ColorRed(x);
    fixup_insert (x, t);
  }
  else
  {
    x = CastUp( RBTNode, bst, y );
  }
  return x;
}

/**
 * This function is called with {y} removed from the tree,
 * but {y->joint} points to a node whose {side} is one level
 * short on black nodes.
 **/
  static void
fixup_remove (RBTNode* y, RBTree* t, Bit side)
{
  if (RedColorOf(y)) {
    ColorBlack(y);
    return;
  }

  while (!IsRoot(y)) {
    RBTNode* b; RBTNode* a; RBTNode* w; RBTNode* x;
    b = JointOf(y);
    a = SplitOf(b, 1-side);
    w = SplitOf(a, 1-side);
    x = SplitOf(a, side);

    /* Case 1.                         (done)
     *
     *      b*             b*            x*
     *      / \            / \           / \
     *    a#   #'y  -->  x+   #y  -->  a#   #b
     *    / \            / \           /|   |\
     *  w*   +x        a#   #2       w* #1 2# #y
     *       / \       / \
     *     1#   #2   w*   #1
     */
    if (!Nullish(x) && RedColorOf(x)) {
      Rotate(a, 1-side);
      Rotate(b, side);
      if (RedColorOf(b)) {
        ColorBlack(b);
      }
      else {
        ColorBlack(x);
      }
      break;
    }

    /* Case 2.           (done)
     *
     *      b+             a#
     *      / \            / \
     *    a#   #'y  -->  w*   +b
     *    / \                / \
     *  w*   #x            x#   #y
     */
    if (RedColorOf(b)) {
      Rotate(b, side);
      break;
    }

    /* Case 3.         (continue, match case 1 or 2)
     *
     *      b#             a#
     *      / \            / \
     *    a+   #'y  -->  w#   +b
     *    / \                / \
     *  w#   #x            x#   #'y
     */
    if (RedColorOf(a)) {
      Rotate(b, side);
      ColorBlack(a);
      ColorRed(b);
      continue;  /* Match case 1 or 2.*/
    }

    /* Case 4.           (done)
     *
     *      b#             a#
     *      / \            / \
     *    a#   #'y  -->  w#   #b
     *    / \                / \
     *  w+   *x            x*   #y
     */
    if (!Nullish(w) && RedColorOf(w)) {
      Rotate(b, side);
      ColorBlack(w);
      break;
    }

    /* Case 5.         (continue)
     *
     *      b#            b'#
     *      / \            / \
     *    a#   #'y  -->  a+   #y
     *    / \            / \
     *  w#   #x        w#   #x
     */
    ColorRed(a);
    y = b;
    side = SideOf(y);
  }
}

  void
remove_RBTree (RBTree* t, RBTNode* y)
{
  RBTNode* b = JointOf(y);
  RBTNode* z;
  const unsigned side = SideOf(y);
  remove_BSTNode (&y->bst);
  z = SplitOf(b, side);
  if (z) {
    /* Recolor the node that replaced {y}.*/
    unsigned y_color = RedColorOf(y);
    AssignColor(y, RedColorOf(z));
    AssignColor(z, y_color);
  }

  /* If the removed color is red, then it is still a red-black tree.*/
  if (RedColorOf(y)) {return;}

  if (!Nullish(SplitOf(y, 0))) {
    fixup_remove(SplitOf(y, 0), t, 0);
  }
  else if (!Nullish(SplitOf(y, 1))) {
    fixup_remove(SplitOf(y, 1), t, 1);
  }
  else {
    /* Assume {y} is a leaf in the tree to simplify fixup.*/
    unsigned side = Nullish(SplitOf(JointOf(y), 1)) ? 1 : 0;
    fixup_remove(y, t, side);
  }
}

