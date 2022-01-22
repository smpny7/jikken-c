#define MAXBUF 80

typedef enum
{
    Pro_AST,   // プログラムのノード型
    Decls_AST, // 変数宣言部のノード型
    Stats_AST, // 文集合のノード型

    /* 変数宣言部 */
    Define_AST, // 識別子の宣言のノード型
    Array_AST,  // 配列の宣言のノード型

    /* 変数部 */
    Ident_AST,      // 識別子のノード型
    Number_AST,     // 整数のノード型
    ArrayEl_AST,    // 配列のノード型
    ArrayIndex_AST, // 配列のキーのノード型

    /* 代入文 */
    Assign_AST, // 代入文のノード型

    /* ループ文 */
    While_AST, // whileループ文のノード型
    For_AST,   // forループ文のノード型

    /* 条件文 */
    If_AST, // 条件分岐文のノード型

    /* 算術演算子 */
    Add_AST, // 加算のノード型
    Sub_AST, // 減算のノード型
    Mul_AST, // 乗算のノード型
    Div_AST, // 除算のノード型
    Mod_AST, // 剰余のノード型

    /* 比較演算子 */
    Eq_AST,   // 同値のノード型
    LtoE_AST, // 小なりイコールのノード型
    GtoE_AST, // 大なりイコールのノード型
    Lt_AST,   // 小なりのノード型
    Gt_AST,   // 大なりのノード型

    /* 前置・後置演算子 */
    Pre_Increment_AST,  // 前置インクリメントのノード型
    Pre_Decrement_AST,  // 後置インクリメントのノード型
    Post_Increment_AST, // 前置デクリメントのノード型
    Post_Decrement_AST, // 後置デクリメントのノード型
} NodeType;

typedef struct abstract_node Node;
typedef struct abstract_node
{
    NodeType nType;
    char *varName; // IDENT の場合その変数名を入れる
    int value;     // NUMBERの値や配列の添え字の値を入れる
    Node *child;
    Node *brother;
} Node;

/* ================================================================== */

Node *top;

Node *build_node1(NodeType nType, Node *np1);
Node *build_node2(NodeType nType, Node *np1, Node *np2);
Node *build_node3(NodeType nType, Node *np1, Node *np2, Node *np3);
Node *build_ident_node(NodeType nType, char *varName);
Node *build_num_node(NodeType nType, int value);
Node *build_array_node(NodeType nType, char *varName, Node *np1);
Node *build_node4(NodeType nType, Node *np1, Node *np2, Node *np3, Node *np4);
