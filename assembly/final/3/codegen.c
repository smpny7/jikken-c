#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"

extern int yyerror();

typedef unsigned int Reg;

typedef struct three_addr ThreeAddr;
typedef struct three_addr
{
    NodeType nType;
    Reg result;
    Reg r_opd1;
    Reg r_opd2;
    Node *n_opd1;
    Node *n_opd2;
    ThreeAddr *next;
} ThreeAddr;

typedef struct array_index ArrayIndex;
typedef struct array_index
{
    int size;
    ArrayIndex *next;
} ArrayIndex;

typedef struct symbol_table Symbol;
typedef struct symbol_table
{
    char *varName;
    int offset;
    int size;
    Symbol *next;
    ArrayIndex *index;
} Symbol;

char *n_type[] = { // TODO: Debug
    "Pro_AST",
    "Decls_AST",
    "Stats_AST",
    "Define_AST",
    "Array_AST",
    "Ident_AST",
    "Number_AST",
    "ArrayEl_AST",
    "ArrayIndex_AST",
    "Assign_AST",
    "While_AST",
    "For_AST",
    "If_AST",
    "Add_AST",
    "Sub_AST",
    "Mul_AST",
    "Div_AST",
    "Mod_AST",
    "Eq_AST",
    "LtoE_AST",
    "GtoE_AST",
    "Lt_AST",
    "Gt_AST",
    "Pre_Increment_AST",
    "Pre_Decrement_AST",
    "Post_Increment_AST",
    "Post_Decrement_AST"};

void exploreStatsTree(Node *np, Symbol *symbolTable);

int symbol_offset = 0;
int while_label_cnt = 0;
Reg reg_cnt = 1;

void printTA(ThreeAddr *ta)
{
    printf("=================\n");
    printf("type: %s\n", n_type[ta->nType]);
    printf("result: %d\n", ta->result);
    printf("r_opd1: %d\n", ta->r_opd1);
    printf("r_opd2: %d\n", ta->r_opd2);
    if (ta->n_opd1 != NULL)
        printf("n_opd1: %s\n", n_type[ta->n_opd1->nType]);
    if (ta->n_opd1 != NULL && ta->n_opd1->value)
        printf("value: %d\n", ta->n_opd1->value);
    if (ta->n_opd1 != NULL && ta->n_opd1->varName != NULL)
        printf("varName: %s\n", ta->n_opd1->varName);
    if (ta->n_opd2 != NULL)
        printf("n_opd2: %s\n", n_type[ta->n_opd2->nType]);
    if (ta->n_opd2 != NULL && ta->n_opd2->value)
        printf("value: %d\n", ta->n_opd2->value);
    if (ta->n_opd2 != NULL && ta->n_opd2->varName != NULL)
        printf("varName: %s\n", ta->n_opd2->varName);
    printf("=================\n\n");

    if (ta->next != NULL)
        printTA(ta->next);
}

void printIP(ArrayIndex *ip)
{
    printf("=================\n");
    printf("size: %d\n", ip->size);
    printf("=================\n\n");

    if (ip->next != NULL)
        printIP(ip->next);
}

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

int isExpressionNode(Node *np)
{
    return np->nType == Add_AST || np->nType == Sub_AST || np->nType == Mul_AST || np->nType == Div_AST || np->nType == Mod_AST;
}

// int isTerminalSymbol(Node *np)
// {
//     return np->nType == Ident_AST || np->nType == Number_AST || np->nType == ArrayEl_AST;
// }

void printInitialize()
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

void printFinalize()
{
    printf("$EXIT:\n");
    printf("\tjr $ra\n");
    printf("nop # (delay slot)\n");
}

int getOffset(Node *np, Symbol *sp)
{
    if (strcmp(np->varName, sp->varName) == 0)
        return sp->offset;
    if (sp->next)
        return getOffset(np, (Symbol *)sp->next);
    printf("Variable name is not registered in the symbol table\n");
    exit(1);
}

int getArraySize(ArrayIndex *ip)
{
    if (ip->next != NULL)
        return ip->size * getArraySize(ip->next);

    return ip->size;
}

ArrayIndex *arrayIndexListAdd(ArrayIndex *ip, int size)
{
    ArrayIndex *n_ip;
    if ((n_ip = (ArrayIndex *)malloc(sizeof(ArrayIndex))) == NULL)
        yyerror("out of memory");

    n_ip->size = size;
    n_ip->next = NULL;

    if (ip == NULL)
        return n_ip;

    ArrayIndex *ip_end = ip;
    while (ip_end->next != NULL)
        ip_end = ip_end->next;
    ip_end->next = n_ip;
    return ip;
}

ThreeAddr *threeAddrListAdd(ThreeAddr *tp, NodeType nType, Reg result, Reg r_opd1, Reg r_opd2, Node *n_opd1, Node *n_opd2)
{
    ThreeAddr *n_tp;
    if ((n_tp = (ThreeAddr *)malloc(sizeof(ThreeAddr))) == NULL)
        yyerror("out of memory");

    n_tp->nType = nType;
    n_tp->result = result;
    n_tp->r_opd1 = r_opd1;
    n_tp->r_opd2 = r_opd2;
    n_tp->n_opd1 = n_opd1;
    n_tp->n_opd2 = n_opd2;
    n_tp->next = NULL;

    if (tp == NULL)
        return n_tp;

    ThreeAddr *tp_end = tp;
    while (tp_end->next != NULL)
        tp_end = tp_end->next;
    tp_end->next = n_tp;
    return tp;
}

