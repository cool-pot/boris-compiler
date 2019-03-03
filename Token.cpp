//
// Created by 周易 on 2019-03-03.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "prettyprint.h"
#include "tokens.h"

#include "Token.h"

Token::Token(int tok, YYSTYPE yylval, TOKEN_POSITION yytokpos) {
    if(tok == ID){
        strncpy(this->yylval.sval, yylval.sval, MAX_ID_LENGTH);
    } else if (tok == INT_LIT){
        this->yylval.ival = yylval.ival;
    }
    this->tok = tok;
    this->yytokpos.char_finish = yytokpos.char_finish;
    this->yytokpos.char_start = yytokpos.char_start;
    this->yytokpos.line = yytokpos.line;
}

void Token::printTokenSummary() {
    printf("[Token %d] in Line %d, [%d,%d].", this->tok, this->yytokpos.line, this->yytokpos.char_start, this->yytokpos.char_finish);
    if(this->tok == ID){
        printf("With value %s\n", this->yylval.sval);
    } else if (this->tok == INT_LIT){
        printf("With value %u\n", this->yylval.ival);
    } else{
        printf("\n");
    }
}