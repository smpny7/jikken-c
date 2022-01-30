#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"
#include "codegen.h"
#include "explore.h"
#include "register.h"
#include "generate.h"
#include "list.h"

extern int if_label_cnt;
extern int for_label_cnt;
extern int while_label_cnt;

void genInitialize()
{
    printf("\tINITIAL_GP = 0x10008000\t# initial value of global pointer\n");
    printf("\tINITIAL_SP = 0x7ffffffc\t# initial value of stack pointer\n");
    printf("\tstop_service = 99\t# system call service number\n\n");

    printf("\t.text\t\t\t# テキストセグメントの開始\n");
    printf("init:\n");
    printf("\t# initialize $gp (global pointer) and $sp (stack pointer)\n");
    printf("\tla $gp, INITIAL_GP\t# $gp ←0x10008000 (INITIAL_GP)\n");
    printf("\tla $sp, INITIAL_SP\t# $sp ←0x7ffffffc (INITIAL_SP)\n");
    printf("\tjal main\t\t# jump to 'main'\n");
    printf("\tnop\t\t\t# (delay slot)\n");
    printf("\tli $v0, stop_service\t# $v0 ←99 (stop_service)\n");
    printf("\tsyscall\t\t\t# stop\n");
    printf("\tnop\n");
    printf("\t# not reach here\n");
    printf("\tla $sp, INITIAL_SP\t# $sp ←0x7ffffffc (INITIAL_SP)\n\n");

    printf("stop:\t# if syscall return\n");
    printf("\tj stop\t\t\t# infinite loop...\n");
    printf("\tnop\t\t\t# (delay slot)\n\n");
}

void genStartDataSegment()
{
    printf("\t#\n");
    printf("\t# data segment\n");
    printf("\t#\n");
    printf("\t.data 0x10004000\t# データセグメントの開始\n");
    printf("RESULT:\n");
}

void genStartTextSegment()
{
    printf("\n\t#\n");
    printf("\t# text segment\n");
    printf("\t#\n");
    printf("\t.text 0x00001000\t# 以降のコードを 0から配置 x00001000\n");
    printf("main:\n");
    printf("\tla\t$t0, RESULT\t\t# $t0 ←0x10004000\n");
}

void genFinalize()
{
    printf("\n$EXIT:\n");
    printf("\tjr $ra\n");
    printf("\tnop # (delay slot)\n");
}

void genCalc(ThreeAddr *threeAddrTable, NodeType nType, Symbol *symbolTable)
{
    int i;
    RegIndex r1, r2;

    if (threeAddrTable->n_opd1 != NULL)
    {
        threeAddrTable->r_opd1 = vr_reg_end;
        addSaveReg(vr_reg_end++);

        r1 = getFreeReg(threeAddrTable->r_opd1);

        while (r1 == NO_FREE_REG)
        {
            for (i = 0; i < MAX_REG; i++)
                if (regState[i] != threeAddrTable->result && regState[i] != threeAddrTable->r_opd1)
                    saveReg(i);
            r1 = getFreeReg(threeAddrTable->r_opd1);
        }
    }
    else
    {
        r1 = getAssignedRegister(threeAddrTable->r_opd1);

        while (r1 == NO_FREE_REG)
        {
            for (i = 0; i < MAX_REG; i++)
                if (regState[i] != threeAddrTable->result && regState[i] != threeAddrTable->r_opd1)
                    saveReg(i);
            r1 = getAssignedRegister(threeAddrTable->r_opd1);
        }
    }

    if (threeAddrTable->n_opd2 != NULL)
    {
        threeAddrTable->r_opd2 = vr_reg_end;
        addSaveReg(vr_reg_end++);
        r2 = getFreeReg(threeAddrTable->r_opd2);

        while (r2 == NO_FREE_REG)
        {
            for (i = 0; i < MAX_REG; i++)
                if (regState[i] != threeAddrTable->result && regState[i] != threeAddrTable->r_opd1 && regState[i] != threeAddrTable->r_opd2)
                    saveReg(i);
            r2 = getFreeReg(threeAddrTable->r_opd2);
        }
    }
    else
    {
        r2 = getAssignedRegister(threeAddrTable->r_opd2);

        while (r2 == NO_FREE_REG)
        {
            for (i = 0; i < MAX_REG; i++)
                if (regState[i] != threeAddrTable->result && regState[i] != threeAddrTable->r_opd1 && regState[i] != threeAddrTable->r_opd2)
                    saveReg(i);
            r2 = getAssignedRegister(threeAddrTable->r_opd2);
        }
    }

    if (threeAddrTable->n_opd1 != NULL)
        genTerminationNodeLoad(threeAddrTable->n_opd1, regName[r1], symbolTable);
    if (threeAddrTable->n_opd2 != NULL)
        genTerminationNodeLoad(threeAddrTable->n_opd2, regName[r2], symbolTable);

    freeReg(r1);
    freeReg(r2);

    if (threeAddrTable->result == FREE_REG)
        exit(1);
    assignRegFromVR(threeAddrTable->result, r1);

    switch (nType)
    {
    case Add_AST:
        printf("\tadd\t%s, %s, %s\n", threeAddrTable->next != NULL ? regName[r1] : "$v0", regName[r1], regName[r2]);
        break;
    case Sub_AST:
        printf("\tsub\t%s, %s, %s\n", threeAddrTable->next != NULL ? regName[r1] : "$v0", regName[r1], regName[r2]);
        break;
    case Mul_AST:
        printf("\tmult\t%s, %s\n", regName[r1], regName[r2]);
        printf("\tmflo\t%s\n", threeAddrTable->next != NULL ? regName[r1] : "$v0");
        break;
    case Div_AST:
        printf("\tdiv\t%s, %s\n", regName[r1], regName[r2]);
        printf("\tmflo\t%s\n", threeAddrTable->next != NULL ? regName[r1] : "$v0");
        break;
    case Mod_AST:
        printf("\tdiv\t%s, %s\n", regName[r1], regName[r2]);
        printf("\tmfhi\t%s\n", threeAddrTable->next != NULL ? regName[r1] : "$v0");
        break;

    default:
        break;
    }
}

