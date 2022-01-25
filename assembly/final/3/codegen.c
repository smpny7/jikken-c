#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"
#include "register.h"
#include "generate.h"
#include "list.h"

extern int yyerror();
// extern char* node_type_str[];

void exploreStatsTree(Node *np, Symbol *symbolTable);

int symbol_offset = 0;
int if_label_cnt = 0;
int while_label_cnt = 0;

int isExpressionNodeType(NodeType nType)
{
    return nType == Add_AST || nType == Sub_AST || nType == Mul_AST || nType == Div_AST || nType == Mod_AST;
}

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

void printREG()
{
    int i;
    printf("=================\n");
    printf("regState\n");
    printf("=================\n");
    for (i = 0; i < MAX_REG; i++)
        printf("%d: %d\n", i, regState[i]);
    printf("=================\n");
    printf("vrRegState\n");
    printf("=================\n");
    for (i = 0; i < MAX_SAVE; i++)
        printf("%d: %d\n", i, regSaveState[i]);
    printf("=================\n");
}

int getOffset(Node *np, Symbol *sp)
{
    if (strcmp(np->varName, sp->varName) == 0)
        return sp->offset;
    if (sp->next)
        return getOffset(np, (Symbol *)sp->next);
    fprintf(stderr, "Variable name is not registered in the symbol table\n");
    exit(1);
}

int getArraySize(ArrayIndex *ip)
{
    if (ip->next != NULL)
        return ip->size * getArraySize(ip->next);

    return ip->size;
}

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
    if (np->child != NULL)
        tp = exploreExpressionTree(np->child, tp);

    if (isExpressionNodeType(np->nType))
    {
        int return_reg_num = vr_reg_end++;
        int left_reg_num = isExpressionNodeType(np->child->nType) ? np->child->reg ? np->child->reg : vr_reg_end++ : 0;
        int right_reg_num = isExpressionNodeType(np->child->brother->nType) ? np->child->brother->reg ? np->child->brother->reg : vr_reg_end++ : 0;

        // int has_left_expression_node = isExpressionNodeType(np->child->nType);
        // int has_right_expression_node = isExpressionNodeType(np->child->brother->nType);
        tp = threeAddrListAdd(tp, np->nType, return_reg_num, left_reg_num, right_reg_num, !isExpressionNodeType(np->child->nType) ? np->child : NULL, !isExpressionNodeType(np->child->brother->nType) ? np->child->brother : NULL);
        np->reg = return_reg_num;
        // vr_reg_end += has_left_expression_node + has_right_expression_node + 1;
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
        ip = exploreArrayTree(np, ip);
        // printIP(ip);
        int array_size = getArraySize(ip);

        printf("\t.space %d\t\t# %s\n", array_size * 4, np->child->varName);

        sp = symbolListAdd(sp, np->child->varName, symbol_offset, array_size, ip);
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

/*
• 代入文
• ループ文
• 条件分岐文
• 算術式　—- 別資料「算術式のコード生成」を参照のこと
• 条件式
*/

void genCalc(ThreeAddr *threeAddrTable, NodeType nType, Symbol *symbolTable)
{
    // printREG();

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
        // printf("左子ノードの計算結果仮想レジスタを代入\n");

        // printf("左子ノードの計算結果仮想レジスタを代入 (%d <- %d)\n", threeAddrTable->r_opd1, threeAddrTable->n_opd1->reg);
        // threeAddrTable->r_opd1 = threeAddrTable->n_opd1->reg;

        r1 = getAssignedRegister(threeAddrTable->r_opd1);

        while (r1 == NO_FREE_REG)
        {
            for (i = 0; i < MAX_REG; i++)
                if (regState[i] != threeAddrTable->result && regState[i] != threeAddrTable->r_opd1)
                    saveReg(i);
            r1 = getAssignedRegister(threeAddrTable->r_opd1);
        }
    }
    // printf("r1(%d)のレジスタインデックス: %d\n", threeAddrTable->r_opd1, r1);

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
        // printf("右子ノードの計算結果仮想レジスタを代入\n");

        // printf("右子ノードの計算結果仮想レジスタを代入 (%d <- %d)\n", threeAddrTable->r_opd2, threeAddrTable->n_opd2->reg);
        // threeAddrTable->r_opd2 = threeAddrTable->n_opd2->reg;

        r2 = getAssignedRegister(threeAddrTable->r_opd2);

        while (r2 == NO_FREE_REG)
        {
            for (i = 0; i < MAX_REG; i++)
                if (regState[i] != threeAddrTable->result && regState[i] != threeAddrTable->r_opd1 && regState[i] != threeAddrTable->r_opd2)
                    saveReg(i);
            r2 = getAssignedRegister(threeAddrTable->r_opd2);
        }
    }
    // printf("r2(%d)のレジスタインデックス: %d\n", threeAddrTable->r_opd2, r2);

    freeReg(r1);
    freeReg(r2);
    if (threeAddrTable->result < 0)
        return;
    assignRegFromVR(threeAddrTable->result, r1);
    if (threeAddrTable->n_opd1 != NULL)
    {
        if (threeAddrTable->n_opd1->varName != NULL)
        {
            printf("\tlw\t%s, %d($t0)\n\tnop\n", regName[r1], getOffset(threeAddrTable->n_opd1, symbolTable));
        }
        else
        {
            printf("\tori\t%s, $zero, %d\t# %s ← %d\n", regName[r1], threeAddrTable->n_opd1->value, regName[r1], threeAddrTable->n_opd1->value); // Add TODO: 数字だけ
        }
    }
    if (threeAddrTable->n_opd2 != NULL)
    {
        if (threeAddrTable->n_opd2->varName != NULL)
        {
            printf("\tlw\t%s, %d($t0)\n\tnop\n", regName[r2], getOffset(threeAddrTable->n_opd2, symbolTable));
        }
        else
        {
            printf("\tori\t%s, $zero, %d\t# %s ← %d\n", regName[r2], threeAddrTable->n_opd2->value, regName[r2], threeAddrTable->n_opd2->value); // Add TODO: 数字だけ
        }
    }

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
    {
        genCalc(threeAddrTable, threeAddrTable->nType, symbolTable);
        // printf("la\t$a2, 0x00000000\n");
        // printf("sw\t%s, 0($a2)\n", regName[getAssignedRegister(threeAddrTable->result)]);
        // printf("\tnop\n");
    }

    // printTA(threeAddrTable);

    if (threeAddrTable->next != NULL)
    {
        genCalcs(threeAddrTable->next, symbolTable);
    }
}

