#define MAX_REG 3   /* レジスタ数 */
#define MAX_SAVE 80 /* 退避領域数 */

#define FREE_REG -1
#define NO_FREE_REG -1

/* （記号表）配列のインデックス */
typedef struct array_index ArrayIndex;
typedef struct array_index
{
    int size;
    ArrayIndex *next;
} ArrayIndex;

/* 記号表 */
typedef struct symbol_table Symbol;
typedef struct symbol_table
{
    char *varName;
    int offset;
    int size;
    Symbol *next;
    ArrayIndex *index;
} Symbol;

/* 3番地コード */
typedef struct three_addr ThreeAddr;
typedef struct three_addr
{
    NodeType nType;
    RegIndex result;
    RegIndex r_opd1;
    RegIndex r_opd2;
    Node *n_opd1;
    Node *n_opd2;
    ThreeAddr *next;
} ThreeAddr;

extern int regState[MAX_REG];      /* レジスタと仮想レジスタの紐付け */
extern int regSaveState[MAX_SAVE]; /* 退避領域と仮想レジスタの紐付け */
extern char *regName[MAX_REG];     /* レジスタ名 */

extern RegIndex vr_reg_end;
extern RegIndex reg_save_end;

/* ================================================================== */

RegIndex getFreeReg(VRReg vr_reg);
RegIndex getAssignedRegister(VRReg vr_reg);
void saveReg(RegIndex reg_index);
void assignRegFromVR(VRReg vr_reg, RegIndex reg_index);
void freeReg(RegIndex reg_index);
void freeAllReg();
void setRegs(ThreeAddr *ta);
void addSaveReg(VRReg vr_reg);