void genCalcs(ThreeAddr *threeAddrTable, Symbol *symbolTable)
{
    if (isExpressionNodeType(threeAddrTable->nType))
        genCalc(threeAddrTable, threeAddrTable->nType, symbolTable);

    if (threeAddrTable->next != NULL)
        genCalcs(threeAddrTable->next, symbolTable);
}

void genExpression(Node *np, Symbol *symbolTable)
{
    if (isExpressionNodeType(np->nType))
    {
        ThreeAddr *threeAddrTable = NULL;
        threeAddrTable = exploreExpressionTree(np, threeAddrTable);
        genCalcs(threeAddrTable, symbolTable);
    }
    else
    {
        switch (np->nType)
        {
        case Pre_Increment_AST:
        case Post_Increment_AST:
            printf("\tlw\t$v0, %d($t0)\n\tnop\n", getIdentOffset(np->child, symbolTable));
            printf("\taddi\t$v0, $v0, 1\n");
            printf("\tsw\t$v0, %d($t0)\n\tnop\n", getIdentOffset(np->child, symbolTable));
            break;
        case Pre_Decrement_AST:
        case Post_Decrement_AST:
            printf("\tlw\t$v0, %d($t0)\n\tnop\n", getIdentOffset(np->child, symbolTable));
            printf("\taddi\t$v0, $v0, -1\n");
            printf("\tsw\t$v0, %d($t0)\n\tnop\n", getIdentOffset(np->child, symbolTable));
            break;

        default:
            genTerminationNodeLoad(np, "$v0", symbolTable);
            break;
        }
    }
}

// 注意: 副作用で np->child 以下の木構造変化（効率化）
void genArrayElOffset(Node *np, ArrayIndex *ip, Symbol *symbolTable)
{
    replaceForArrayOffset(np->child, ip);
    genExpression(np->child, symbolTable);
    printf("\taddi\t$t1, $zero, 4\t# $t1 = 4\n");
    printf("\tmult\t$v0, $t1\t# $v0 *= 4\n");
    printf("\tmflo\t$v0\n");
    printf("\taddi $v0, $v0, %d\n", getIdentOffset(np, symbolTable));
}

void genTerminationNodeLoad(Node *np, char *regLabel, Symbol *symbolTable)
{
    Symbol *sp;
    switch (np->nType)
    {
    case Ident_AST:
        printf("\tlw\t%s, %d($t0)\n\tnop\n", regLabel, getIdentOffset(np, symbolTable));
        break;

    case Number_AST:
        printf("\tori\t%s, $zero, %d\t# %s ← %d\n", regLabel, np->value, regLabel, np->value);
        break;

    case ArrayEl_AST:
        sp = getSymbolFromVarName(np->varName, symbolTable);
        genArrayElOffset(np, sp->index, symbolTable);
        printf("\tadd\t$t1, $t0, $v0\t# $t1 = $t0 + $v0\n");
        printf("\tlw\t%s, 0($t1)\n\tnop\n", regLabel);
        break;

    default:
        break;
    }
}

