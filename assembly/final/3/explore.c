#include <stdio.h>
#include "ast.h"
#include "codegen.h"
#include "explore.h"
#include "register.h"
#include "generate.h"
#include "list.h"

extern int symbol_offset;

ArrayIndex *exploreArrayTree(Node *np, ArrayIndex *ip)
{
    if (np->nType == ArrayIndex_AST && np->child->value)
        ip = arrayIndexListAdd(ip, np->child->value);

    if (np->child != NULL)
        ip = exploreArrayTree(np->child, ip);
    if (np->brother != NULL)
        ip = exploreArrayTree(np->brother, ip);

    return ip;
}

ThreeAddr *exploreExpressionTree(Node *np, ThreeAddr *tp)
{
    if (np->child != NULL && np->nType != ArrayEl_AST)
        tp = exploreExpressionTree(np->child, tp);

    if (isExpressionNodeType(np->nType))
    {
        int return_reg_num = vr_reg_end++;
        int left_reg_num = isExpressionNodeType(np->child->nType) ? np->child->reg ? np->child->reg : vr_reg_end++ : 0;
        int right_reg_num = isExpressionNodeType(np->child->brother->nType) ? np->child->brother->reg ? np->child->brother->reg : vr_reg_end++ : 0;

        tp = threeAddrListAdd(tp, np->nType, return_reg_num, left_reg_num, right_reg_num, !isExpressionNodeType(np->child->nType) ? np->child : NULL, !isExpressionNodeType(np->child->brother->nType) ? np->child->brother : NULL);
        np->reg = return_reg_num;
    }

    if (np->brother != NULL)
        tp = exploreExpressionTree(np->brother, tp);

    return tp;
}

Symbol *exploreDeclsTree(Node *np, Symbol *sp)
{
    ArrayIndex *ip = NULL;

    switch (np->nType)
    {
    case Define_AST:
        printf("\t.word 4\t\t# %s\n", np->child->varName);

        sp = symbolListAdd(sp, np->child->varName, symbol_offset, 1, NULL);
        symbol_offset += 4;
        break;

    case Array_AST:
        ip = exploreArrayTree(np->child->child, ip);
        int array_size = getArraySize(ip);

        printf("\t.space %d\t\t# %s\n", array_size * 4, np->child->varName);

        sp = symbolListAdd(sp, np->child->varName, symbol_offset, array_size * 4, ip);
        symbol_offset += 4 * array_size;
        break;

    default:
        break;
    }

    if (np->child != NULL)
        sp = exploreDeclsTree(np->child, sp);
    if (np->brother != NULL)
        sp = exploreDeclsTree(np->brother, sp);

    return sp;
}

void exploreStatsTree(Node *np, Symbol *symbolTable)
{
    switch (np->child->nType)
    {
    case Assign_AST:
        genAssignment(np->child, symbolTable);
        break;

    case For_AST:
        genForLoop(np->child, symbolTable);
        break;

    case While_AST:
        genWhileLoop(np->child, symbolTable);
        break;

    case If_AST:
        genIfBranch(np->child, symbolTable);
        break;

    default:
        break;
    }

    if (np->child->brother != NULL)
        exploreStatsTree(np->child->brother, symbolTable);
}
