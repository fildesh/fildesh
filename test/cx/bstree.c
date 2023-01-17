/**
 * \file bstree.c
 * Binary search tree.
 **/
#include "bstree.h"
#include <assert.h>

#define positive_bit( x )  ((x) > 0 ? 1 : 0)

#define Nullish(x)  !(x)
#define JointOf(x)  ((x)->joint)
#define AssignJoint(x, y)  (x)->joint = (y)
#define MaybeAssignJoint(x, y)  if (x)  AssignJoint(x, y)
#define SplitOf(x, side)  ((x)->split[side])
#define AssignSplit(y, side, x)  (y)->split[side] = (x)
#define MaybeAssignSplit(y, side, x)  if (y)  AssignSplit(y, side, x)
#define NullifySplit(y, side)  (y)->split[side] = NULL
#define Join(y, side, x)  join_BSTNode(y, x, side)
#define SideOf(x)  side_of_BSTNode(x)

  BSTree
dflt2_BSTree (BSTNode* sentinel, PosetCmp cmp)
{
  BSTree t;
  sentinel->joint = 0;
  sentinel->split[0] = 0;
  sentinel->split[1] = 0;
  t.sentinel = sentinel;
  t.cmp = cmp;
  return t;
}

  void
init_BSTree (BSTree* t, BSTNode* sentinel, PosetCmp cmp)
{
  *t = dflt2_BSTree (sentinel, cmp);
}

  void
lose_BSTree (BSTree* t, void (* lose) (BSTNode*))
{
  BSTNode* y = root_of_BSTree (t);

  while (y)
  {
    BSTNode* x = y;
    Bit side = 0;

    /* Descend to the lo side.*/
    do
    {
      y = x;
      x = x->split[0];
    } while (x);

    /* Ascend until we can descend to the hi side.*/
    if (!y->split[1])  do
    {
      x = y;
      y = x->joint;
      side = side_of_BSTNode (x);
      lose (x);
    } while (y->joint && (side == 1 || !y->split[1]));

    y = (y->joint ? y->split[1] : 0);
  }
}

/**
 * Preorder, postorder, and inorder traversals are supported by
 * values of Nil, Yes, and May for {postorder} respectively.
 **/
  void
walk_BSTree (BSTree* t, Trit postorder,
    void (* f) (BSTNode*, void*), void* dat)
{
  BSTNode* y = root_of_BSTree (t);

  while (y)
  {
    BSTNode* x = y;
    Bit side = 0;

    /* Descend to the lo side.*/
    do
    {
      if (postorder == Nil)  f (x, dat);
      y = x;
      x = x->split[0];
    } while (x);

    /* Ascend until we can descend to the hi side.*/
    if (!y->split[1])  do
    {
      if (postorder == May && side == 0)  f (y, dat);
      x = y;
      y = x->joint;
      side = side_of_BSTNode (x);
      if (postorder == Yes)  f (x, dat);
    } while (y->joint && (side == 1 || !y->split[1]));

    if (postorder == May && y->joint)  f (y, dat);
    y = (y->joint ? y->split[1] : 0);
  }
}

  BSTNode*
find_BSTree (BSTree* t, const void* key)
{
  BSTNode* y = root_of_BSTree (t);

  while (!Nullish(y)) {
    Sign si = poset_cmp_lhs (t->cmp, key, y);
    if (si == 0) {return y;}
    y = SplitOf(y, (si < 0 ? 0 : 1));
  }
  return 0;
}

  void
insert_BSTree (BSTree* t, BSTNode* x)
{
  BSTNode* a = t->sentinel;
  BSTNode* y = root_of_BSTree (t);
  Bit side = side_of_BSTNode (y);

  while (y)
  {
    side = positive_bit (poset_cmp (t->cmp, x, y));
    a = y;
    y = y->split[side];
  }

  a->split[side] = x;
  x->joint = a;
  x->split[0] = 0;
  x->split[1] = 0;
}

/** If a node matching {x} exists, return that node.
 * Otherwise, add {x} to the tree and return it.
 **/
  BSTNode*
