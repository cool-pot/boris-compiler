%{
#  include <stdio.h>
#  include <stdlib.h>
#  include "boris.h"
%}

%locations

%union {
  int ival;
  char* sval;
  struct pNode* pnode;
}

/* define key words, constant strings*/
%token KW_ARRAY
%token KW_TUPLE
%token KW_LOCAL
%token KW_GLOBAL
%token KW_DEFUN
%token KW_END
%token KW_WHILE
%token KW_DO
%token KW_IF
%token KW_THEN
%token KW_ELSIF
%token KW_ELSE
%token KW_FOREACH
%token KW_IN
%token RETURN
%token PRINT

/* define operators*/
%token OP_DOTDOT
%token OP_COMMA
%token OP_DOT
%token OP_LESS
%token OP_GREATER
%token OP_LESSEQUAL
%token OP_GREATEREQUAL
%token OP_EQUAL
%token OP_NOTEQUA
%token OP_PLUS
%token OP_MINUS
%token OP_MULT
%token OP_DIV
%token OP_LBRAK
%token OP_RBRAK
%token OP_SEMI
%token OP_LPAR
%token OP_RPAR
%token OP_ASSIGN
%token OP_EXCHANGE

/* define other*/
%token <ival>INT_LIT
%token <sval>ID


/* define nonsense tokens*/
%token COMMENT
%token WS
%token END_OF_LINE
%token END_OF_FILE
%token UNKNOWN

/* define the values to be used in non-terminals*/

/*
%type<pnode> sdd_list
%type<pnode> sdd
%type<pnode> comma_expr_list
%type<pnode> decl
%type<pnode> comma_id_list
%type<pnode> defn
%type<pnode> sd_list
%type<pnode> sd
%type<pnode> body
%type<pnode> statement_list
%type<pnode> elsif_sentence
%type<pnode> elsif_sentence_list
%type<pnode> else_sentence
%type<pnode> statement
%type<pnode> array_id
%type<pnode> range
%type<pnode> bool_expr
%type<pnode> bool_op
%type<pnode> comma_lhsitem_list
%type<pnode> lhs
%type<pnode> lhsitem*/
%type<pnode> expr

%start input

%% 

%left OP_COMMA;
%left OP_MINUS OP_PLUS;
%left OP_DIV OP_MULT;

input:  expr { printf("\n> Start visualization:\n"); visualize($1, 0);}
;

expr:  expr OP_COMMA expr { $$ = newpNode(NODETYPE_EXPR_COMMA_EXPR, 3, $1, newplaceholderNode(OP_COMMA), $3);}
    | expr OP_MINUS expr { $$ = newpNode(NODETYPE_EXPR_MINUS_EXPR, 3, $1, newplaceholderNode(OP_MINUS), $3);}
    | expr OP_PLUS expr { $$ = newpNode(NODETYPE_EXPR_PLUS_EXPR, 3, $1, newplaceholderNode(OP_PLUS), $3);}
    | expr OP_DIV expr { $$ = newpNode(NODETYPE_EXPR_DIV_EXPR, 3, $1, newplaceholderNode(OP_DIV), $3);}
    | expr OP_MULT expr { $$ = newpNode(NODETYPE_EXPR_MULT_EXPR, 3, $1, newplaceholderNode(OP_MULT), $3);}
    | OP_LPAR expr OP_RPAR { $$ = newpNode(NODETYPE_LPAR_EXPR_RPAR, 3, newplaceholderNode(OP_LPAR), $2, newplaceholderNode(OP_RPAR));}
    | ID { $$ = newpNode(NODETYPE_SINGLE_ID_AS_EXPR, 1, newsNode($1));}
;

%%