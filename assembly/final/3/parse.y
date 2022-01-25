%{
#include <stdio.h>
#include "ast.h"
#include "parse.tab.h"
extern int yylex();
extern int yyerror();
%}
%union{
    Node* np;
    NodeType nType;
    int ival;
    char* sp;
}
%token DEFINE ARRAY WHILE FOR IF ELSE SEMIC L_BRACKET R_BRACKET L_PARAN R_PARAN L_BRACE R_BRACE ASSIGN ADD SUB MUL DIV MOD EQ LTOE GTOE LT GT INCREMENT DECREMENT
%token <sp> IDENT
%token <ival> NUMBER
%type <np> declarations statements decl_statement statement assignment_stmt expression term factor condition var while_stmt for_stmt cond_stmt array_index
%type <nType> add_op mul_op cond_op pre_increment post_increment
%%
/* <プログラム> ::= <変数宣言部> <文集合> */
program
    : declarations statements
    {
        top = build_node2(Pro_AST, $1, $2);
    }
;
/* <変数宣言部> ::= <宣言文> <変数宣言部> | <宣言文> */
declarations
    : decl_statement declarations
    {
        $$ = build_node2(Decls_AST, $1, $2);
    }
    | decl_statement
    {
        $$ = build_node1(Decls_AST, $1);
    }
;
/* <宣言文> ::= define <識別子>; | array <識別子> <配列番号>; */
decl_statement
    : DEFINE IDENT SEMIC
    {
        $$ = build_node1(Define_AST, build_ident_node(Ident_AST, $2));
    }
    | ARRAY IDENT array_index SEMIC {
        $$ = build_node1(Array_AST, build_array_node(ArrayEl_AST, $2, $3));
    }
;
/* <文集合> ::= <文> <文集合>| <文> */
statements
    : statement statements
    {
        $$ = build_node2(Stats_AST, $1, $2);
    }
    | statement
    {
        $$ = build_node1(Stats_AST, $1);
    }
;
/* <文> ::= <代入文> | <whileループ文> | <forループ文> | <条件分岐文> */
statement
    : assignment_stmt
    | while_stmt
    | for_stmt
    | cond_stmt
;
/* <代入文> ::= <識別子> = <算術式>; | <識別子> <配列番号> = <算術式>; */
assignment_stmt
    : IDENT ASSIGN expression SEMIC
    {
        $$ = build_node2(Assign_AST, build_ident_node(Ident_AST, $1), $3);
    }
    | IDENT array_index ASSIGN expression SEMIC
    {
        $$ = build_node2(Assign_AST, build_array_node(ArrayEl_AST, $1, $2), $4);
    }
;
/* <算術式> ::= <算術式> <加減演算子> <項> | <前置演算> <識別子> | <識別子> <後置演算> | <項> */
expression
    : expression add_op term
    {
        $$ = build_node2($2, $1, $3);
    }
    | pre_increment IDENT
    {
        $$ = build_node1($1, build_ident_node(Ident_AST, $2));
    }
    | IDENT post_increment
    {
        $$ = build_node1($2, build_ident_node(Ident_AST, $1));
    }
    | term
;
/* <前置演算> ::= '++' | '--' */
pre_increment
    : INCREMENT
    {
        $$ = Pre_Increment_AST;
    }
    | DECREMENT
    {
        $$ = Pre_Decrement_AST;
    }
;
/* <後置演算> ::= '++' | '--' */
post_increment
    : INCREMENT
    {
        $$ = Post_Increment_AST;
    }
    | DECREMENT
    {
        $$ = Post_Decrement_AST;
    }
;
/* <項> ::= <項> <乗除演算子> <因子> | <因子> */
term
    : term mul_op factor
    {
        $$ = build_node2($2, $1, $3);
    }
    | factor
;
/* <因子> ::= <変数> | ( <算術式> ) */
factor
    : var
    | L_PARAN expression R_PARAN
    {
        $$ = $2;
    }
;
/* <加減演算子> ::= '+' | '-' */
add_op
    : ADD
    {
        $$ = Add_AST;
    }
    | SUB
    {
        $$ = Sub_AST;
    }
;
/* <乗除演算子> ::= '*' | '/' | '%' */
mul_op
    : MUL
    {
        $$ = Mul_AST;
    }
    | DIV
    {
        $$ = Div_AST;
    }
    | MOD
    {
        $$ = Mod_AST;
    }
;
/* <変数> ::= <識別子> | <数> | <識別子> <配列番号> */
var
    : IDENT
    {
        $$ = build_ident_node(Ident_AST, $1);
    }
    | NUMBER
    {
        $$ = build_num_node(Number_AST, $1);
    }
    | IDENT array_index
    {
        $$ = build_array_node(ArrayEl_AST, $1, $2);
    }
;
/* <配列番号> ::= [ <算術子> ] <配列番号> | [ <算術子> ] */
array_index
    : L_BRACKET expression R_BRACKET array_index
    {
        $$ = build_node2(ArrayIndex_AST, $2, $4);
    }
    | L_BRACKET expression R_BRACKET
    {
        $$ = build_node1(ArrayIndex_AST, $2);
    }
;
/* <whileループ文> ::= while ( <条件式> ) { <文集合> } */
while_stmt
    : WHILE L_PARAN condition R_PARAN L_BRACE statements R_BRACE
    {
        $$ = build_node2(While_AST, $3, $6);
    }
;
/* <forループ文> ::= for ( <文集合> <条件式>; <算術式> ) { <文集合> } */
for_stmt
    : FOR L_PARAN statement condition SEMIC expression R_PARAN L_BRACE statements R_BRACE
    {
        $$ = build_node4(For_AST, $3, $4, $6, $9);
    }
;
/* <条件分岐文> ::= if ( <条件式> ) { <文集合> } | if (<条件式>) { <文集合> } else { <文集合> } | if (<条件式>) { <文集合> } else <条件分岐文> */
cond_stmt
    : IF L_PARAN condition R_PARAN L_BRACE statements R_BRACE
    {
        $$ = build_node2(If_AST, $3, $6);
    }
    | IF L_PARAN condition R_PARAN L_BRACE statements R_BRACE ELSE L_BRACE statements R_BRACE
    {
        $$ = build_node3(If_AST, $3, $6, $10);
    }
    | IF L_PARAN condition R_PARAN L_BRACE statements R_BRACE ELSE cond_stmt
    {
        $$ = build_node3(If_AST, $3, $6, build_node1(Stats_AST, $9));
    }
;
/* <条件式> ::= <算術式> <比較演算子> <算術式> */
condition
    : expression cond_op expression
    {
        $$ = build_node2($2, $1, $3);
    }
;
/* <比較演算子> ::= '==' | '<=' | '>=' | '<' | '>' */
cond_op
    : EQ
    {
        $$ = Eq_AST;
    }
    | LTOE
    {
        $$ = LtoE_AST;
    }
    | GTOE
    {
        $$ = GtoE_AST;
    }
    | LT
    {
        $$ = Lt_AST;
    }
    | GT
    {
        $$ = Gt_AST;
    }
;
%%
