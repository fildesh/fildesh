/**
 * \file rbtree.c
 * Red-black tree.
 **/
#include "rbtree.h"
#include <stdlib.h>

#define Nullish(x)  !(x)
#define IsRoot(x)  root_ck_BSTree(&map->bst, &(x)->bst)

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

static inline void RotateUp(RBTNode* const* p_x) {
  rotate_up_BSTNode(&(*p_x)->bst);
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

static
  void
fixup_insert(RBTree* map, RBTNode* x)
{
  while (1)
  {
    RBTNode* b;

    /* {x} is root, just set to black!*/
    if (IsRoot(x)) {
      ColorBlack(x);
      break;
    }

    b = JointOf(x);
    /* {b} is black, {x} is safe to be red!*/
    if (!RedColorOf(b)) {
      break;
    }

    /* Case 1.         (continue)
     *
     *    a#              b'+
     *    / \              / \
     *  1*   +b   -->    a#   #x
     *      / \          /|   |\
     *    2#   +'x     w* #2 3# #4
     *        / \
     *      3#   #4
     */
    if (SideOf(x) == SideOf(b)) {
      ColorBlack(x);
      RotateUp(&b);
      x = b;
    }
    /* Case 2.                        (continue)
     *
     *       a#             a#           x'+
     *       / \            / \           / \
     *     b+   *4  -->   x+   *4  -->  b#   #a
     *     / \            / \           /|   |\
     *   1#   +'x       b#   #3       1# #2 3# *4
     *       / \        / \
     *     2#   #3    1#   #2
     */
    else {
      ColorBlack(b);
      RotateUp(&x);
      RotateUp(&x);
    }
  }
}

  void
insert_RBTree (RBTree* map, RBTNode* x)
{
  insert_BSTree(&map->bst, &x->bst);
  ColorRed(x);
  fixup_insert(map, x);
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
    fixup_insert(t, x);
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
static
  void
fixup_remove (RBTree* map, RBTNode* y, Bit side)
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
      if (RedColorOf(b)) {
        ColorBlack(b);
      }
      else {
        ColorBlack(x);
      }
      RotateUp(&x);
      RotateUp(&x);
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
      RotateUp(&a);
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
      ColorBlack(a);
      ColorRed(b);
      RotateUp(&a);
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
      ColorBlack(w);
      RotateUp(&a);
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
remove_RBTree (RBTree* map, RBTNode* y)
{
  RBTNode* b = JointOf(y);
  RBTNode* z;
  const unsigned side = SideOf(y);
  remove_BSTNode (&y->bst);
  z = SplitOf(b, side);

  if (!Nullish(z)) {
    /* Recolor the node that replaced {y}.*/
    bool y_color = RedColorOf(y);
    AssignColor(y, RedColorOf(z));
    AssignColor(z, y_color);
  }

  if (RedColorOf(y)) {
    /* If the removed color is red, then it is still a red-black tree.*/
  }
  else if (!Nullish(SplitOf(y, 0))) {
    fixup_remove(map, SplitOf(y, 0), 0);
  }
  else if (!Nullish(SplitOf(y, 1))) {
    fixup_remove(map, SplitOf(y, 1), 1);
  }
  else if (!Nullish(JointOf(y))) {
    /* Assume {y} is a leaf in the tree to simplify fixup.*/
    unsigned side = Nullish(SplitOf(JointOf(y), 1)) ? 1 : 0;
    assert(Nullish(SplitOf(JointOf(y), side)));
    fixup_remove(map, y, side);
  }
}

