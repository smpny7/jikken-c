#define MAXBUF 80

typedef enum
{
    Pro_AST,    // プログラムのノード型
    Decls_AST,  // 変数宣言部のノード型
    Stats_AST,  // 文集合のノード型
    Stat_AST,   // 文のノード型
    Assign_AST, // 代入文のノード型
    Add_AST,    // 加減のノード型
    Mul_AST,    // 乗除のノード型
    Factor_AST, // 因子のノード型
    Ident_AST,  // 識別子のノード型
    Number_AST, // 整数のノード型
    ArryEl_AST, // 配列のノード型
    While_AST,  // ループ文のノード型
    If_AST,     // 条件分岐文のノード型
    Cond_AST,   // 条件式のノード型
} NodeType;

typedef struct abstract_node
{
    NodeType nType;
    char *varName; // IDENT の場合その変数名を入れる
    int value;     // NUMBERの値や配列の添え字の値を入れる
    struct Node *child;
    struct Node *brother;
} Node;

/* ================================================================== */

Node *top;

Node *build_node1(NodeType nType, Node *np1);
Node *build_node2(NodeType nType, Node *np1, Node *np2);
Node *build_node3(NodeType nType, Node *np1, Node *np2, Node *np3);
Node *build_ident_node(NodeType nType, char *varName);
Node *build_num_node(NodeType nType, int value);
Node *build_array_node(NodeType nType, char *varName, int value);
