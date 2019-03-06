//
// Created by 周易 on 2019-03-02.
//

#ifndef COMPILERDESIGNPROJECT_TOKENS_H
#define COMPILERDESIGNPROJECT_TOKENS_H

/* parameters */
#define MAX_ID_LENGTH 50
#define MAX_ABSOLUTE_INT_LIT 2147483648
#define MAX_INT_LIT_LENGTH 10

/* some structure definition*/
typedef struct TOKEN_POSITION{
    int line;
    int char_start;
    int char_finish;
} TOKEN_POSITION;

typedef union YYSTYPE{
    unsigned int ival;
    char sval[MAX_ID_LENGTH+1];
} YYSTYPE;

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
    OP_UMINUS = 294,/*THIS IS NEVER USED*/

    /* define the others*/
    INT_LIT = 295,
    ID = 296,
    COMMENT = 297,
    WS = 298,
    END_OF_LINE = 299,
    END_OF_FILE = 300,

    /* define the values in yylval, */
    INT_LIT_VALUE = 301,
    ID_VALUE = 302,

    /* Unknown character*/
    UNKNOWN = 303,

    /* UPDATES */
    KW_FOR = 304
};


#endif //COMPILERDESIGNPROJECT_TOKENS_H