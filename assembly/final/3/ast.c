#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"
#include "print_tree_gui.h"

extern int yylex();
extern int yyerror();
extern int yyparse();

#define IS_DEBUG 1

/*
* Overview: Create one child node.
* @argument: {NodeType} nType - Node type.
* @argument: {Node *} np1 - Child node.
* @return: {Node *} np - Parent node.
*/
Node *build_node1(NodeType nType, Node *np1)
{
    Node *np;
    if ((np = (Node *)malloc(sizeof(Node))) == NULL)
        yyerror("out of memory");
    np->nType = nType;
    np->child = (struct Node *)np1;
    np->brother = NULL;
    return np;
}

/*
* Overview: Create two child nodes.
* @argument: {NodeType} nType - Node type.
* @argument: {Node *} np1 - Child node.
* @argument: {Node *} np2 - Child node.
* @return: {Node *} np - Parent node.
*/
Node *build_node2(NodeType nType, Node *np1, Node *np2)
{
    Node *np;
    if ((np = (Node *)malloc(sizeof(Node))) == NULL)
        yyerror("out of memory");
    np->nType = nType;
    np->child = (struct Node *)np1;
    np1->brother = (struct Node *)np2;
    np->brother = NULL;
    return np;
}

/*
* Overview: Create three child nodes.
* @argument: {NodeType} nType - Node type.
* @argument: {Node *} np1 - Child node.
* @argument: {Node *} np2 - Child node.
* @argument: {Node *} np3 - Child node.
* @return: {Node *} np - Parent node.
*/
Node *build_node3(NodeType nType, Node *np1, Node *np2, Node *np3)
{
    Node *np;
    if ((np = (Node *)malloc(sizeof(Node))) == NULL)
        yyerror("out of memory");
    np->nType = nType;
    np->child = (struct Node *)np1;
    np1->brother = (struct Node *)np2;
    np2->brother = (struct Node *)np3;
    np->brother = NULL;
    return np;
}

/*
* Overview: Create three child nodes.
* @argument: {NodeType} nType - Node type.
* @argument: {Node *} np1 - Child node.
* @argument: {Node *} np2 - Child node.
* @argument: {Node *} np3 - Child node.
* @argument: {Node *} np4 - Child node.
* @return: {Node *} np - Parent node.
*/
Node *build_node4(NodeType nType, Node *np1, Node *np2, Node *np3, Node *np4)
{
    Node *np;
    if ((np = (Node *)malloc(sizeof(Node))) == NULL)
        yyerror("out of memory");
    np->nType = nType;
    np->child = (struct Node *)np1;
    np1->brother = (struct Node *)np2;
    np2->brother = (struct Node *)np3;
    np3->brother = (struct Node *)np4;
    np->brother = NULL;
    return np;
}

/*
* Overview: Create ident node.
* @argument: {NodeType} nType - Node type.
* @argument: {char *} varName - Variable name.
* @return: {Node *} np - Parent node.
*/
Node *build_ident_node(NodeType nType, char *varName)
{
    Node *np;
    if ((np = (Node *)malloc(sizeof(Node))) == NULL)
        yyerror("out of memory");
    np->nType = nType;
    np->varName = (char *)malloc(MAXBUF);
    strncpy(np->varName, varName, MAXBUF);
    np->child = NULL;
    np->brother = NULL;
    return np;
}

/*
* Overview: Create number node.
* @argument: {NodeType} nType - Node type.
* @argument: {int} value - Number.
* @return: {Node *} np - Parent node.
*/
Node *build_num_node(NodeType nType, int value)
{
    Node *np;
    if ((np = (Node *)malloc(sizeof(Node))) == NULL)
        yyerror("out of memory");
    np->nType = nType;
    np->value = value;
    np->child = NULL;
    np->brother = NULL;
    return np;
}

/*
* Overview: Create array node.
* @argument: {NodeType} nType - Node type.
* @argument: {char *} varName - Variable name.
* @argument: {int} value - Number.
* @return: {Node *} np - Parent node.
*/
Node *build_array_node(NodeType nType, char *varName, Node *np1)
{
    Node *np;
    if ((np = (Node *)malloc(sizeof(Node))) == NULL)
        yyerror("out of memory");
    np->nType = nType;
    np->varName = (char *)malloc(MAXBUF);
    strncpy(np->varName, varName, MAXBUF);
    np->child = (struct Node *)np1;
    np->brother = NULL;
    return np;
}

/*
* Overview: Output the tree structure.
* @argument: {Node *} np - Parent node.
* @return: No return
*/
void printTree(Node *np)
{
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
        printf("{");
        printTree((Node *)np->brother);
        printf("}");
    }
}

/*
* Overview: Outputs syntax errors and tree structure.
*/
int main(void)
{
    if (yyparse())
    {
        fprintf(stderr, "Error\n");
        return 1;
    }

    if (IS_DEBUG)
    {
        printTreeGUI(top);
        printf("\n");
    }

    return 0;
}
