#define MAXBUF 80

typedef enum
{
    Pro_AST,    //プログラムのノード型
    Stats_AST,  //文集合のノード型
    Ident_AST,  // 識別子のノード型
    Number_AST, // 整数のノード型
    ArryEl_AST, // 配列のノード型

    Add_AST,    // 和のノード型
    Assign_AST, // 代入文の=のノード型
} NodeType;

typedef struct abstract_node
{
    NodeType nType;
    char *varName; // IDENT の場合その変数名を入れる
    int value;     // NUMBERの値や配列の添え字の値を入れる
    struct Node *child;
    struct Node *brother;
} Node;

Node *build_node2(NodeType nType, Node *np1, Node *np2);
Node *build_ident_node(NodeType nType, char *varName);
Node *build_num_node(NodeType nType, int value);
Node *build_array_node(NodeType nType, char *varName, int value);

/* ==  for debug  ================== */

char *node_type_str[] = {
    "Pro_AST",    //プログラムのノード型
    "Stats_AST",  //文集合のノード型
    "Ident_AST",  // 識別子のノード型
    "Number_AST", // 整数のノード型
    "ArryEl_AST", // 配列のノード型

    "Add_AST",    // 和のノード型
    "Assign_AST", // 代入文の=のノード型
};

/* ================================= */
