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
#define MAX_SYMBOL_TABLE_SIZE 1000

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
#define NODETYPE_TUPLE_REF_AS_EXPR 1035
#define NODETYPE_ARRAY_REF_AS_EXPR 1036
#define NODETYPE_SINGLE_ID_AS_LHSITEM 1037
#define NODETYPE_TUPLE_REF_AS_LHSITEM 1038
#define NODETYPE_ARRAY_REF_AS_LHSITEM 1039
#define NODETYPE_LHS 1040
#define NODETYPE_COMMA_LHSITEN_LIST 1041
#define NODETYPE_BOOLEXPR 1042
#define NODETYPE_RANGE 1043
#define NODETYPE_LHS_ASSIGN_EXPR_AS_STATEMENT 1044
#define NODETYPE_LHS_EXCHANGE_LHS_AS_STATEMENT 1045
#define NODETYPE_STATEMENT_LIST 1046
#define NODETYPE_WHILE_STATEMENT 1047
#define NODETYPE_ELSIF_SENTENCE 1048
#define NODETYPE_ELSE_SENTENCE 1049
#define NODETYPE_ELSIF_SENTENCE_LIST 1050
#define NODETYPE_IF_STATEMENT 1051
#define NODETYPE_IF_ELSE_STATEMENT 1052
#define NODETYPE_FOREACH_RANGE_STATEMENT 1053
#define NODETYPE_ARRAY_ID 1054
#define NODETYPE_FOREACH_ARRAYID_STATEMENT 1055
#define NODETYPE_PRINT_STATEMENT 1056
#define NODETYPE_RETURN_STATEMENT 1057
#define NODETYPE_SINGLE_INT_AS_EXPR 1058
#define NODETYPE_LOCAL_DECL 1059
#define NODETYPE_GLOBAL_DECL 1060
#define NODETYPE_ARRAY_DECL 1061
#define NODETYPE_ARRAY_DECL_WITH_ANONY_FUNC 1062
#define NODETYPE_STATEMENT_AS_SD 1063
#define NODETYPE_DECL_AS_SD 1064
#define NODETYPE_SD_LIST 1065
#define NODETYPE_BODY 1066
#define NODETYPE_COMMA_ID_LIST 1067
#define NODETYPE_FUNC_DEFN 1068
#define NODETYPE_STATEMENT_AS_SDD 1069
#define NODETYPE_DECL_AS_SDD 1070
#define NODETYPE_DEFN_AS_SDD 1071
#define NODETYPE_SDD_LIST 1072
#define NODETYPE_ROOT_INPUT 1073
#define NODETYPE_NO_EXPR_GLOBAL_DECL 1074
#define NODETYPE_NO_EXPR_LOCAL_DECL 1075

/* Nodes in the Parse Tree */
struct pNode {
  int pnodetype;
  int line;
  int childscount;
  struct pNode* childs[PARSE_TREE_MAX_CHILD];
};

struct sNode {
  // a terminal node for storing string 
  int pnodetype; // = NODETYPE_ID
  int line;
  int childscount; // = 0
  char *sval;
};

struct iNode {
  // a terminal node for storing integer
  int pnodetype; // = NODETYPE_INT
  int line;
  int childscount; // = 0
  int ival;
};

struct placeholderNode {
  // a terminal node for keywords, operators, constant string tokens. etc.
  int pnodetype; // = NODETYPE_PLACEHOLDEER
  int line;
  int childscount; // = 0
  int tok;
  char tokstr[20]; 
};

/* interface to the lexer */
extern int yylineno; /* from lexer */
extern char *yytext; /* from lexer */
extern int yylex(); /* from lexer */
extern int yyparse(); /* from parser */
void yyerror(char *s, ...);
void visualize(struct pNode *p, int level);
void treefree(struct pNode *p);


/* build a Parse Tree*/
struct pNode *newpNode(int type, ...);
struct pNode *newsNode(char* sval);
struct pNode *newiNode(int ival);
struct pNode *newplaceholderNode(int tok);


/* symbol table struct*/
#define GLOBAL_SCOPE 'g'
#define LOCAL_SCOPE 'l'
#define VALUETYPE_TUPLE 'T'
#define VALUETYPE_ARRAY 'A'
#define VALUETYPE_INT 'I'
#define VALUETYPE_FUNC 'F'
#define INITTYPE 0

struct symboltableRecordValue {
  int ival;             // reserved for VALUETYPE_INT
  int* ivallist;        // reserved for VALUETYPE_TUPLE,  VALUETYPE_ARRAY
  int ivallist_start;   // the start index, reserved for VALUETYPE_TUPLE,  VALUETYPE_ARRAY
  int ivallistlength;   // the total length, reserved for VALUETYPE_TUPLE,  VALUETYPE_ARRAY
};

struct symboltableRecord {
    int valid;                                // 0 if not valid
    char* sval;                               // ID
    int scope;                                // GLOBAL_SCOPE or LOCAL_SCOPE
    int valuetype;                            // VALUETYPE_TUPLE,  VALUETYPE_ARRAY,  VALUETYPE_INT
    struct symboltableRecordValue* value;     // if null, this record is not initilized
    int line;                                 // the line when this variable is declared
};

struct symboltable {
  int length;                           // total length
  int scope;                            // scope = GLOBAL_SCOPE or LOCAL_SCOPE
  struct symboltableRecord* records;    // am array of symboltableRecord
};

#endif // COMPILERDESIGNPROJECT_BORIS_H