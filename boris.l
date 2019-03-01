%{
    /* parameters */
    #define MAX_ID_LENGTH 5
    #define MAX_ABSOLUTE_INT_LIT 2147483648
    #define MAX_INT_LIT_LENGTH 10

    /* tokens */
    enum yytokentype {
        /* define key words, constant strings*/
        KW_ARRAY = 258,
        KW_TUPLE = 259,
        KW_LOCAL = 260,
        KW_GLOBAL = 261,
        KW_DEFUN = 262,
        KW_END = 263,
        KW_WHILE = 264,
        KW_DO = 265,
        KW_IF = 266,
        KW_THEN = 267,
        KW_ELSIF = 268,
        KW_ELSE = 269,
        KW_FOREACH = 270,
        KW_IN = 271,
        RETURN = 272,
        PRINT = 273,

        /* define operators*/
        OP_DOTDOT = 274,
        OP_COMMA = 275,
        OP_DOT = 276,
        LBRAK = 277,
        RBRAK = 278,
        SEMI = 279,
        LPAR = 280,
        RPAR = 281,
        ASSIGN = 282,
        EXCHANGE = 283,
        OP_LESS = 284,
        OP_GREATER = 285,
        OP_LESSEQUAL = 286,
        OP_GREATEREQUAL = 287,
        OP_EQUAL = 288,
        OP_NOTEQUA = 289,
        OP_PLUS = 290,
        OP_MINUS = 291,
        OP_MULT = 292,
        OP_DIV = 293,
        OP_UMINUS = 294,

        /* define the others*/
        INT_LIT = 295,
        ID = 296,
        COMMENT = 297,
        WS = 298,
        END_OF_LINE = 299,
        END_OF_FILE = 300,

        /* define the values in yylval, */
        INT_LIT_VALUE = 301,
        ID_VALUE = 302

    };

    typedef struct TOKEN_POSITION{
        int line;
        int char_start;
        int char_finish;
    } TOKEN_POSITION;

    typedef union YYSTYPE{
      unsigned int ival;
      char *sval;
    } YYSTYPE;

    /* global variables*/
    YYSTYPE yylval;
    TOKEN_POSITION yytokpos = {1, 0, 0};
    int yypretok = -1;

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
. { updateTokenPosition(); warning("Unexpected characater");}





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
    fprintf(stderr, "\033[1;33m");
    fprintf( stderr, "[Warning]%s\n", message);
    yytokposition();
    fprintf(stderr, "\033[0m");
}

void processID(){
    int length = strlen(yytext);
    char id_str[MAX_ID_LENGTH];
    strncpy ( id_str, yytext, MAX_ID_LENGTH);
    yylval.sval = id_str;
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
    fprintf( stderr, "[Position] Line %d [%d, %d] --> %s\n", yytokpos.line, yytokpos.char_start, yytokpos.char_finish, yytext);
}



int main(int argc, char **argv)
{
    int tok;
    while ((tok = yylex())){
        printf("TOKEN NUMBER %d, Line %d [%d, %d]\n", tok, yytokpos.line, yytokpos.char_start, yytokpos.char_finish);
        yypretok = tok;
        if (tok == ID){
            processID();
        } else if (tok == INT_LIT){
            processINT_LIT();
        } else if (tok == END_OF_FILE){
            break;
        }
    }
}