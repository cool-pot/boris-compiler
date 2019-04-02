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
#define NODETYPE_STRING 1024
#define NODETYPE_INT 1025
#define NODETYPE_EXPR_COMMA_EXPR 1026


/* interface to the lexer */
extern int yylineno; /* from lexer */
extern char *yytext; /* from lexer */
extern int yylex(); /* from lexer */
extern int yyparse(); /* from parser */
void yyerror(char *s, ...);

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


/* build a Parse Tree*/
struct pNode *newpNode(int type, ...);
struct pNode *newsNode(char* sval);
struct pNode *newiNode(int ival);

#endif // COMPILERDESIGNPROJECT_BORIS_H