
#include "fileb.h"
#include "bstree.h"
#include "table.h"

#include <stdio.h>
#include <stdlib.h>

#ifndef TableT_uint
#define TableT_uint TableT_uint
DeclTableT( uint, uint );
#endif

typedef struct ASTree ASTree;
typedef struct AST AST;

typedef
enum SyntaxKind
{   ConsCell
    ,Function
    ,Char
    ,Int
    ,Float
    ,Double
    ,String
    ,Plus
    ,Minus
    ,NSyntaxKinds
} SyntaxKind;

struct ASTree
{
    BSTree bst;
    BSTNode sentinel;
};

struct AST
{
    SyntaxKind kind;
    BSTNode bst;
    union AST_union {
        char a_char;
        int a_int;
        float a_float;
        double a_double;
        char* a_string;
        void* a_other;
    } dat;
};

    AST*
side_of_AST (AST* ast, Bit side)
{
    BSTNode* bst = ast->bst.split[side];
    return CastUp( AST, bst, bst );
}

    void
set_side_AST (AST* a, AST* b, Bit side)
{
    a->bst.split[side] = &b->bst;
    b->bst.joint       = &a->bst;
}

    void
init_ASTree (ASTree* ast)
{
    init_BSTree (&ast->bst, &ast->sentinel, NULL);
}

    void
init_AST (AST* ast)
{
    ast->kind = NSyntaxKinds;
    ast->bst.split[0] = 0;
    ast->bst.split[1] = 0;
    ast->dat.a_other = 0;
}

static
    void
lose_AST (BSTNode* bst)
{
    AST* ast = CastUp( AST, bst, bst );
    switch (ast->kind)
    {
    case Char:
    case Int:
    case Float:
    case Double:
        break;
    default:
        if (ast->dat.a_other)
            free (ast->dat.a_other);
        break;
    }
}

    void
lose_ASTree (ASTree* ast)
{
    lose_BSTree (&ast->bst, lose_AST);
}

    void
dump_AST (FileB* f, const ASTree* t, const BSTNode* bst)
{
    char txt[100];
    uint txtsz = 0;
    const AST* ast = CastUp( AST, bst, bst );

    switch (ast->kind)
    {
    case Char:
            /* TODO: Special characters.*/
        txtsz = sprintf (txt, "'%c'", ast->dat.a_char);
        break;
    case Int:
        txtsz = sprintf (txt, "%d", ast->dat.a_int);
        break;
    case Float:
        txtsz = sprintf (txt, "%ff", ast->dat.a_float);
        break;
    case Double:
        txtsz = sprintf (txt, "%f", ast->dat.a_double);
        break;
    case Plus:
        dump_char_FileB (f, '(');
        dump_AST (f, t, bst->split[0]);
        dump_char_FileB (f, '+');
        dump_AST (f, t, bst->split[1]);
        dump_char_FileB (f, ')');
        break;
    case Minus:
        dump_char_FileB (f, '(');
        dump_AST (f, t, bst->split[0]);
        dump_char_FileB (f, '-');
        dump_AST (f, t, bst->split[1]);
        dump_char_FileB (f, ')');
        break;
    default:
        fputs ("No Good!\n", stderr);
        break;
    };
    if (txtsz > 0)
        dumpn_char_FileB (f, txt, txtsz);
}

    void
dump_ASTree (FileB* f, ASTree* t)
{
    dump_AST (f, t, root_of_BSTree (&t->bst));
}

int main (int argc, char** argv)
{
    DecloStack( FileB, f );
    ASTree tree;
    ASTree* t = &tree;
    AST nodes[10];
    AST* ast = &nodes[0];
    (void) argc;
    (void) argv;

    init_FileB (f);
    seto_FileB (f, true);
    set_FILE_FileB (f, stdout);

    init_ASTree (t);
    init_AST (ast);
    ast->kind = Plus;
    root_for_BSTree (&t->bst, &ast->bst);

    init_AST (++ ast);
    set_side_AST (ast-1, ast, 0);
    ast->kind = Int;
    ast->dat.a_int = 1;

    init_AST (++ ast);
    set_side_AST (ast-2, ast, 1);
    ast->kind = Minus;

    init_AST (++ ast);
    set_side_AST (ast-1, ast, 0);
    ast->kind = Int;
    ast->dat.a_int = 2;

    init_AST (++ ast);
    set_side_AST (ast-2, ast, 1);
    ast->kind = Int;
    ast->dat.a_int = 3;

    dump_ASTree (f, t);
    dump_char_FileB (f, '\n');
    lose_FileB (f);
    lose_ASTree (&tree);
    return 0;
}