ensure_BSTree (BSTree* t, BSTNode* x)
{
  BSTNode* a = t->sentinel;
  BSTNode* y = root_of_BSTree (t);
  unsigned side = SideOf(y);

  while (!Nullish(y)) {
    Sign si = poset_cmp (t->cmp, x, y);
    if (si == 0)  return y;
    side = (si < 0 ? 0 : 1);
    a = y;
    y = SplitOf(y, side);
  }

  MaybeAssignSplit(a, side, x);
  AssignJoint(x, a);
  NullifySplit(x, 0);
  NullifySplit(x, 1);
  return x;
}

/** Remove a given node {y} from the tree.
 *
 * {y} will hold information about what had to be moved,
 * which is used by the red-black tree removal.
 * {y->joint} will hold {b} in the diagrams below.
 * {y->split} will hold the new split of {b} that changed depth.
 **/
  void
remove_BSTNode (BSTNode* y)
{
  unsigned side_y = SideOf(y);
  unsigned side;
#define oside (1-side)
  BSTNode* b;
  BSTNode* x;

  /* This is the only case that leaves {y->joint} unchanged.
   * {y} can be the root.
   * {x} can be null.
   **** OLD ********* NEW *** AUX ***
   *     b*            b*      b*
   *    / \           / \      |
   *  0*   y'  -->  0*   *x    y
   *      /                     \
   *    x*                       *x
   */
  for (side = 0; side < 2; ++side) {
    b = JointOf(y);

    if (Nullish(SplitOf(y, oside))) {
      x = SplitOf(y, side);
      if (Nullish(x)) {
        MaybeAssignSplit(b, side_y, x);
        return;
      }
      AssignJoint(x, b);
      /* Update {x} neighbors.*/
      AssignSplit(b, side_y, x);
      /* Populate {y} for return.*/
      /* noop: AssignJoint(y, b); */
      AssignSplit(y, side_y, x);
      NullifySplit(y, 1-side_y);
      return;
    }
  }

  /* We know {1} exists.
   ****** OLD ******** NEW ***** AUX ***
   *       y'           b         b
   *      / \          / \        |
   *     b   1  -->  0*   1       y
   *    /                        /
   *  0*                       0*
   */
  for (side = 0; side < 2; ++side) {
    b = SplitOf(y, side);

    if (Nullish(SplitOf(b, oside))) {
      AssignJoint(b, JointOf(y));
      AssignSplit(b, oside, SplitOf(y, oside));
      /* Update {b} neighbors.*/
      MaybeAssignSplit(JointOf(y), side_y, b);
      AssignJoint(SplitOf(y, oside), b);
      /* Populate {y} for return.*/
      AssignJoint(y, b);
      AssignSplit(y, side, SplitOf(b, side));
      NullifySplit(y, oside);
      return;
    }
  }

  /* We know both {0} and {3} exist.
   * {x} descends from {0} or {3} to be closest in value to {y}.
   * {b} follows one step behind {x}.
   **** OLD ******** NEW **** AUX ***
   *     y'           x        b
   *    / \          / \       |
   *  0*   3       0*   3      y
   *   .\.    -->   .\.         \
   *     b            b          *2
   *    / \          / \
   *  1*   x       1*   *2
   *      /
   *    2*
   */
  side = 0;  /* Arbitrary side.*/
  b = SplitOf(y, side);
  x = SplitOf(b, oside);
  do {
    b = x;
    x = SplitOf(b, oside);
  } while (!Nullish(x));
  x = b;
  b = JointOf(x);

  assert(oside == SideOf(x));
  Join(b, oside, SplitOf(x, side));

  AssignJoint(x, JointOf(y));
  AssignSplit(JointOf(y), side_y, x);
  Join(x, 0, SplitOf(y, 0));
  Join(x, 1, SplitOf(y, 1));

  /* Populate {y} for return.*/
  AssignJoint(y, b);
  NullifySplit(y, side);
  AssignSplit(y, oside, SplitOf(b, oside));
#undef oside
}

/** Do a tree rotation,
 *
 *       b             a
 *      / \           / \
 *     a   *z  -->  x*   b
 *    / \               / \
 *  x*   *y           y*   *z
 *
 * When {side} is 1, opposite direction when 0.
 * ({b} always starts as {a}'s joint)
 **/
  void
rotate_up_BSTNode(BSTNode* a)
{
  BSTNode* b = JointOf(a);
  const unsigned side = SideOf(a);
  const unsigned oside = 1-side;
  BSTNode* y = SplitOf(a, oside);

  Join(JointOf(b), SideOf(b), a);
  Join(a, oside, b);
  Join(b, side, y);
}