Symbol *symbolListAdd(Symbol *sp, char *varName, int offset, int size, ArrayIndex *ip)
{
    Symbol *n_sp;
    if ((n_sp = (Symbol *)malloc(sizeof(Symbol))) == NULL)
        yyerror("out of memory");

    // TODO: ポインタだけでOK
    // n_sp->varName = (char *)malloc(MAXBUF);
    // strncpy(n_sp->varName, varName, MAXBUF);

    n_sp->varName = varName;

    n_sp->offset = offset;
    n_sp->size = size;
    n_sp->next = NULL;
    n_sp->index = ip;

    if (sp == NULL)
        return n_sp;

    Symbol *sp_end = sp;
    while (sp_end->next != NULL)
        sp_end = sp_end->next;
    sp_end->next = n_sp;
    return sp;
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

    if (isExpressionNode(np))
    {
        int has_left_expression_node = isExpressionNode(np->child);
        int has_right_expression_node = isExpressionNode(np->child->brother);
        tp = threeAddrListAdd(tp, np->nType, reg_cnt, has_left_expression_node ? reg_cnt + 1 : 0, has_right_expression_node ? reg_cnt + has_left_expression_node + 1 : 0, !has_left_expression_node ? np->child : NULL, !has_right_expression_node ? np->child->brother : NULL);
        reg_cnt += has_left_expression_node + has_right_expression_node + 1;
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
        printf("\t.word 10\t\t# %s\n", np->child->varName);

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

void genExpression(Node *np, Symbol *symbolTable)
{
    ThreeAddr *threeAddrTable = NULL;
    threeAddrTable = exploreExpressionTree(np, threeAddrTable);

    printTA(threeAddrTable);
}

void genCondition(Node *np, Symbol *symbolTable, char *label)
{
    // code_generation_for_expression(child_node->brother);
    printf("\t(算術式)\n");
    printf("\tadd $v1, $zero, $a0\t# $v1 = $v0\n");
    printf("\t(算術式)\n");
    switch (np->nType)
    {
    case Eq_AST:
        printf("\tbne $t1, $t3, %s\n", label);
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
        printf("ERROR\n");
        exit(1);
    }
}

void genAssignment(Node *np, Symbol *symbolTable)
{
    // code_generation_for_expression(child_node->brother);
    printf("\t(算術式)\n");
    printf("\tsw $v0, %d($t0)\n", getOffset(np->child, symbolTable));
}

void genWhileLoop(Node *np, Symbol *symbolTable)
{
    char label[MAXBUF] = {0};
    int n_while_label_cnt = while_label_cnt++;

    printf("\n$WHILE%d:\n", n_while_label_cnt);
    snprintf(label, MAXBUF, "$WHILE%d_EXIT", n_while_label_cnt);
    genCondition(np->child, symbolTable, label);
    printf("\tnop\n");
    exploreStatsTree(np->child->brother, symbolTable);
    printf("\tj $WHILE%d\n", n_while_label_cnt);
    printf("\tnop\n\n");

    printf("$WHILE%d_EXIT:\n", n_while_label_cnt);
}

void exploreStatsTree(Node *np, Symbol *symbolTable)
{
    switch (np->nType)
    {
    case Assign_AST:
        genAssignment(np, symbolTable);
        break;

    case While_AST:
        genWhileLoop(np, symbolTable);
        if (np->brother != NULL)
            exploreStatsTree(np->brother, symbolTable);
        return;

    case Add_AST:
    case Sub_AST:
    case Mul_AST:
    case Div_AST:
    case Mod_AST:
        genExpression(np, symbolTable);
        if (np->brother != NULL)
            exploreStatsTree(np->brother, symbolTable);
        return;

    default:
        break;
    }

    if (np->child != NULL)
        exploreStatsTree(np->child, symbolTable);
    if (np->brother != NULL)
        exploreStatsTree(np->brother, symbolTable);
}

int codegen(Node *top)
{
    Symbol *symbolTable = NULL;

    printInitialize();

    printf("\t#\n");
    printf("\t# data segment\n");
    printf("\t#\n");
    printf("\t.data 0x10004000\t# データセグメントの開始\n");
    printf("RESULT:\n");

    symbolTable = exploreDeclsTree(top->child, symbolTable);

    // printSP(symbolTable);

    printf("\n\t#\n");
    printf("\t# text segment\n");
    printf("\t#\n");
    printf("\t.text 0x00001000\t# 以降のコードを 0から配置 x00001000\n");
    printf("main:\n");
    printf("\tla $t0, RESULT\t\t# $t0 ←0x10004000\n");

    exploreStatsTree(top->child->brother, symbolTable);

    printf("\n\tjr $ra\n");
    printf("\tnop\t\t\t# (delay slot)\n");

    return 0;
}

// lw $t1, 4($t0) # $t1 ←mem[$t0 + 4
// ori $t2, $zero, 0 # $t2 ←0