void genExpression(Node *np, Symbol *symbolTable)
{
    if (isExpressionNodeType(np->nType))
    {
        ThreeAddr *threeAddrTable = NULL;
        threeAddrTable = exploreExpressionTree(np, threeAddrTable);
        // printTA(threeAddrTable);
        freeAllReg();
        setRegs(threeAddrTable);
        // printf("DONE INIT\n");
        // printREG();
        genCalcs(threeAddrTable, symbolTable);
        // TODO: ここ2行かな？
        // if (np->brother != NULL)
        //     exploreStatsTree(np->brother, symbolTable);
    }
    else if (np->varName != NULL) // TODO: 変数or数字 関数化 sw lw 注意
    {
        printf("\tlw\t$v0, %d($t0)\n\tnop\n", getOffset(np, symbolTable));
    }
    else
    {
        printf("\tori\t$v0, $zero, %d\t# $v0 ← %d\n", np->value, np->value); // Add TODO: 数字だけ
    }
}

void genCondition(Node *np, Symbol *symbolTable, char *label)
{
    // code_generation_for_expression(child_node->brother);
    genExpression(np->child, symbolTable);
    printf("\tadd $v1, $zero, $v0\t# $v1 = $v0\n");

    // printf("la\t$a2, 0x00000000\n");
    // printf("sw\t%s, 0($a2)\n", "$v1");
    // printf("\tnop\n");

    genExpression(np->child->brother, symbolTable);

    // printf("la\t$a2, 0x00000000\n");
    // printf("sw\t%s, 0($a2)\n", "$v0");
    // printf("\tnop\n");

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
        printf("\taddi $v0, $v0, 1\n");
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
    // code_generation_for_expression(child_node->brother);
    // printf("\t(算術式)\n");
    genExpression(np->child->brother, symbolTable);
    printf("\tsw\t$v0, %d($t0)\n\tnop\n", getOffset(np->child, symbolTable));
}

void genIfBranch(Node *np, Symbol *symbolTable)
{
    int n_if_label_cnt = if_label_cnt++;
    // printf("\n\n===================（開始%d）===================\n\n", n_if_label_cnt);
    char else_label[MAXBUF] = {0};
    char exit_label[MAXBUF] = {0};
    snprintf(else_label, MAXBUF, "$IF%d_ELSE", n_if_label_cnt);
    snprintf(exit_label, MAXBUF, "$IF%d_EXIT", n_if_label_cnt);

    printf("\n$IF%d:\n", n_if_label_cnt);

    genCondition(np->child, symbolTable, np->child->brother->brother == NULL ? exit_label : else_label);
    printf("\tnop\n");
    // printf("\n\n（TRUE式）\n\n");
    exploreStatsTree(np->child->brother, symbolTable);
    // printf("\n\n（TRUE式 終わり）\n\n");
    if (np->child->brother->brother != NULL)
    {
        printf("\tj\t%s\n\tnop\n\n", exit_label);
        printf("%s:\n", else_label);
        exploreStatsTree(np->child->brother->brother, symbolTable);
    }
    printf("\n%s:\n", exit_label);
    // printf("\n\n===================（終了%d）===================\n\n", n_if_label_cnt);
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

    case While_AST:
        genWhileLoop(np->child, symbolTable);
        break;

    case If_AST:
        genIfBranch(np->child, symbolTable);
        break;

    default:
        break;
    }

    // if (np->child != NULL)
    //     exploreStatsTree(np->child, symbolTable);
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
