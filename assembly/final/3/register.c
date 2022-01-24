#include <stdio.h>
#include <stdlib.h>
#include "ast.h"
#include "register.h"
#include "list.h"

int regState[MAX_REG];      /* レジスタと仮想レジスタの紐付け */
int regSaveState[MAX_SAVE]; /* 退避領域と仮想レジスタの紐付け */

char *regName[MAX_REG] = {"$t1", "$t2", "$t3"};

RegIndex vr_reg_end = 0;
RegIndex reg_save_end = 0;

/*
* Overview: 仮想レジスタをレジスタに割り当てる.
* @argument: {VRReg} vr_reg - 仮想レジスタ.
* @return: {RegIndex} - 割り当てたレジスタのインデックス / 空きレジスタなし.
*/
RegIndex getFreeReg(VRReg vr_reg)
{
    int i;
    for (i = 0; i < MAX_REG; i++)
    {
        if (regState[i] == FREE_REG)
        {
            regState[i] = vr_reg;
            return i;
        }
    }
    return NO_FREE_REG;
}

/*
* Overview: 仮想レジスタが割り当てられているレジスタを返す.退避状態の場合はロードして返す.
* @argument: {VRReg} vr_reg - 仮想レジスタ.
* @return: {RegIndex} - 割り当てられているレジスタのインデックス.
*/
RegIndex getAssignedRegister(VRReg vr_reg)
{
    int i;
    RegIndex restored_reg;

    for (i = 0; i < MAX_REG; i++)
        if (regState[i] == vr_reg)
            return i;

    /* 退避状態の場合 */
    for (i = 0; i < MAX_SAVE; i++)
    {
        if (regSaveState[i] == vr_reg)
        {
            restored_reg = getFreeReg(vr_reg);
            if (restored_reg == NO_FREE_REG)
                break;
            regSaveState[i] = FREE_REG;
            printf("\tlw\t%s, %d($sp)\n\tnop\n", regName[restored_reg], -reg_save_end-- * 4);
            return restored_reg;
        }
    }
    fprintf(stderr, "仮想レジスタまたは空きレジスタが存在しません.\n");
    exit(1);
}

/*
* Overview: レジスタを退避させる.
* @argument: {RegIndex} reg_index - 退避するレジスタ.
*/
void saveReg(RegIndex reg_index)
{
    int i;

    if (regState[reg_index] == FREE_REG)
        return;
    for (i = 0; i < MAX_SAVE; i++)
    {
        if (regSaveState[i] == FREE_REG)
        {
            printf("\tsw\t%s, %d($sp)\n\tnop\n", regName[reg_index], -++reg_save_end * 4);
            regSaveState[i] = regState[reg_index];
            regState[reg_index] = FREE_REG;
            return;
        }
    }
    fprintf(stderr, "退避領域に空きスペースがありません.\n");
    exit(1);
}

/*
* Overview: 仮想レジスタを指定のレジスタに割り当てる.
* @argument: {VRReg} vr_reg - 割り当て元の仮想レジスタ.
* @argument: {RegIndex} reg_index - 割り当て先のレジスタ.
*/
void assignRegFromVR(VRReg vr_reg, RegIndex reg_index)
{
    if (regState[reg_index] == vr_reg)
        return;
    saveReg(reg_index);
    regState[reg_index] = vr_reg;
}

/*
* Overview: レジスタを解放する.
* @argument: {RegIndex} reg_index - 解放するレジスタ.
*/
void freeReg(RegIndex reg_index)
{
    regState[reg_index] = FREE_REG;
}

/*
* Overview: 全てのレジスタ・仮想レジスタ・退避領域を解放する.
*/
void freeAllReg()
{
    int i;
    for (i = 0; i < MAX_REG; i++)
        regState[i] = FREE_REG;
    for (i = 0; i < MAX_SAVE; i++)
        regSaveState[i] = FREE_REG;

    vr_reg_end = 0;
    reg_save_end = 0;
}

/*
* Overview: 3番地コードから仮想レジスタを生成・格納する.
* @argument: {ThreeAddr} ta - 3番地コード.
*/
void setRegs(ThreeAddr *ta)
{
    int reg_num;
    for (reg_num = 0; reg_num < vr_reg_end; reg_num++)
        regSaveState[reg_num] = reg_num;
}

/*
* Overview: 新規仮想レジスタを追加する.
* @argument: {VRReg} vr_reg - 仮想レジスタのインデックス.
*/
void addSaveReg(VRReg vr_reg)
{
    int i;

    for (i = 0; i < MAX_SAVE; i++)
        if (regSaveState[i] == FREE_REG)
        {
            regSaveState[i] = vr_reg;
            return;
        }
}
