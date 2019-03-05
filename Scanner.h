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
    int yytok;
    TOKEN_POSITION yytokpos;
    bool skip_non_sense_tokens;

public:
    Scanner(FILE* file, bool skip = false);
    Token* peek(void);
    Token* next(void);

private:
    void processID(void);
    void processINT_LIT(void);
    void processUNKNOWN(void);
    void warning(const char* message);
    void error(const char* message);
    void updateTokenPosition(int tok);
    void processCurrentTok(int tok);
};

#endif //COMPILERDESIGNPROJECT_SCANNER_H