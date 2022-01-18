
/*
* ===================================================
*  Print Tree GUI  ver.1.0.0  (January 18th 2022)
* ===================================================
*
*  For more info.
*  https://github.com/smpny7/print-tree-gui
*
*/

#include <stdio.h>

char *node_type_str[] = {
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
	"Post_Decrement_AST"
};

/*
* Overview: Output the tree structure.
* @argument: {Node *} np - Parent node.
* @return: No return
*/
void printTreeGUI(Node *np)
{
    printf("{");

    printf("\"type\": \"%s\",", node_type_str[np->nType]);

    printf("\"varName\": \"%s\",", np->varName ? np->varName : "null");

    printf("\"value\": ");
    np->nType == Number_AST ? printf("\"%d\",", np->value) : printf("\"null\",");

    printf("\"child\": [");
    if (np->child != NULL)
        printTreeGUI((Node *)np->child);
    printf("],");

    printf("\"brother\": [");
    if (np->brother != NULL)
        printTreeGUI((Node *)np->brother);
    printf("]");

    printf("}");
}
