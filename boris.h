/*
* Declarations for the boris compiler
*
* Author: Yi Zhou
* March 31, 2019
*
*/
#ifndef COMPILERDESIGNPROJECT_BORIS_H
#define COMPILERDESIGNPROJECT_BORIS_H

/* parameters */
#define MAX_ID_LENGTH 50
#define MAX_INT_LIT 2147483647
#define MAX_INT_LIT_LENGTH 10
#define PARSE_TREE_MAX_CHILD 50

/* colors setting*/
#define RESET   "\033[0m"
#define BLACK   "\033[30m"      /* Black */
#define RED     "\033[31m"      /* Red */
#define GREEN   "\033[32m"      /* Green */
#define YELLOW  "\033[33m"      /* Yellow */

/* Parse Tree Node Types*/
#define NODETYPE_ID 1024
#define NODETYPE_INT 1025
#define NODETYPE_PLACEHOLDER 1026
#define NODETYPE_EXPR_COMMA_EXPR 1027
#define NODETYPE_EXPR_MINUS_EXPR 1028
#define NODETYPE_EXPR_PLUS_EXPR 1029
#define NODETYPE_EXPR_DIV_EXPR 1030
#define NODETYPE_EXPR_MULT_EXPR 1031
#define NODETYPE_LPAR_EXPR_RPAR 1032
#define NODETYPE_SINGLE_ID_AS_EXPR 1033
#define NODETYPE_FUNC_CALL_AS_EXPR 1034

/* Nodes in the Parse Tree */
struct pNode {
  int pnodetype;
  struct pNode* childs[PARSE_TREE_MAX_CHILD];
};

struct sNode {
  // a terminal node for storing string 
  int pnodetype; // = NODETYPE_STRING
  char *sval;
};

struct iNode {
  // a terminal node for storing integer
  int pnodetype; // = NODETYPE_INT
  int ival;
};

struct placeholderNode {
  // a terminal node for keywords, operators, constant string tokens. etc.
  int pnodetype; // = NODETYPE_PLACEHOLDEER
  int tok;
};

/* interface to the lexer */
extern int yylineno; /* from lexer */
extern char *yytext; /* from lexer */
extern int yylex(); /* from lexer */
extern int yyparse(); /* from parser */
void yyerror(char *s, ...);
void visualize(struct pNode *p, int level);


/* build a Parse Tree*/
struct pNode *newpNode(int type, ...);
struct pNode *newsNode(char* sval);
struct pNode *newiNode(int ival);
struct pNode *newplaceholderNode(int ival);

#endif // COMPILERDESIGNPROJECT_BORIS_H