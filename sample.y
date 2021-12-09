%{
#include <stdio.h>
#include "sample.tab.h"
extern int yylex();
extern int yyerror();
%}
%token SUBJECT PRED
%%
statement
 : SUBJECT PRED { printf("OK!\n");}
 ;
%%
int main(void){
 if (yyparse()) {
 fprintf(stderr, "Error ! Error ! Error !\n");
 return 1;
 }
}