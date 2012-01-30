
#include <stdio.h>
#include <stdlib.h>

#include "table.h"
#include "bstree.h"

DeclTableT( uint, uint );


typedef struct ASTree ASTree;
typedef struct AST AST;

DeclTableT( char, char );
DeclTableT( AST, AST );

typedef
enum SyntaxKind
{    Function
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
};

struct AST
{
    SyntaxKind kind;
    uint ascend_idx;
    Table_AST* ascend_table;
    Table( AST ) descend;
        /* Should just have 'BSTNode bst;' */
    union AST_union {
        char a_char;
        int a_int;
        float a_float;
        double a_double;
        char* a_string;
        void* a_other;
    } dat;
};

    void
init_AST (AST* ast)
{
    ast->kind = NSyntaxKinds;
    ast->ascend_idx = 0;
    ast->ascend_table = 0;
    InitTable( AST, ast->descend );
    ast->dat.a_other = 0;
}

    void
lose_AST (AST* ast)
{
    uint i;
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

    for (i = 0; i < ast->descend.sz; ++i)
        lose_AST (&ast->descend.s[i]);
    LoseTable( AST, ast->descend );
}

    void
output_AST (FILE* out, const AST* ast)
{
    char txt[100];
    uint txtsz = 0;

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
        fputc ('(', out);
        output_AST (out, &ast->descend.s[0]);
        fputc ('+', out);
        output_AST (out, &ast->descend.s[1]);
        fputc (')', out);
        break;
    case Minus:
        fputc ('(', out);
        output_AST (out, &ast->descend.s[0]);
        fputc ('-', out);
        output_AST (out, &ast->descend.s[1]);
        fputc (')', out);
        break;
    default:
        fputs ("No Good!\n", stderr);
        break;
    };
    if (txtsz > 0)  fwrite (txt, sizeof(char), txtsz, out);
}

int main (int argc, char** argv)
{
    FILE* out = stdout;
    AST root;
    AST* ast = &root;
    init_AST (ast);

    GrowTable( AST, ast->descend, 2 );
    init_AST (&ast->descend.s[0]);
    init_AST (&ast->descend.s[1]);
    ast->kind = Plus;
    ast->descend.s[0].kind = Int;
    ast->descend.s[0].dat.a_int = 1;
    ast->descend.s[1].kind = Int;
    ast->descend.s[1].dat.a_int = 2;

    ast = &root;
    output_AST (out, ast);
    lose_AST (ast);
    return 0;
}

