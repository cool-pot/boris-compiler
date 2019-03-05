%{
    #include "tokens.h"
    #include "Scanner.h"
    #include "Token.h"
%}

%option noyywrap

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
\[ {  return LBRAK;}
\] {  return RBRAK;}
; {  return SEMI;}
\( {  return LPAR;}
\) {return RPAR;}
= { return ASSIGN;}
\<-\> {  return EXCHANGE;}
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
\*\*\*.* {  return COMMENT;}
\n {  return END_OF_LINE;}
[ \t] {  return WS;}
<<EOF>> {  return END_OF_FILE;}
[a-zA-Z_]+ { return ID;}
[0-9]+ { return INT_LIT;}
. {  return UNKNOWN;}



%%

int main(int argc, char **argv)
{
    // define the Scanner
    bool skip_nonsense_token = false;
    Scanner myScanner(stdin, skip_nonsense_token);

    // lex through the file
    while(myScanner.peek()->getTok() != END_OF_FILE){
        myScanner.next()->printTokenSummary();
    }
}