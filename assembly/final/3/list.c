#include <stdio.h>
#include <string.h>
#include "ast.h"
#include "register.h"
#include "list.h"

extern int yyerror();

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

ThreeAddr *threeAddrListAdd(ThreeAddr *tp, NodeType nType, RegIndex result, RegIndex r_opd1, RegIndex r_opd2, Node *n_opd1, Node *n_opd2)
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
