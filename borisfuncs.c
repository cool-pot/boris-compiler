#  include <stdio.h>
#  include <stdlib.h>
#  include <stdarg.h>
#  include "boris.h"
#  include "boris.tab.h" // yylloc definition, token number


void yyerror(char *s, ...)
{
    va_list ap;
    va_start(ap, s);
    fprintf(stderr, RED);
    vfprintf(stderr, s, ap);
    fprintf(stderr, " In position %d,%d-%d,%d.",yylloc.first_line,yylloc.first_column, yylloc.last_line, yylloc.last_column);
    fprintf(stderr, RESET);
    fprintf(stderr, "\n");
}

void lexerTester()
{ 
    printf("> Hello from lexerTester for boris compiler!\n");
    int tok;
    do {
        tok = yylex();
        printf("token[%d].", tok);
        if (tok == END_OF_LINE) {
                printf("[\\n]");\
            } else{ 
                printf("[%s]", yytext); 
        }
        printf(" In position %d,%d-%d,%d.",yylloc.first_line,yylloc.first_column, yylloc.last_line, yylloc.last_column);
        if (tok == ID) {
            printf(GREEN);
            printf("[ID] Retained infomation: yylval.sval=%s",yylval.sval);
            printf(RESET);
        } else if (tok == INT_LIT){
            printf(GREEN);
            printf("[INT_LIT] Retained infomation: yylval.ival=%d",yylval.ival);
            printf(RESET);
        }
        
        printf("\n");
    } while (tok != END_OF_FILE);
}

int main(){
    lexerTester();
    return 0;
}