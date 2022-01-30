#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"
#include "codegen.h"
#include "explore.h"
#include "register.h"
#include "generate.h"
#include "list.h"

extern int yyerror();

int symbol_offset = 0;
int if_label_cnt = 0;
int for_label_cnt = 0;
int while_label_cnt = 0;

int isExpressionNodeType(NodeType nType)
{
    return nType == Add_AST || nType == Sub_AST || nType == Mul_AST || nType == Div_AST || nType == Mod_AST;
}

Symbol *getSymbolFromVarName(char *varName, Symbol *sp)
{
    if (strcmp(varName, sp->varName) == 0)
        return sp;

    if (sp->next)
        return getSymbolFromVarName(varName, sp->next);

    fprintf(stderr, "Variable name is not registered in the symbol table\n");
    exit(1);
}

int getArraySize(ArrayIndex *ip)
{
    if (ip->next != NULL)
        return ip->size * getArraySize(ip->next);

    return ip->size;
}

int getIdentOffset(Node *np, Symbol *symbolTable)
{
    return getSymbolFromVarName(np->varName, symbolTable)->offset;
}

void replaceForArrayOffset(Node *np, ArrayIndex *ip)
{
    int mul_op = 1;
    ArrayIndex *tmp_ip = ip;

    while (tmp_ip->next != NULL)
    {
        tmp_ip = tmp_ip->next;
        mul_op *= tmp_ip->size;
    }

    Node *np_brother_tmp;
    switch (np->child->nType)
    {
    case Number_AST:
        np_brother_tmp = np->child->brother;
        np->child = build_node2(Mul_AST, build_num_node(Number_AST, np->child->value), build_num_node(Number_AST, mul_op));
        np->child->brother = np_brother_tmp;
        break;
    case Ident_AST:
        np_brother_tmp = np->child->brother;
        np->child = build_node2(Mul_AST, build_ident_node(Ident_AST, np->child->varName), build_num_node(Number_AST, mul_op));
        np->child->brother = np_brother_tmp;
        break;

    default:
        break;
    }

    np->nType = Add_AST;

    if (np->child->brother != NULL)
        replaceForArrayOffset(np->child->brother, ip->next);
    else
        np->child->brother = build_num_node(Number_AST, 0);
}

int codegen(Node *top)
{
    freeAllReg();
    Symbol *symbolTable = NULL;

    genInitialize();
    genStartDataSegment();

    symbolTable = exploreDeclsTree(top->child, symbolTable);

    genStartTextSegment();
    exploreStatsTree(top->child->brother, symbolTable);

    genFinalize();

    return 0;
}