void genCondition(Node *np, Symbol *symbolTable, char *label)
{
    genExpression(np->child, symbolTable);
    printf("\tadd $v1, $zero, $v0\t# $v1 = $v0\n");

    genExpression(np->child->brother, symbolTable);

    switch (np->nType)
    {
    case Eq_AST:
        printf("\tbne $v0, $v1, %s\n", label);
        break;

    case LtoE_AST:
        printf("\taddi $v0, $v0, 1\n");
    case Lt_AST:
        printf("\tslt $t2, $v1, $v0\n");
        printf("\tbeq $t2, $zero, %s\n", label);
        break;

    case GtoE_AST:
        printf("\taddi $v1, $v1, 1\n");
    case Gt_AST:
        printf("\tslt $t2, $v0, $v1\n");
        printf("\tbeq $t2, $zero, %s\n", label);
        break;

    default:
        fprintf(stderr, "不明な比較演算子が入力されました.\n");
        exit(1);
    }
}

void genAssignment(Node *np, Symbol *symbolTable)
{
    switch (np->child->nType)
    {
    case Ident_AST:
        genExpression(np->child->brother, symbolTable);
        printf("\tsw\t$v0, %d($t0)\n\tnop\n", getIdentOffset(np->child, symbolTable));
        break;

    case ArrayEl_AST:
        genExpression(np->child->brother, symbolTable);
        printf("\tadd $v1, $zero, $v0\t# $v1 = $v0\n");
        Symbol *sp = getSymbolFromVarName(np->child->varName, symbolTable);
        genArrayElOffset(np->child, sp->index, symbolTable);
        printf("\tadd\t$t1, $t0, $v0\t# $t1 = $t0 + $v0\n");
        printf("\tsw\t$v1, 0($t1)\n\tnop\n");
        break;

    default:
        break;
    }
}

void genIfBranch(Node *np, Symbol *symbolTable)
{
    int n_if_label_cnt = if_label_cnt++;
    char else_label[MAXBUF] = {0};
    char exit_label[MAXBUF] = {0};
    snprintf(else_label, MAXBUF, "$IF%d_ELSE", n_if_label_cnt);
    snprintf(exit_label, MAXBUF, "$IF%d_EXIT", n_if_label_cnt);

    printf("\n$IF%d:\n", n_if_label_cnt);

    genCondition(np->child, symbolTable, np->child->brother->brother == NULL ? exit_label : else_label);
    printf("\tnop\n");
    exploreStatsTree(np->child->brother, symbolTable);
    if (np->child->brother->brother != NULL)
    {
        printf("\tj\t%s\n\tnop\n\n", exit_label);
        printf("%s:\n", else_label);
        exploreStatsTree(np->child->brother->brother, symbolTable);
    }
    printf("\n%s:\n", exit_label);
}

void genForLoop(Node *np, Symbol *symbolTable)
{
    int n_for_label_cnt = for_label_cnt++;
    char exit_label[MAXBUF] = {0};
    snprintf(exit_label, MAXBUF, "$FOR%d_EXIT", n_for_label_cnt);

    genAssignment(np->child, symbolTable);
    printf("\n$FOR%d:\n", n_for_label_cnt);
    genCondition(np->child->brother, symbolTable, exit_label);
    printf("\tnop\n");
    exploreStatsTree(np->child->brother->brother->brother, symbolTable);
    genExpression(np->child->brother->brother, symbolTable);
    printf("\tj\t$FOR%d\n\tnop\n\n", n_for_label_cnt);

    printf("%s:\n", exit_label);
}

void genWhileLoop(Node *np, Symbol *symbolTable)
{
    int n_while_label_cnt = while_label_cnt++;
    char exit_label[MAXBUF] = {0};
    snprintf(exit_label, MAXBUF, "$WHILE%d_EXIT", n_while_label_cnt);

    printf("\n$WHILE%d:\n", n_while_label_cnt);
    genCondition(np->child, symbolTable, exit_label);
    printf("\tnop\n");
    exploreStatsTree(np->child->brother, symbolTable);
    printf("\tj\t$WHILE%d\n\tnop\n\n", n_while_label_cnt);

    printf("%s:\n", exit_label);
}
