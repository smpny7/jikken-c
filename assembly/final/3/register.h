#define MAX_REG 23   /* レジスタ数 */
#define MAX_SAVE 120 /* 退避領域数 */

#define FREE_REG -1
#define NO_FREE_REG -1

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
