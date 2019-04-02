/* recognize the tokens for the boris compiler*/

%{
# include "boris.h"
# include "boris.tab.h"

int yycolumn = 1;

#define YY_USER_ACTION yylloc.first_line = yylloc.last_line = yylineno; \
    if (yytext[0] == '\n') {\
        yycolumn = 0;\
    } \
    yylloc.first_column = yycolumn; \
    yylloc.last_column = yycolumn + yyleng - 1; \
    yycolumn += yyleng; 
%}

%option noyywrap nodefault yylineno

%%
array { return KW_ARRAY;}
tuple { return KW_TUPLE;}
local { return KW_LOCAL;}
global { return KW_GLOBAL;}
defun {  return KW_DEFUN;}
end {  return KW_END;}
while {  return KW_WHILE;}
do {  return KW_DO;}
if {  return KW_IF;}
then {  return KW_THEN;}
elsif {  return KW_ELSIF;}
else {  return KW_ELSE;}
foreach { return KW_FOREACH;}
in {  return KW_IN;}
return { return RETURN;}
print {  return PRINT;}
\.\. {  return OP_DOTDOT;}
, {  return OP_COMMA;}
\. {  return OP_DOT;}
\[ {  return OP_LBRAK;}
\] {  return OP_RBRAK;}
; {  return OP_SEMI;}
\( {  return OP_LPAR;}
\) {return OP_RPAR;}
= { return OP_ASSIGN;}
\<-\> {  return OP_EXCHANGE;}
\< { return OP_LESS;}
\> {  return OP_GREATER;}
\<= {  return OP_LESSEQUAL;}
\>= {return OP_GREATEREQUAL;}
== {  return OP_EQUAL;}
!= { return OP_NOTEQUA;}
\+ {  return OP_PLUS;}
- { return OP_MINUS;}
\* { return OP_MULT;}
\/ { return OP_DIV;}
[a-zA-Z_]+ { 
    if(yyleng > MAX_ID_LENGTH){ 
        yyerror("[Lexer Error]ID length is too long. Will be truncted."); 
    } 
    yylval.sval = strndup(yytext, MAX_ID_LENGTH); //TODO, NEED FREE IN THE FUTURE
    return ID;}
[0-9]+ { 
    long int val = atol(yytext);
    if (yyleng > MAX_INT_LIT_LENGTH) {
        yyerror("[Lexer Error]The length of INT_LIT exceeds max length, No guarantee what will happen.");
    } else if (yyleng > 1 && yytext[0] == '0') {
        yyerror("[Lexer Error]INT_LIT in a invalid format. Non-zero integer can't start with 0. No guarantee what will happen.");
    } else if (val > MAX_INT_LIT){
        yyerror("[Lexer Error]INT_LIT exceeds max value permitted. No guarantee what will happen.");
    }
    int ival = (int) val;
    yylval.ival = ival;
    return INT_LIT;}
\*\*\*.* { /* return COMMENT; */}
\n {  /* return END_OF_LINE; */}
[ \t] {  /* return WS; */}
. { yyerror("[Lexer Error]Mystery character %c", *yytext); /* return UNKNOWN; */} 

%%