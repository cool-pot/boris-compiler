%{
#  include <stdio.h>
#  include <stdlib.h>
#  include "boris.h"
%}

%locations

%union {
  int ival;
  char* sval;
  struct Node* pnode;
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
%token OP_UMINUS
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
%type<pnode> input
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
%type<pnode> lhsitem
%type<pnode> expr*/
%% 


input:  END_OF_FILE
;

%%