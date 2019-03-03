//
// Created by 周易 on 2019-03-03.
//

#ifndef COMPILERDESIGNPROJECT_TOKEN_H
#define COMPILERDESIGNPROJECT_TOKEN_H

#include "tokens.h"

class Token {
private:
    int tok;
    YYSTYPE yylval;
    TOKEN_POSITION yytokpos;

public:
    Token(int tok, YYSTYPE yylval, TOKEN_POSITION yytokpos);
    void printTokenSummary(void);
};


#endif //COMPILERDESIGNPROJECT_TOKEN_H
