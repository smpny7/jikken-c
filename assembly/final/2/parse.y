%{
#include <stdio.h>
#include "main.tab.h"
 extern int yylex();
 extern int yyerror();
%}
%union{
 Node* np; // 抽象構文木
 int ival; // 数
 char* sp; // 変数名
}
%token DEFINE ARRAY WHILE IF ELSE SEMIC L_BRACKET R_BRACKET L_PARAN R_PARAN L_BRACE R_BRACE ASSIGN ADD SUB MUL DIV EQ LT GT
%token <sp> IDENT
%token <ival> NUMBER
// %type <np> expression term factor condition var
%type <np> declarations statements statement var
%%
/* <プログラム> ::= <変数宣言部> <文集合> */
program :
    declarations statements
    {
        top = build_node2(Pro_AST, $1, $2);
    }
;
/* <変数宣言部> ::= <宣言文> <変数宣言部> | <宣言文> */
declarations : decl_statement declarations | decl_statement
;
/* <宣言文> ::= define <識別子>; | array <識別子> [ <数> ]; */
decl_statement : DEFINE IDENT SEMIC | ARRAY IDENT L_BRACKET NUMBER R_BRACKET SEMIC
;
/* <文集合> ::= <文> <文集合>| <文> */
statements :
    statement statements
    {
        $$ = build_node2(Stats_AST, $1, $2);
    }
    | statement
    {
        $$ = build_node2(Stats_AST, $1, NULL);
    }
;
/* <文> ::= <代入文> | <ループ文> | <条件分岐文> */
statement : assignment_stmt | loop_stmt | cond_stmt
;
/* <代入文> ::= <識別子> = <算術式>; | <識別子> [ <数> ] = <算術式>; */
assignment_stmt : IDENT ASSIGN expression SEMIC | IDENT L_BRACKET NUMBER R_BRACKET ASSIGN expression SEMIC
;
/* <算術式> ::= <算術式> <加減演算子> <項> | <項> */
expression : expression add_op term | term
;
/* <項> ::= <項> <乗除演算子> <因子> | <因子> */
term : term mul_op factor | factor
;
/* <因子> ::= <変数> | (<算術式>) */
factor : var | L_PARAN expression R_PARAN
;
/* <加減演算子> ::= + | - */
add_op : ADD | SUB
;
/* <乗除演算子> ::= * | / */
mul_op : MUL | DIV
;
/* <変数> ::= <識別子> | <数> | <識別子> [ <数> ] */
var : IDENT
    {
        $$ = build_ident_node(Ident_AST, $1);
    }
    | NUMBER
    {
        $$ = build_num_node(Number_AST, $1);
    }
    | IDENT L_BRACKET NUMBER R_BRACKET
    {
        $$ = build_array_node(ArryEl_AST, $1, $3);
    }
;
/* <ループ文> ::= while (<条件式>) { <文集合> } */
loop_stmt : WHILE L_PARAN condition R_PARAN L_BRACE statements R_BRACE
;
/* <条件分岐文> ::= if (<条件式>) { <文集合> } | if (<条件式>) { <文集合> } else { <文集合> } */
cond_stmt : IF L_PARAN condition R_PARAN L_BRACE statements R_BRACE | IF L_PARAN condition R_PARAN L_BRACE statements R_BRACE ELSE L_BRACE statements R_BRACE
;
/* <条件式> ::= <算術式> <比較演算子> <算術式> */
condition : expression cond_op expression
;
/* <比較演算子> ::= == | '<' | '>' */
cond_op : EQ | LT | GT
;
%%
Node* top;

int main(void)
{
    if(yyparse()) {
        fprintf(stderr, "Error\n");
        return 1;
    }
    return 0;
}