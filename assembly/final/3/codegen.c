#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"
#include "register.h"
#include "generate.h"
#include "list.h"

extern void printTreeGUI();

extern int yyerror();

void printSP(Symbol *sp)
{
    printf("=================\n");
    printf("varName: %s\n", sp->varName);
    printf("offset: %d\n", sp->offset);
    printf("size: %d\n", sp->size);
    printf("=================\n\n");

    if (sp->next != NULL)
        printSP(sp->next);
}

void printIP(ArrayIndex *ip)
{
    printf("=================\n");
    printf("size: %d\n", ip->size);
    printf("=================\n\n");

    if (ip->next != NULL)
        printIP(ip->next);
}

void printREG()
{
    int i;
    printf("=================\n");
    printf("regState\n");
    printf("=================\n");
    for (i = 0; i < MAX_REG; i++)
        printf("%d: %d\n", i, regState[i]);
    printf("=================\n");
    printf("regSaveState\n");
    printf("=================\n");
    for (i = 0; i < MAX_SAVE; i++)
        printf("%d: %d\n", i, regSaveState[i]);
    printf("=================\n");
}

void exploreStatsTree(Node *np, Symbol *symbolTable);
void genIdentOrNumberLoad(Node *np, char *regLabel, Symbol *symbolTable);

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
        fprintf(stderr, "まだ続きの配列次元がありますよ\n");
        tmp_ip = tmp_ip->next;
        mul_op *= tmp_ip->size;
    }

    Node *np_brother_tmp;
    switch (np->child->nType)
    {
    case Number_AST:
        fprintf(stderr, "Number_AST\n");
        np_brother_tmp = np->child->brother;
        np->child = build_node2(Mul_AST, build_num_node(Number_AST, np->child->value), build_num_node(Number_AST, mul_op));
        np->child->brother = np_brother_tmp;
        break;
    case Ident_AST:
        fprintf(stderr, "Ident_AST\n");
        np_brother_tmp = np->child->brother;
        np->child = build_node2(Mul_AST, build_ident_node(Ident_AST, np->child->varName), build_num_node(Number_AST, mul_op));
        np->child->brother = np_brother_tmp;
        break;

    default:
        fprintf(stderr, "OTHERS\n");
        break;
    }

    np->nType = Add_AST;

    if (np->child->brother != NULL)
    {
        fprintf(stderr, "次へ行く\n");
        replaceForArrayOffset(np->child->brother, ip->next);
    }
    else
    {
        np->child->brother = build_num_node(Number_AST, 0);
    }
    fprintf(stderr, "おわり\n");
}

int getArraySize(ArrayIndex *ip)
{
    if (ip->next != NULL)
        return ip->size * getArraySize(ip->next);

    return ip->size;
}

// int getArrayDimensionSize(ArrayIndex *ip)
// {
//     if (ip->next != NULL)
//         return getArrayDimensionSize(ip->next) + 1;

//     return 1;
// }

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

void genCalc(ThreeAddr *threeAddrTable, NodeType nType, Symbol *symbolTable)
{
    int i;
    RegIndex r1, r2;

    // printREG();

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
        genIdentOrNumberLoad(threeAddrTable->n_opd1, regName[r1], symbolTable);
    if (threeAddrTable->n_opd2 != NULL)
        genIdentOrNumberLoad(threeAddrTable->n_opd2, regName[r2], symbolTable);

    freeReg(r1);
    freeReg(r2);

    if (threeAddrTable->result == FREE_REG)
        exit(1);
    assignRegFromVR(threeAddrTable->result, r1);

    // printREG();

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

// extern char *node_type_str[];

// void printTA(ThreeAddr *ta)
// {
//     printf("=================\n");
//     printf("type: %s\n", node_type_str[ta->nType]);
//     printf("result: %d\n", ta->result);
//     printf("r_opd1: %d\n", ta->r_opd1);
//     printf("r_opd2: %d\n", ta->r_opd2);
//     if (ta->n_opd1 != NULL)
//         printf("n_opd1: %s\n", node_type_str[ta->n_opd1->nType]);
//     if (ta->n_opd1 != NULL && ta->n_opd1->value)
//         printf("value: %d\n", ta->n_opd1->value);
//     if (ta->n_opd1 != NULL && ta->n_opd1->varName != NULL)
//         printf("varName: %s\n", ta->n_opd1->varName);
//     if (ta->n_opd2 != NULL)
//         printf("n_opd2: %s\n", node_type_str[ta->n_opd2->nType]);
//     if (ta->n_opd2 != NULL && ta->n_opd2->value)
//         printf("value: %d\n", ta->n_opd2->value);
//     if (ta->n_opd2 != NULL && ta->n_opd2->varName != NULL)
//         printf("varName: %s\n", ta->n_opd2->varName);
//     printf("=================\n\n");

//     if (ta->next != NULL)
//         printTA(ta->next);
// }

void genExpression(Node *np, Symbol *symbolTable)
{
    if (isExpressionNodeType(np->nType))
    {
        ThreeAddr *threeAddrTable = NULL;
        threeAddrTable = exploreExpressionTree(np, threeAddrTable);
        // freeAllReg(); TODO: 多分不要
        // setRegs(threeAddrTable);
        // printTA(threeAddrTable);
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
            genIdentOrNumberLoad(np, "$v0", symbolTable);
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
    fprintf(stderr, "変数 %s の Offset は %d です\n", np->varName, getIdentOffset(np, symbolTable));
    printf("\taddi $v0, $v0, %d\n", getIdentOffset(np, symbolTable));
}

void genIdentOrNumberLoad(Node *np, char *regLabel, Symbol *symbolTable) //TODO: 名前変更
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
        printf("# ↓ 配列 %s の作成\n", np->varName);
        sp = getSymbolFromVarName(np->varName, symbolTable);
        genArrayElOffset(np, sp->index, symbolTable);
        printf("\tadd\t$t1, $t0, $v0\t# $t1 = $t0 + $v0\n");
        printf("\tlw\t%s, 0($t1)\n\tnop\n", regLabel);
        printf("# ↑ 配列 %s の作成\n", np->varName);
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
