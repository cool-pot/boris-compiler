%{
    /* [ ---------------------WARNING----------------*/
    /* THIS SCRIPT IS OUTDATED,ONLY FOR TEST PURPOSES*/
    /* [ ---------------------WARNING----------------*/

    #include "tokens.h"
    #include "prettyprint.h"

    /* global variables*/
    YYSTYPE yylval;
    YYSTYPE yyprelval;
    int yypretok = -1;
    int yytok = -1;
    TOKEN_POSITION yytokpos = {1, 0, 0};
    TOKEN_POSITION yypretokpos = {1, 0, 0};


    /* helpful routines to keep track of the scanning process*/
    void updateTokenPosition();
    void yytokposition();
    void warning(const char*);

%}

%option noyywrap





%%
array { updateTokenPosition(); return KW_ARRAY;}
tuple { updateTokenPosition(); return KW_TUPLE;}
local { updateTokenPosition(); return KW_LOCAL;}
global { updateTokenPosition(); return KW_GLOBAL;}
defun { updateTokenPosition(); return KW_DEFUN;}
end { updateTokenPosition(); return KW_END;}
while { updateTokenPosition(); return KW_WHILE;}
do { updateTokenPosition(); return KW_DO;}
if { updateTokenPosition(); return KW_IF;}
then { updateTokenPosition(); return KW_THEN;}
elsif { updateTokenPosition(); return KW_ELSIF;}
else { updateTokenPosition(); return KW_ELSE;}
foreach { updateTokenPosition(); return KW_FOREACH;}
in { updateTokenPosition(); return KW_IN;}
return { updateTokenPosition(); return RETURN;}
print { updateTokenPosition(); return PRINT;}
\.\. { updateTokenPosition(); return OP_DOTDOT;}
, { updateTokenPosition(); return OP_COMMA;}
\. { updateTokenPosition(); return OP_DOT;}
\[ { updateTokenPosition(); return LBRAK;}
\] { updateTokenPosition(); return RBRAK;}
; { updateTokenPosition(); return SEMI;}
\( { updateTokenPosition(); return LPAR;}
\) { updateTokenPosition(); return RPAR;}
= { updateTokenPosition(); return ASSIGN;}
\<-\> { updateTokenPosition(); return EXCHANGE;}
\< { updateTokenPosition(); return OP_LESS;}
\> { updateTokenPosition(); return OP_GREATER;}
\<= { updateTokenPosition(); return OP_LESSEQUAL;}
\>= { updateTokenPosition(); return OP_GREATEREQUAL;}
== { updateTokenPosition(); return OP_EQUAL;}
!= { updateTokenPosition(); return OP_NOTEQUA;}
\+ { updateTokenPosition(); return OP_PLUS;}
- { updateTokenPosition(); return OP_MINUS;}
\* { updateTokenPosition(); return OP_MULT;}
\/ { updateTokenPosition(); return OP_DIV;}
- { updateTokenPosition(); return OP_UMINUS;}
\*\*\*.*\n { updateTokenPosition(); return COMMENT;}
\n { updateTokenPosition(); return END_OF_LINE;}
[ \t] { updateTokenPosition(); return WS;}
<<EOF>> { updateTokenPosition(); return END_OF_FILE;}
[a-zA-Z_]+ {updateTokenPosition(); return ID;}
[0-9]+ {updateTokenPosition(); return INT_LIT;}
. { updateTokenPosition(); fprintf(stderr, RED "Unexpected characater found: %s\n" RESET, yytext);}



%%
void updateTokenPosition(){
    if (yypretok == COMMENT | yypretok == END_OF_LINE) {
        yytokpos.char_start = 0;
        yytokpos.char_finish = 0;
        yytokpos.line += 1;
    }
    int length = strlen(yytext);
    yytokpos.char_start = yytokpos.char_finish + 1;
    yytokpos.char_finish = yytokpos.char_start + length - 1;
}

void warning(const char* message){
    fprintf(stderr, YELLOW);
    fprintf( stderr, "[Warning]%s. ", message);
    yytokposition();
    fprintf(stderr, RESET);
}

void processID(){
    int length = strlen(yytext);
    strncpy (yylval.sval, yytext, MAX_ID_LENGTH);
    if (length > MAX_ID_LENGTH) {
        warning("The length of ID exceeds max length, ID will be truncated.");
    }
}

void processINT_LIT(){
    int length = strlen(yytext);
    long int val = atol(yytext);
    if (length > MAX_INT_LIT_LENGTH) {
        warning("The length of INT_LIT exceeds max length, No guarantee what will happen.");
    } else if (length > 1 && yytext[0] == '0') {
        warning("INT_LIT in a invalid format. Non-zero integer can't start with 0. No guarantee what will happen.");
    } else if (val > MAX_ABSOLUTE_INT_LIT){
       warning("INT_LIT exceeds max value permitted. No guarantee what will happen.");
    }
    unsigned int ival = (unsigned int) val;
    yylval.ival = ival;
}

void yytokposition(){
    fprintf( stderr, "In position Line %d [%d, %d] --> %s\n", yytokpos.line, yytokpos.char_start, yytokpos.char_finish, yytext);
}



int main(int argc, char **argv)
{
    int tok;
    while ((tok = yylex())){
        printf("[TOKEN NUMBER] %d, Line %d [%d, %d]\n", tok, yytokpos.line, yytokpos.char_start, yytokpos.char_finish);
        if (tok == ID){
            processID();
            printf(GREEN "Get a token ID with value %s\n" RESET, yylval.sval);
        } else if (tok == INT_LIT){
            processINT_LIT();
            printf(GREEN "Get a token INT_LIT with value %u\n" RESET, yylval.ival);
        } else if (tok == END_OF_FILE){
            break;
        }
        yypretok = tok;
    }
}