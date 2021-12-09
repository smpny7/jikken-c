%{
#include "sample.tab.h"
/* lex for sample.y */
%}
%%
I {return SUBJECT;}
am {return PRED;}
[\t ]+
\n return 0;
. return yytext[0];
%%