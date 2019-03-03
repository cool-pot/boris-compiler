//
// Created by 周易 on 2019-03-02.
//

#ifndef COMPILERDESIGNPROJECT_SCANNER_H
#define COMPILERDESIGNPROJECT_SCANNER_H

#include "Token.h"

class Scanner {
public:
    FILE* input;
    YYSTYPE yylval;
    int yypretok;
    int yytok;
    TOKEN_POSITION yytokpos;
    bool skip_non_sense_tokens;

public:
    Scanner(FILE* file, bool skip = false);
    Token* peek(void);
    Token* next(void);
    void updateTokenPosition(void);

private:
    void processID(void);
    void processINT_LIT(void);
    void warning(const char* message);
};

#endif //COMPILERDESIGNPROJECT_SCANNER_H