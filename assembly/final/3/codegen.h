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

int isExpressionNodeType(NodeType nType);
Symbol *getSymbolFromVarName(char *varName, Symbol *sp);
int getArraySize(ArrayIndex *ip);
int getIdentOffset(Node *np, Symbol *symbolTable);
void replaceForArrayOffset(Node *np, ArrayIndex *ip);
