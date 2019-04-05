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
%type<pnode> array_id*/
%type<pnode> elsif_sentence_list
%type<pnode> else_sentence
%type<pnode> elsif_sentence
%type<pnode> statement_list
%type<pnode> statement
%type<pnode> range
%type<pnode> bool_expr
%type<pnode> bool_op
%type<pnode> lhs
%type<pnode> comma_lhsitem_list
%type<pnode> lhsitem
%type<pnode> expr
%start input

%% 

%left OP_COMMA;
%left OP_MINUS;
%left OP_PLUS;
%left OP_DIV;
%left OP_MULT;

%nonassoc EXPR_LPAR_RPAR_INCLUSICE;
%nonassoc EXPR_NORMAL_ID;
%nonassoc EXPR_FUNCTION_ID;
%nonassoc EXPR_TUPLE_ID;
%nonassoc EXPR_ARRAY_ID;

input: statement{ printf("\n> Start visualization\n"); visualize($1, 0);}
;

elsif_sentence: KW_ELSIF bool_expr KW_THEN statement_list { $$ = newpNode(NODETYPE_ELSIF_SENTENCE, 4, newplaceholderNode(KW_ELSIF), $2, newplaceholderNode(KW_THEN), $4);}
;

else_sentence: KW_ELSE statement_list { $$ = newpNode(NODETYPE_ELSE_SENTENCE, 2, newplaceholderNode(KW_ELSE), $2);}
;

elsif_sentence_list: /* empty */ { $$ = NULL; } 
        | elsif_sentence elsif_sentence_list{ $$ = newpNode(NODETYPE_ELSIF_SENTENCE_LIST, 2, $1, $2); }
;

statement_list: /* empty */ { $$ = NULL; } 
        | statement statement_list{ $$ = newpNode(NODETYPE_STATEMENT_LIST, 2, $1, $2); }

statement: lhs OP_ASSIGN expr OP_SEMI { $$ = newpNode(NODETYPE_LHS_ASSIGN_EXPR_AS_STATEMENT, 4, $1, newplaceholderNode(OP_ASSIGN), $3, newplaceholderNode(OP_SEMI));}
        | lhs OP_EXCHANGE lhs OP_SEMI { $$ = newpNode(NODETYPE_LHS_EXCHANGE_LHS_AS_STATEMENT, 4, $1, newplaceholderNode(OP_EXCHANGE), $3, newplaceholderNode(OP_SEMI));}
        | KW_WHILE bool_expr KW_DO statement_list KW_END KW_WHILE { $$ = newpNode(NODETYPE_WHILE_STATEMENT, 6, newplaceholderNode(KW_WHILE), $2, newplaceholderNode(KW_DO), $4, newplaceholderNode(KW_END), newplaceholderNode(KW_WHILE));}
        | KW_IF bool_expr KW_THEN statement_list elsif_sentence_list KW_END KW_IF { $$ = newpNode(NODETYPE_IF_STATEMENT, 7, newplaceholderNode(KW_IF), $2, newplaceholderNode(KW_THEN), $4, $5, newplaceholderNode(KW_END), newplaceholderNode(KW_IF));}
        | KW_IF bool_expr KW_THEN statement_list elsif_sentence_list else_sentence KW_END KW_IF { $$ = newpNode(NODETYPE_IF_ELSE_STATEMENT, 8, newplaceholderNode(KW_IF), $2, newplaceholderNode(KW_THEN), $4, $5, $6, newplaceholderNode(KW_END), newplaceholderNode(KW_IF));}
;

range: expr OP_DOTDOT expr { $$ = newpNode(NODETYPE_RANGE, 3, $1, newplaceholderNode(OP_DOTDOT), $3); }
;

bool_expr: expr bool_op expr { $$ = newpNode(NODETYPE_BOOLEXPR, 3, $1, $2, $3); }
;

bool_op: OP_LESS { $$ = newplaceholderNode(OP_LESS); }
        | OP_GREATER { $$ = newplaceholderNode(OP_GREATER); }
        | OP_EQUAL { $$ = newplaceholderNode(OP_EQUAL); }
        | OP_NOTEQUA { $$ = newplaceholderNode(OP_NOTEQUA); }
        | OP_LESSEQUAL { $$ = newplaceholderNode(OP_LESSEQUAL); }
        | OP_GREATEREQUAL { $$ = newplaceholderNode(OP_GREATEREQUAL); }
;

lhs: lhsitem comma_lhsitem_list { $$ = newpNode(NODETYPE_LHS, 2, $1, $2); }
;

comma_lhsitem_list: /* empty */ { $$ = NULL; } 
    | OP_COMMA lhsitem comma_lhsitem_list{ $$ = newpNode(NODETYPE_COMMA_LHSITEN_LIST, 3, newplaceholderNode(OP_COMMA), $2, $3); } 
;

lhsitem:  ID { $$ = newpNode(NODETYPE_SINGLE_ID_AS_LHSITEM, 1, newsNode($1));} 
    | ID OP_DOT INT_LIT  { $$ = newpNode(NODETYPE_TUPLE_REF_AS_LHSITEM, 3, newsNode($1), newplaceholderNode(OP_DOT), newiNode($3));}
    | ID OP_LBRAK expr OP_RBRAK { $$ = newpNode(NODETYPE_ARRAY_REF_AS_LHSITEM, 4, newsNode($1), newplaceholderNode(OP_LBRAK), $3, newplaceholderNode(OP_RBRAK));}
;

expr: expr OP_COMMA expr { $$ = newpNode(NODETYPE_EXPR_COMMA_EXPR, 3, $1, newplaceholderNode(OP_COMMA), $3);}
    | expr OP_MINUS expr { $$ = newpNode(NODETYPE_EXPR_MINUS_EXPR, 3, $1, newplaceholderNode(OP_MINUS), $3);}
    | expr OP_PLUS expr { $$ = newpNode(NODETYPE_EXPR_PLUS_EXPR, 3, $1, newplaceholderNode(OP_PLUS), $3);}
    | expr OP_DIV expr { $$ = newpNode(NODETYPE_EXPR_DIV_EXPR, 3, $1, newplaceholderNode(OP_DIV), $3);}
    | expr OP_MULT expr { $$ = newpNode(NODETYPE_EXPR_MULT_EXPR, 3, $1, newplaceholderNode(OP_MULT), $3);}
    | OP_LPAR expr OP_RPAR { $$ = newpNode(NODETYPE_LPAR_EXPR_RPAR, 3, newplaceholderNode(OP_LPAR), $2, newplaceholderNode(OP_RPAR));} %prec EXPR_LPAR_RPAR_INCLUSICE
    | ID { $$ = newpNode(NODETYPE_SINGLE_ID_AS_EXPR, 1, newsNode($1));} %prec EXPR_NORMAL_ID
    | ID expr { $$ = newpNode(NODETYPE_FUNC_CALL_AS_EXPR, 2, newsNode($1), $2); } %prec EXPR_FUNCTION_ID
    | ID OP_DOT INT_LIT { $$ = newpNode(NODETYPE_TUPLE_REF_AS_EXPR, 3, newsNode($1), newplaceholderNode(OP_DOT), newiNode($3)); } %prec EXPR_TUPLE_ID
    | ID OP_LBRAK expr OP_RBRAK { $$ = newpNode(NODETYPE_ARRAY_REF_AS_EXPR, 4, newsNode($1), newplaceholderNode(OP_LBRAK), $3, newplaceholderNode(OP_RBRAK)); } %prec EXPR_ARRAY_ID
;

%%