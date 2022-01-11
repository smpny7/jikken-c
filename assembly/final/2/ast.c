#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"

Node *build_node2(NodeType nType, Node *np1, Node *np2)
{
    Node *np;
    np = (Node *)malloc(sizeof(Node));
    np->nType = nType;
    np->child = (struct Node *)np1;
    np1->brother = (struct Node *)np2;
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
    printf("%s", node_type_str[np->nType]);
    if (np->varName && np->value)
        printf("[= %s[%d]]", np->varName, np->value);
    else if (np->varName)
        printf("[= %s]", np->varName);
    else if (np->value)
        printf("[= %d]", np->value);

    if (np->brother != NULL)
    {
        printf(" ");
        printTree((Node *)np->brother);
    }
    if (np->child != NULL)
    {
        printf("(");
        printTree((Node *)np->child);
        printf(")");
    }
}

int main(void)
{
    Node *top;
    Node *p1;
    p1 = build_node2(Add_AST,
                     build_array_node(ArryEl_AST, "a", 20),
                     build_num_node(Number_AST, 10));
    top = build_node2(Assign_AST,
                      build_ident_node(Ident_AST, "x"),
                      p1);
    printTree(top);
    printf("\n");
    return 0;
}
