%{
    #include "tokens.h"
    #include "prettyprint.h"
    #include "Scanner.h"
    #include "Token.h"
    Scanner myScanner(stdin);
%}

%option noyywrap

%%
array { myScanner.updateTokenPosition(); return KW_ARRAY;}
tuple { myScanner.updateTokenPosition(); return KW_TUPLE;}
local { myScanner.updateTokenPosition(); return KW_LOCAL;}
global { myScanner.updateTokenPosition(); return KW_GLOBAL;}
defun { myScanner.updateTokenPosition(); return KW_DEFUN;}
end { myScanner.updateTokenPosition(); return KW_END;}
while { myScanner.updateTokenPosition(); return KW_WHILE;}
do { myScanner.updateTokenPosition(); return KW_DO;}
if { myScanner.updateTokenPosition(); return KW_IF;}
then { myScanner.updateTokenPosition(); return KW_THEN;}
elsif { myScanner.updateTokenPosition(); return KW_ELSIF;}
else { myScanner.updateTokenPosition(); return KW_ELSE;}
foreach { myScanner.updateTokenPosition(); return KW_FOREACH;}
in { myScanner.updateTokenPosition(); return KW_IN;}
return { myScanner.updateTokenPosition(); return RETURN;}
print { myScanner.updateTokenPosition(); return PRINT;}
\.\. { myScanner.updateTokenPosition(); return OP_DOTDOT;}
, { myScanner.updateTokenPosition(); return OP_COMMA;}
\. { myScanner.updateTokenPosition(); return OP_DOT;}
\[ { myScanner.updateTokenPosition(); return LBRAK;}
\] { myScanner.updateTokenPosition(); return RBRAK;}
; { myScanner.updateTokenPosition(); return SEMI;}
\( { myScanner.updateTokenPosition(); return LPAR;}
\) { myScanner.updateTokenPosition(); return RPAR;}
= { myScanner.updateTokenPosition(); return ASSIGN;}
\<-\> { myScanner.updateTokenPosition(); return EXCHANGE;}
\< { myScanner.updateTokenPosition(); return OP_LESS;}
\> { myScanner.updateTokenPosition(); return OP_GREATER;}
\<= { myScanner.updateTokenPosition(); return OP_LESSEQUAL;}
\>= { myScanner.updateTokenPosition(); return OP_GREATEREQUAL;}
== { myScanner.updateTokenPosition(); return OP_EQUAL;}
!= { myScanner.updateTokenPosition(); return OP_NOTEQUA;}
\+ { myScanner.updateTokenPosition(); return OP_PLUS;}
- { myScanner.updateTokenPosition(); return OP_MINUS;}
\* { myScanner.updateTokenPosition(); return OP_MULT;}
\/ { myScanner.updateTokenPosition(); return OP_DIV;}
- { myScanner.updateTokenPosition(); return OP_UMINUS;}
\*\*\*.*\n { myScanner.updateTokenPosition(); return COMMENT;}
\n { myScanner.updateTokenPosition(); return END_OF_LINE;}
[ \t] { myScanner.updateTokenPosition(); return WS;}
<<EOF>> { myScanner.updateTokenPosition(); return END_OF_FILE;}
[a-zA-Z_]+ {myScanner.updateTokenPosition(); return ID;}
[0-9]+ {myScanner.updateTokenPosition(); return INT_LIT;}
. { myScanner.updateTokenPosition(); fprintf(stderr, RED "Unexpected characater found: %s\n" RESET, yytext);}



%%

int main(int argc, char **argv)
{
    while(true){
        Token* p = myScanner.peek();
        p->printTokenSummary();
        delete p;
        Token* n = myScanner.next();
        n->printTokenSummary();
        delete n;
    }
}