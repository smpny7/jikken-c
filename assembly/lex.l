%{
#include "ast.h"
#include "parse.tab.h"
%}
%option noinput
%option nounput
%%
"define"                return DEFINE;
"array"                 return ARRAY;
"while"                 return WHILE;
"for"                   return FOR;
"if"                    return IF;
"else"                  return ELSE;
";"                     return SEMIC;
"["                     return L_BRACKET;
"]"                     return R_BRACKET;
"("                     return L_PARAN;
")"                     return R_PARAN;
"{"                     return L_BRACE;
"}"                     return R_BRACE;
"="                     return ASSIGN;
"+"                     return ADD;
"-"                     return SUB;
"*"                     return MUL;
"/"                     return DIV;
"%"                     return MOD;
"=="                    return EQ;
"<="                    return LTOE;
">="                    return GTOE;
"<"                     return LT;
">"                     return GT;
"++"                    return INCREMENT;
"--"                    return DECREMENT;
-?[1-9][0-9]*|0         {yylval.ival = atoi(yytext); return NUMBER;}
[a-zA-Z][a-zA-Z0-9]*    {yylval.sp = (char*)malloc(sizeof(char) * yyleng); strncpy(yylval.sp, yytext, yyleng); return IDENT;}
[\t ]                   ; /* ignore whitespace */
\n                      ; /* ignore whitespace */
%%
