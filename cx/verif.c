
#include "fileb.h"
#include "rbtree.h"
#include "table.h"

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#if 1
char* strdup (const char* s)
{
    char* a = (char*) malloc ((strlen (s) + 1) * sizeof (char));
    strcpy (a, s);
    return a;
}
#endif

typedef struct TNode TNode;
struct TNode
{
    RBTNode rbt;
    char* key;
};

DeclTableT( TNode, TNode );

    Trit
swapped_TNode (const BSTNode* lhs, const BSTNode* rhs)
{
    const TNode* a = CastUp( TNode, rbt, CastUp( RBTNode, bst, lhs ) );
    const TNode* b = CastUp( TNode, rbt, CastUp( RBTNode, bst, rhs ) );
    int ret = strcmp (a->key, b->key);

        /* fprintf (stderr, "%s   %s\n", a->key, b->key); */

    if (ret < 0)  return Nil;
    if (ret > 0)  return Yes;
    return May;
}

    void
lose_TNode (BSTNode* x)
{
    TNode* a = CastUp( TNode, rbt, CastUp( RBTNode, bst, x ) );
    free (a->key);
}

    void
insert_TNode (RBTree* t, TNode* a)
{
        /* insert_BSTree (&t->bst, &a->rbt.bst); */
    insert_RBTree (t, &a->rbt);
}

    void
output_dot_fn (BSTNode* x, void* args)
{
    TNode* a = CastUp( TNode, rbt, CastUp( RBTNode, bst, x ) );
    TNode* b = CastUp( TNode, rbt, CastUp( RBTNode, bst, x->joint ) );
    FILE* out = (FILE*) ((void**)args)[0];
    TNode* all = (TNode*) ((void**)args)[1];

    fprintf (out, "q%u [label = \"%s\", color = \"%s\"];\n",
             (uint) IndexOf( TNode, all, a ),
             a->key,
             (a->rbt.red) ? "red" : "black");

    fprintf (out, "q%u -> q%u;\n",
             (uint) IndexOf( TNode, all, b ),
             (uint) IndexOf( TNode, all, a ));
}

    void
output_dot (BSTree* t)
{
    void* args[2];
    FILE* out = fopen ("out.dot", "wb");

    args[0] = out;
    args[1] = t->sentinel;

    fputs ("digraph tree {\n", out);
    traverse_BSTree (t, Yes, output_dot_fn, args);
    fputs ("}\n", out);
    fclose (out);
}

    TNode*
find_TNode (RBTree* t, TNode* a)
{
    BSTNode* x = find_BSTree (&t->bst, &a->rbt.bst);
    if (!x)  return 0;
    return CastUp( TNode, rbt, CastUp( RBTNode, bst, x ) );
}

    void
remove_TNode (RBTree* t, TNode* a)
{
    remove_RBTree (t, &a->rbt);
    free (a->key);
}

    void
testfn_skipws_FileB ()
{
    const char text[] = "hello i am\n some \n text! ";
    const char* const expect_text[] = {
        "hello", "i", "am", "some", "text!"
    };
    uint idx = 0;
    FileB st_in;
    FileB* const in = &st_in;
    FILE* out = stderr;

    init_FileB (in);
#if 0
    open_FileB (in, "", "test");
    in->f = fopen ("test", "rb");
#else
    in->buf.s = (byte*) strdup (text);
    in->buf.sz = sizeof(text);
    in->buf.alloc_sz = in->buf.sz;
#endif

    while ((getline_FileB (in)))
    {
        FileB st_olay;
        FileB* const olay = &st_olay;
        char* s;

        olay_FileB (olay, in);
        while ((s = nextok_FileB (olay, 0, 0)))
        {
            Claim2(idx ,<, ArraySz( expect_text ));
            Claim2(0 ,==, strcmp(expect_text[idx], s));
            ++ idx;
            fputs (s, out);
            fputc ('\n', out);
        }
    }

    lose_FileB (in);
}

int main ()
{
    RBTree rbtree;
    RBTree* t;
    TNode nodes[10];
    TNode* a;
    TNode* b;
    uint nodei = 0;
    char buf[100];

    testfn_skipws_FileB ();

    a = &nodes[nodei++];
    t = &rbtree;
    init_RBTree (t, &a->rbt, swapped_TNode);

    a = &nodes[nodei++];
    a->key = strdup ("hi");
    insert_TNode (t, a);

    a = &nodes[nodei++];
    a->key = strdup ("bye");
    insert_TNode (t, a);


    a = &nodes[nodei++];
    a->key = strdup ("why");
    insert_TNode (t, a);

    a = &nodes[nodei++];
    a->key = strdup ("sigh");
    insert_TNode (t, a);

    a = &nodes[nodei++];
    a->key = buf;
    strcpy (a->key, "why");

    fprintf (stderr, "------------\n");
    b = find_TNode (t, a);
    if (b)
    {
        printf ("Got: %s\n", b->key);
        remove_TNode (t, b);
    }

    output_dot (&t->bst);

    lose_BSTree (&t->bst, lose_TNode);

    return 0;
}

