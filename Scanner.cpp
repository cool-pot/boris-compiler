//
// Created by 周易 on 2019-03-02.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "prettyprint.h"
#include "tokens.h"

#include "Scanner.h"
#include "Token.h"

extern char *yytext;
extern int yylex(void);


Scanner:: Scanner(FILE* file, bool skip){
    input = file;
    yypretok = -1;
    yytok = -1;
    yytokpos.line = 1;
    yytokpos.char_start = 0;
    yytokpos.char_finish = 0;
    skip_non_sense_tokens = skip;
    next();
}

Token* Scanner::peek(){
    return new Token(yytok, yylval, yytokpos);
}


Token* Scanner::next(){
    if(yypretok == END_OF_FILE){
        fprintf(stderr, GREEN "---------------------------------------\n" RESET);
        fprintf(stderr, GREEN "Exit. Input finished. Meet END_OF_FILE.\n" RESET);
        exit(0);
    } else{
        // get the return value
        Token* ret_value = new Token(yytok, yylval, yytokpos);

        // process tok
        yytok = yylex();
        if (yytok == ID){
            processID();
        } else if (yytok == INT_LIT){
            processINT_LIT();
        }

        // reset pretok from tok
        yypretok = yytok;
        return ret_value;
    }
}

void Scanner::updateTokenPosition() {
    if (yypretok == COMMENT | yypretok == END_OF_LINE) {
        yytokpos.char_start = 0;
        yytokpos.char_finish = 0;
        yytokpos.line += 1;
    }
    int length = strlen(yytext);
    yytokpos.char_start = yytokpos.char_finish + 1;
    yytokpos.char_finish = yytokpos.char_start + length - 1;
}



void Scanner::processID(){
    int length = strlen(yytext);
    strncpy (yylval.sval, yytext, MAX_ID_LENGTH);
    if (length > MAX_ID_LENGTH) {
        warning("The length of ID exceeds max length, ID will be truncated.");
    }
}

void Scanner::processINT_LIT(){
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

void Scanner::warning(const char* message){
    fprintf(stderr, YELLOW);
    fprintf( stderr, "[Warning]%s in Line %d, [%d,%d]\n", message, yytokpos.line, yytokpos.char_start, yytokpos.char_finish);
    fprintf(stderr, RESET);
}




