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
#define PARSE_TREE_MAX_CHILD 20

/* colors setting*/
#define RESET   "\033[0m"
#define BLACK   "\033[30m"      /* Black */
#define RED     "\033[31m"      /* Red */
#define GREEN   "\033[32m"      /* Green */
#define YELLOW  "\033[33m"      /* Yellow */

/* Parse Tree Node Types*/
#define NODETYPE_ID 1024
#define NODETYPE_INT 1025


/* interface to the lexer */
extern int yylineno; /* from lexer */
extern char * yytext; /* from lexer */
extern int yylex(); /* from lexer */
void yyerror(char *s, ...);

/* Nodes in the Abstract Syntax Tree */
struct Node {
  int nodetype;
  struct Node* child[PARSE_TREE_MAX_CHILD];
};


#endif // COMPILERDESIGNPROJECT_BORIS_H