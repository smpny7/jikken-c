#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"

extern int yylex();
extern int yyerror();
extern int yyparse();

Node *build_node2(NodeType nType, Node *np1, Node *np2)
{
    Node *np; // TODO 一行可能？
    np = (Node *)malloc(sizeof(Node));
    np->nType = nType;
    np->child = (struct Node *)np1;
    np1->brother = (struct Node *)np2;
    np->brother = NULL;
    return np;
}

Node *build_node3(NodeType nType, Node *np1, Node *np2, Node *np3)
{
    Node *np;
    np = (Node *)malloc(sizeof(Node));
    np->nType = nType;
    np->child = (struct Node *)np1;
    np1->brother = (struct Node *)np2;
    np2->brother = (struct Node *)np3;
    np->brother = NULL;
    return np;
}

Node *build_ident_node(NodeType nType, char *varName)
{
    Node *np;
    np = (Node *)malloc(sizeof(Node));
    np->nType = nType;
    np->varName = (char *)malloc(MAXBUF);
    strncpy(np->varName, varName, MAXBUF);
    np->child = NULL;
    np->brother = NULL;
    return np;
}

Node *build_num_node(NodeType nType, int value)
{
    Node *np;
    np = (Node *)malloc(sizeof(Node));
    np->nType = nType;
    np->value = value;
    np->child = NULL;
    np->brother = NULL;
    return np;
}

Node *build_array_node(NodeType nType, char *varName, int value)
{
    Node *np;
    np = (Node *)malloc(sizeof(Node));
    np->nType = nType;
    np->varName = (char *)malloc(MAXBUF);
    strncpy(np->varName, varName, MAXBUF);
    np->value = value;
    np->child = NULL;
    np->brother = NULL;
    return np;
}

void printTree(Node *np)
{
    /* ==  for debug  ================== */

    char *node_type_str[] = {
        "Pro_AST",    // プログラムのノード型
        "Decls_AST",  // 変数宣言部のノード型
        "Stats_AST",  // 文集合のノード型
        "Stat_AST",   // 文のノード型
        "Assign_AST", // 代入文のノード型
        "Add_AST",    // 加減のノード型
        "Mul_AST",    // 乗除のノード型
        "Ident_AST",  // 識別子のノード型
        "Number_AST", // 整数のノード型
        "ArryEl_AST", // 配列のノード型
        "While_AST",  // ループ文のノード型
        "If_AST",     // 条件分岐文のノード型
        "Cond_AST",   // 条件式のノード型
    };

    /* ================================= */

    printf("%s", node_type_str[np->nType]);
    if (np->varName && np->value)
        printf("[= %s[%d]]", np->varName, np->value);
    else if (np->varName)
        printf("[= %s]", np->varName);
    else if (np->value)
        printf("[= %d]", np->value);

    if (np->child != NULL)
    {
        printf("(");
        printTree((Node *)np->child);
        printf(")");
    }
    if (np->brother != NULL)
    {
        printf(" ");
        printTree((Node *)np->brother);
    }
}

// int main(void)
// {
//     Node *top;
//     Node *p1;
//     Node *p2;
//     p1 = build_node2(Add_AST,
//                      build_array_node(ArryEl_AST, "a", 2),
//                      build_num_node(Number_AST, 1));
//     p2 = build_node3(Add_AST,
//                      build_array_node(ArryEl_AST, "a", 20),
//                      build_num_node(Number_AST, 10),
//                      build_ident_node(Ident_AST, "x"));
//     top = build_node3(Assign_AST,
//                       build_ident_node(Ident_AST, "x"),
//                       p1,
//                       p2);
//     printTree(top);
//     printf("\n");
//     return 0;
// }

int main(void)
{
    if (yyparse())
    {
        fprintf(stderr, "Error\n");
        return 1;
    }

    printTree(top);
    printf("\n");

    return 0;
}
