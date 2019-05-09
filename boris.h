/*
* Declarations for the boris compiler
*
* Author: Yi Zhou
* March 31, 2019
*
*/
#ifndef COMPILERDESIGNPROJECT_BORIS_H
#define COMPILERDESIGNPROJECT_BORIS_H

#include <llvm-c/Core.h>
#include <llvm-c/ExecutionEngine.h>
#include <llvm-c/Target.h>
#include <llvm-c/Analysis.h>
#include <llvm-c/BitWriter.h>

/* parameters */
#define MAX_ID_LENGTH 50
#define MAX_INT_LIT 2147483647
#define MAX_INT_LIT_LENGTH 10
#define PARSE_TREE_MAX_CHILD 50
#define MAX_TUPLE_ARRAY_SIZE 10
#define MAX_SYMBOLTABLE_SIZE 10
#define MAX_SYMBOLTABLE_STACK_SIZE 5

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
#define NODETYPE_ITER_ID 1076

/* Nodes in the Parse Tree */
struct pNode {
  int pnodetype;
  int line;
  int childscount;
  LLVMBasicBlockRef true_block; // reserved for boolexpr
  LLVMBasicBlockRef false_block;// reserved for boolexpr 
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

/* symbol table struct*/
#define GLOBAL_SCOPE 'g'
#define LOCAL_SCOPE 'l'
#define VALUETYPE_TUPLE 'T'
#define VALUETYPE_ARRAY 'A'
#define VALUETYPE_INT 'I'
#define VALUETYPE_FUNC 'F'
#define VALUETYPE_UNKNOWN 'U'        //only for vairable just after declariation
#define VALUETYPE_LINK_TO_GLOBAL 'L' //only for local scope vairable name to access the global symboltable
#define INITTYPE 0

struct symboltableRecordValue {
  LLVMValueRef address;                      // The LLVM address to the value
  int ival;                                  // reserved for VALUETYPE_INT
  int ivallist[MAX_TUPLE_ARRAY_SIZE];        // reserved for VALUETYPE_TUPLE,  VALUETYPE_ARRAY
  int ivallist_start;                        // the start index, reserved for VALUETYPE_TUPLE,  VALUETYPE_ARRAY
  int ivallistlength;                        // the total length, reserved for VALUETYPE_TUPLE,  VALUETYPE_ARRAY
};

struct symboltableRecordFunction {
  struct pNode* defnnode;
  int formal_parameter_valuetype;
  int return_valuetype;
};

struct symboltableRecord {
    int valid;                                // 0 if not valid
    char* sval;                               // ID
    int scope;                                // GLOBAL_SCOPE or LOCAL_SCOPE
    int valuetype;                            // VALUETYPE_XXXXXX
    struct symboltableRecordValue* value;     // if null, this record is not initilized
    int line;                                 // the line when this variable is declared
};

struct symboltable {
  int length;                                                 // total length
  int scope;                                                  // scope = GLOBAL_SCOPE or LOCAL_SCOPE
  struct symboltableRecord* records[MAX_SYMBOLTABLE_SIZE];    // an array of symboltableRecord pointers
};

struct symboltableStack {
  int capacity;                                                         // total length of tbarray
  int current_length;                                                   // current length, 0 if empty
  struct symboltable* tbptrs[MAX_SYMBOLTABLE_STACK_SIZE];               // an array of symboltabletable pointers
};

/* interface to the lexer, parser */
extern int yylineno; /* from lexer */
extern char *yytext; /* from lexer */
extern int yylex(); /* from lexer */
extern int yyparse(); /* from parser */
void yyerror(char *s, ...);

/* build a Parse Tree*/
struct pNode *newpNode(int type, ...);
struct pNode *newsNode(char* sval);
struct pNode *newiNode(int ival);
struct pNode *newplaceholderNode(int tok);
void visualize(struct pNode *p, int level);
void treefree(struct pNode *p);
void treewalker(struct pNode *p, struct symboltable* global_tb, struct symboltableStack* local_tbstk, LLVMBuilderRef builder, LLVMModuleRef module);

/* symbol table handler*/
# define LOCAL_ENV (local_tbstk->current_length > 0)

struct symboltableStack* init_symboltableStack(int capacity);
void pop_symboltableStack(struct symboltableStack* tbstk);
void push_symboltableStack(struct symboltable* tb, struct symboltableStack* tbstk);
struct symboltable* top_symboltableStack(struct symboltableStack* tbstk);
void remove_symboltableStack(struct symboltableStack* tbstk);
void print_symboltableStack(struct symboltableStack* tbstk);

struct symboltable* init_symboltable(int length, int scope);
void remove_symboltable(struct symboltable* tb);
void print_symboltable(struct symboltable* tb);

struct symboltableRecord* lookup_symbol(char* sval, int scope, struct symboltable* tb);
int next_available_symbol_slot(struct symboltable* tb);
struct symboltableRecord* declare_symbol(char* sval, int valuetype, int scope, int line, struct symboltable* tb);
void set_symbol_type(char* sval, int valuetype, int scope, int line, struct symboltable* tb);
void init_int_symbol(char* sval, int scope, int line, struct symboltable* tb);
void update_int_symbol(char* sval, int scope, int ival, int line, struct symboltable* tb);
void init_int_list_symbol(char* sval, int scope, int ivallist_start, int ivallistlength, int line, struct symboltable* tb);
void update_int_list_symbol_itemwise(char* sval, int scope, int updateval, int updateindex, int line, struct symboltable* tb);
void init_func_symbol(char* sval, int scope, int formal_parameter_valuetype, int return_valuetype, struct pNode* defnnode, int line, struct symboltable* tb);
void remove_symbol(char* sval, int scope, int line, struct symboltable* tb);
void print_symboltableRecord(struct symboltableRecord* record);


/*code generator*/
LLVMValueRef boris_codegen_expr(struct pNode *node,  LLVMBuilderRef builder, LLVMModuleRef module, struct symboltable* global_tb, struct symboltableStack* local_tbstk);
void verify_llvm_module_and_output(LLVMModuleRef module);
void begin_boris_module(LLVMBuilderRef builder,LLVMModuleRef module);
void end_boris_module(LLVMBuilderRef builder,LLVMModuleRef module);
#endif // COMPILERDESIGNPROJECT_BORIS_H
