/*
* Functions for the boris compiler
*
* Author: Yi Zhou
* March 31, 2019
*
*/

# include <stdio.h>
# include <stdlib.h>
# include <stdarg.h>
# include <string.h>
# include "boris.h"
# include "boris.tab.h" // yylloc definition, token number

# define LOCAL_ENV (local_tbstk->current_length > 0)


void yyerror(char* s, ...){
    va_list ap;
    va_start(ap, s);
    fprintf(stderr, RED);
    fprintf(stderr, "position %d,%d-%d,%d: ",yylloc.first_line,yylloc.first_column, yylloc.last_line, yylloc.last_column);
    vfprintf(stderr, s, ap);
    fprintf(stderr, RESET);
    fprintf(stderr, "\n");
}

void scannerTester(){ 
    printf("> Hello from scannerTester for boris compiler!\n");
    int tok;
    do {
        tok = yylex();
        if(tok == 0) break;
        printf("token[%d].", tok);
        if (tok == END_OF_LINE) {
            printf("[\\n]");
        } else{ 
            printf("[%s]", yytext); 
        }
        printf(" In position %d,%d-%d,%d.",yylloc.first_line,yylloc.first_column, yylloc.last_line, yylloc.last_column);
        if (tok == ID) {
            printf(GREEN);
            printf("[ID] Retained infomation: yylval.sval=%s",yylval.sval);
            printf(RESET);
        } else if (tok == INT_LIT){
            printf(GREEN);
            printf("[INT_LIT] Retained infomation: yylval.ival=%d",yylval.ival);
            printf(RESET);
        }
        
        printf("\n");
    } while (1);
}

void parserTester(){ 
    printf("> Hello from parserTester for boris compiler!\n");
    yyparse();
    return;
}

struct pNode *newpNode(int type, ...){
    /* Initialize a new parse tree node*/
    /* arguments should in thee following format: int num, struct Node* child_1, struct Node* child_2, struct Node* child_3, ..., struct Node* child_num*/
    
    struct pNode * parent = malloc(sizeof(struct pNode));
    if(!parent) {
        yyerror("Out of space.");
        exit(0);
    }

    va_list arguments;                     
    va_start ( arguments, type );           
    int num = va_arg ( arguments, int );
    if (num > PARSE_TREE_MAX_CHILD) {
        yyerror("Parse tree node initialization error, too many child nodes");
        exit(996);
    }
    parent->line = yylloc.first_line;
    parent->childscount = num;
    parent->pnodetype = type;
    int i = 0;
    for ( ; i < num; i++ ) {
        parent->childs[i] = va_arg ( arguments, struct pNode * ); 
    }
    for ( ; i < PARSE_TREE_MAX_CHILD; i++) {
        parent->childs[i] = NULL;
    }
    va_end ( arguments );
    return parent; 
}

struct pNode *newsNode(char* sval){
    struct sNode * parent = malloc(sizeof(struct sNode));
    if(!parent) {
        yyerror("Out of space.");
        exit(0);
    }
    parent->pnodetype = NODETYPE_ID;
    parent->line = yylloc.first_line;
    parent->childscount = 0;
    parent->sval = sval; 
    return (struct pNode *)parent;
}

struct pNode *newiNode(int ival){
    struct iNode * parent = malloc(sizeof(struct iNode));
    if(!parent) {
        yyerror("Out of space.");
        exit(0);
    }
    parent->pnodetype = NODETYPE_INT;
    parent->line = yylloc.first_line;
    parent->childscount = 0;
    parent->ival = ival; 
    return (struct pNode *)parent;
}


struct pNode *newplaceholderNode(int tok){
    struct placeholderNode * parent = malloc(sizeof(struct placeholderNode));
    if(!parent) {
        yyerror("Out of space.");
        exit(0);
    }
    parent->pnodetype = NODETYPE_PLACEHOLDER;
    parent->line = yylloc.first_line;
    parent->childscount = 0;
    parent->tok = tok; 
    switch (tok)
    {
        case KW_ARRAY:{ strcpy(parent->tokstr, "array"); break; }
        case KW_DEFUN:{ strcpy(parent->tokstr, "defun"); break; }
        case KW_DO:{ strcpy(parent->tokstr, "do"); break; }
        case KW_ELSE:{ strcpy(parent->tokstr, "else"); break; }
        case KW_ELSIF:{ strcpy(parent->tokstr, "elsif"); break; }
        case KW_END:{ strcpy(parent->tokstr, "end"); break; }
        case KW_FOR:{ strcpy(parent->tokstr, "for"); break; }
        case KW_FOREACH:{ strcpy(parent->tokstr, "foreach"); break; }
        case KW_GLOBAL:{ strcpy(parent->tokstr, "global"); break; }
        case KW_IF:{ strcpy(parent->tokstr, "if"); break; }
        case KW_IN:{ strcpy(parent->tokstr, "in"); break; }
        case KW_LOCAL:{ strcpy(parent->tokstr, "local"); break; }
        case KW_PRINT:{ strcpy(parent->tokstr, "print"); break; }
        case KW_RETURN:{ strcpy(parent->tokstr, "return"); break; }
        case KW_THEN:{ strcpy(parent->tokstr, "then"); break; }
        case KW_TUPLE:{ strcpy(parent->tokstr, "tuple"); break; }
        case KW_WHILE:{ strcpy(parent->tokstr, "while"); break; }
        case OP_ASSIGN:{ strcpy(parent->tokstr, "="); break; }
        case OP_COMMA:{ strcpy(parent->tokstr, ","); break; }
        case OP_DIV:{ strcpy(parent->tokstr, "/"); break; }
        case OP_DOT:{ strcpy(parent->tokstr, "."); break; }
        case OP_DOTDOT:{ strcpy(parent->tokstr, ".."); break; }
        case OP_EQUAL:{ strcpy(parent->tokstr, "=="); break; }
        case OP_EXCHANGE:{ strcpy(parent->tokstr, "<->"); break; }
        case OP_GREATER:{ strcpy(parent->tokstr, ">"); break; }
        case OP_GREATEREQUAL:{ strcpy(parent->tokstr, ">="); break; }
        case OP_LBRAK:{ strcpy(parent->tokstr, "["); break; }
        case OP_LESS:{ strcpy(parent->tokstr, "<"); break; }
        case OP_LESSEQUAL:{ strcpy(parent->tokstr, "<="); break; }
        case OP_LPAR:{ strcpy(parent->tokstr, "("); break; }
        case OP_MINUS:{ strcpy(parent->tokstr, "-"); break; }
        case OP_MULT:{ strcpy(parent->tokstr, "*"); break; }
        case OP_NOTEQUA:{ strcpy(parent->tokstr, "!="); break; }
        case OP_PLUS:{ strcpy(parent->tokstr, "+"); break; }
        case OP_RBRAK:{ strcpy(parent->tokstr, "]"); break; }
        case OP_RPAR:{ strcpy(parent->tokstr, ")"); break; }
        case OP_SEMI:{ strcpy(parent->tokstr, ";"); break; }
        default:{ strcpy(parent->tokstr, "unknown"); break; }
    }
    return (struct pNode *)parent;
}

void printManySpace(int count){
    for (int i = 0; i < count; i++) printf(" ");
}

void visualize(struct pNode *p, int level){
    if (level == 0) printf("Start parse tree visualization:\n\n"); 
    if (p == NULL) {
        printManySpace(level*4);
        printf("[EMPTY_NODE]\n"); 
        return;
    }
    char nodetype2nodestr[][100] = {
        "ID",                                       //1024
        "INT",                                      //1025
        "PLACEHOLDER",                              //1026
        "EXPR_COMMA_EXPR",                          //1027
        "EXPR_MINUS_EXPR",                          //1028
        "EXPR_PLUS_EXPR",                           //1029
        "EXPR_DIV_EXPR",                            //1030
        "EXPR_MULT_EXPR",                           //1031
        "LPAR_EXPR_LPAR",                           //1032
        "SINGLE_ID_AS_EXPR",                        //1033
        "FUNC_CALL_AS_EXPR",                        //1034
        "TUPLE_REF_AS_EXPR",                        //1035
        "ARRAY_REF_AS_EXPR",                        //1036
        "SINGLE_ID_AS_LHSITEM",                     //1037
        "TUPLE_REF_AS_LHSITEM",                     //1038
        "ARRAY_REF_AS_LHSITEM",                     //1039
        "LHS",                                      //1040
        "COMMA_LHSITEN_LIST",                       //1041
        "BOOLEXPR",                                 //1042
        "RANGE",                                    //1043
        "LHS_ASSIGN_EXPR_AS_STATEMENT",             //1044
        "LHS_EXCHANGE_LHS_AS_STATEMENT",            //1045
        "STATEMENT_LIST",                           //1046
        "WHILE_STATEMENT",                          //1047
        "ELSIF_SENTENCE",                           //1048
        "ELSE_SENTENCE",                            //1049
        "ELSIF_SENTENCE_LIST",                      //1050
        "IF_STATEMENT",                             //1051
        "IF_ELSE_STATEMENT",                        //1052
        "FOREACH_RANGE_STATEMENT",                  //1053
        "ARRAY_ID",                                 //1054
        "FOREACH_ARRAYID_STATEMENT",                //1055
        "PRINT_STATEMENT",                          //1056
        "RETURN_STATEMENT",                         //1057
        "SINGLE_INT_AS_EXPR",                       //1058
        "LOCAL_DECL",                               //1059
        "GLOBAL_DECL",                              //1060
        "ARRAY_DECL",                               //1061
        "ARRAY_DECL_WITH_ANONY_FUNC",               //1062
        "STATEMENT_AS_SD",                          //1063
        "DECL_AS_SD",                               //1064
        "SD_LIST",                                  //1065
        "BODY",                                     //1066
        "COMMA_ID_LIST",                            //1067
        "FUNC_DEFN",                                //1068
        "STATEMENT_AS_SDD",                         //1069
        "DECL_AS_SDD",                              //1070
        "DEFN_AS_SDD",                              //1071
        "SDD_LIST",                                 //1072
        "ROOT_INPUT",                               //1073
        "NO_EXPR_GLOBAL_DECL",                      //1074
        "NO_EXPR_LOCAL_DECL",                       //1075
        "ITER_ID",                                  //1076
    };

    switch(p->pnodetype) {
        case NODETYPE_ID: {
            printManySpace(level*4);
            char* nodestr = nodetype2nodestr[p->pnodetype-NODETYPE_ID];
            printf("[%s]%s (@line %d)\n", nodestr, ((struct sNode*)p)->sval, p->line);
            break;
        }
        case NODETYPE_INT: {
            printManySpace(level*4); 
            char* nodestr = nodetype2nodestr[p->pnodetype-NODETYPE_ID];
            printf("[%s]%d  (@line %d)\n", nodestr, ((struct iNode*)p)->ival, p->line);
            break;
        }
        case NODETYPE_PLACEHOLDER: {
            printManySpace(level*4); 
            char* nodestr = nodetype2nodestr[p->pnodetype-NODETYPE_ID];
            printf("[%s-%d]%s (@line %d)\n", nodestr, ((struct placeholderNode*)p)->tok, ((struct placeholderNode*)p)->tokstr, p->line);
            break;
        }
        case NODETYPE_EXPR_COMMA_EXPR:
        case NODETYPE_EXPR_MINUS_EXPR:
        case NODETYPE_EXPR_PLUS_EXPR:
        case NODETYPE_EXPR_MULT_EXPR:
        case NODETYPE_EXPR_DIV_EXPR:
        case NODETYPE_LPAR_EXPR_RPAR:
        case NODETYPE_FUNC_CALL_AS_EXPR:
        case NODETYPE_SINGLE_ID_AS_EXPR:
        case NODETYPE_TUPLE_REF_AS_EXPR:
        case NODETYPE_ARRAY_REF_AS_EXPR:
        case NODETYPE_SINGLE_ID_AS_LHSITEM:
        case NODETYPE_TUPLE_REF_AS_LHSITEM:
        case NODETYPE_ARRAY_REF_AS_LHSITEM:
        case NODETYPE_LHS:
        case NODETYPE_COMMA_LHSITEN_LIST:
        case NODETYPE_BOOLEXPR:
        case NODETYPE_RANGE:
        case NODETYPE_LHS_ASSIGN_EXPR_AS_STATEMENT:
        case NODETYPE_LHS_EXCHANGE_LHS_AS_STATEMENT:
        case NODETYPE_STATEMENT_LIST:
        case NODETYPE_WHILE_STATEMENT:
        case NODETYPE_ELSIF_SENTENCE:
        case NODETYPE_ELSE_SENTENCE:
        case NODETYPE_ELSIF_SENTENCE_LIST:
        case NODETYPE_IF_STATEMENT:
        case NODETYPE_IF_ELSE_STATEMENT:
        case NODETYPE_FOREACH_RANGE_STATEMENT:
        case NODETYPE_ARRAY_ID:
        case NODETYPE_FOREACH_ARRAYID_STATEMENT:
        case NODETYPE_PRINT_STATEMENT:
        case NODETYPE_RETURN_STATEMENT:
        case NODETYPE_SINGLE_INT_AS_EXPR:
        case NODETYPE_LOCAL_DECL:
        case NODETYPE_GLOBAL_DECL:
        case NODETYPE_ARRAY_DECL:
        case NODETYPE_ARRAY_DECL_WITH_ANONY_FUNC:
        case NODETYPE_STATEMENT_AS_SD:
        case NODETYPE_DECL_AS_SD:
        case NODETYPE_SD_LIST:
        case NODETYPE_BODY:
        case NODETYPE_COMMA_ID_LIST:
        case NODETYPE_FUNC_DEFN:
        case NODETYPE_STATEMENT_AS_SDD:
        case NODETYPE_DECL_AS_SDD:
        case NODETYPE_DEFN_AS_SDD:
        case NODETYPE_SDD_LIST:
        case NODETYPE_ROOT_INPUT:
        case NODETYPE_NO_EXPR_GLOBAL_DECL:
        case NODETYPE_NO_EXPR_LOCAL_DECL:
        case NODETYPE_ITER_ID:{
            printManySpace(level*4); 
            char* nodestr = nodetype2nodestr[p->pnodetype-NODETYPE_ID];
            printf("[%s] (@line %d)\n", nodestr, p->line);
            for (int i = 0; i < p->childscount; i++){
                struct pNode *child = p->childs[i];
                visualize(child, level+1);
            }
            break;
        }
        default: printf("internal error: bad node type%d\n", p->pnodetype);
    }
}

void treefree(struct pNode *p){
    if (p == NULL) return;
    switch(p->pnodetype) {
        /* non-terminals */
        case NODETYPE_EXPR_COMMA_EXPR:
        case NODETYPE_EXPR_MINUS_EXPR:
        case NODETYPE_EXPR_PLUS_EXPR:
        case NODETYPE_EXPR_MULT_EXPR:
        case NODETYPE_EXPR_DIV_EXPR:
        case NODETYPE_LPAR_EXPR_RPAR:
        case NODETYPE_FUNC_CALL_AS_EXPR:
        case NODETYPE_SINGLE_ID_AS_EXPR:
        case NODETYPE_TUPLE_REF_AS_EXPR:
        case NODETYPE_ARRAY_REF_AS_EXPR:
        case NODETYPE_SINGLE_ID_AS_LHSITEM:
        case NODETYPE_TUPLE_REF_AS_LHSITEM:
        case NODETYPE_ARRAY_REF_AS_LHSITEM:
        case NODETYPE_LHS:
        case NODETYPE_COMMA_LHSITEN_LIST:
        case NODETYPE_BOOLEXPR:
        case NODETYPE_RANGE:
        case NODETYPE_LHS_ASSIGN_EXPR_AS_STATEMENT:
        case NODETYPE_LHS_EXCHANGE_LHS_AS_STATEMENT:
        case NODETYPE_STATEMENT_LIST:
        case NODETYPE_WHILE_STATEMENT:
        case NODETYPE_ELSIF_SENTENCE:
        case NODETYPE_ELSE_SENTENCE:
        case NODETYPE_ELSIF_SENTENCE_LIST:
        case NODETYPE_IF_STATEMENT:
        case NODETYPE_IF_ELSE_STATEMENT:
        case NODETYPE_FOREACH_RANGE_STATEMENT:
        case NODETYPE_ARRAY_ID:
        case NODETYPE_FOREACH_ARRAYID_STATEMENT:
        case NODETYPE_PRINT_STATEMENT:
        case NODETYPE_RETURN_STATEMENT:
        case NODETYPE_SINGLE_INT_AS_EXPR:
        case NODETYPE_LOCAL_DECL:
        case NODETYPE_GLOBAL_DECL:
        case NODETYPE_ARRAY_DECL:
        case NODETYPE_ARRAY_DECL_WITH_ANONY_FUNC:
        case NODETYPE_STATEMENT_AS_SD:
        case NODETYPE_DECL_AS_SD:
        case NODETYPE_SD_LIST:
        case NODETYPE_BODY:
        case NODETYPE_COMMA_ID_LIST:
        case NODETYPE_FUNC_DEFN:
        case NODETYPE_STATEMENT_AS_SDD:
        case NODETYPE_DECL_AS_SDD:
        case NODETYPE_DEFN_AS_SDD:
        case NODETYPE_SDD_LIST:
        case NODETYPE_ROOT_INPUT:
        case NODETYPE_NO_EXPR_GLOBAL_DECL:
        case NODETYPE_NO_EXPR_LOCAL_DECL:
        case NODETYPE_ITER_ID:{
            for (int i = 0; i < p->childscount; i++){
                treefree(p->childs[i]);
            }
            free(p);
            break;
        }
        /* terminals */
        case NODETYPE_INT:
        case NODETYPE_PLACEHOLDER:{
            free(p);
            break;
        }
        case NODETYPE_ID:{
            free(((struct sNode*)p)->sval);
            free(p);
            break;
        }
        default: printf("internal error: free bad node %d\n", p->pnodetype);
    }
}

/* 
 * Following: tree walker and type [synthesis, inference]
 */

// look up the type of a symbol named sval, based current context
// return type in {int, tuple, array, func, unknown}, no {link}
int type_lookup(char* sval, struct symboltable* global_tb, struct symboltableStack* local_tbstk) {
    if (LOCAL_ENV) {
        struct symboltable* local_tb = top_symboltableStack(local_tbstk);
        struct symboltableRecord* record = lookup_symbol(sval, LOCAL_SCOPE, local_tb);
        if (record) {
            fprintf(stderr, GREEN"[type_lookup]: %s is a local `%c` symbol. declared in line %d\n"RESET, sval, record->valuetype, record->line);
            if (record->valuetype != VALUETYPE_LINK_TO_GLOBAL){ 
                //if not a link to global variable, return it directly
                return record->valuetype;
            } else {
                // if it's a link to global variable, lookup it in global table
                struct symboltableRecord* g_record = lookup_symbol(sval, GLOBAL_SCOPE, global_tb);
                fprintf(stderr, GREEN"[type_lookup -> redirected]: %s is a global `%c` symbol. declared in line %d\n"RESET, sval, g_record->valuetype, g_record->line);
                if (g_record) return g_record->valuetype;
            }
        }
    } else {
        struct symboltableRecord* record = lookup_symbol(sval, GLOBAL_SCOPE, global_tb);
        if (record) {
            fprintf(stderr, GREEN"[type_lookup]: %s is a global `%c` symbol.  declared in line %d\n"RESET, sval, record->valuetype, record->line);
            return record->valuetype;
        }
    }
    fprintf(stderr, RED"[type_lookup]: %s is not found in current envrionment.\n"RESET, sval);
    exit(999);
    return -1;
}

// first check out local if possible, otherwise lookup global. if not found, return NULL
struct symboltable* get_matched_symboltable(char* sval, struct symboltable* global_tb, struct symboltableStack* local_tbstk){
    if(LOCAL_ENV){
        struct symboltable* local_tb = top_symboltableStack(local_tbstk);
        struct symboltableRecord* record = lookup_symbol(sval, LOCAL_SCOPE, local_tb);
        if(record) return local_tb;
    }
    struct symboltableRecord* g_record = lookup_symbol(sval, GLOBAL_SCOPE, global_tb);
    if(g_record){
        return global_tb;
    }
    return NULL;
}

int check_type_in_list(int valuetype, int truth[], int length){
    int equal = 0;
    for (int i = 0; i < length; i++){
        if (valuetype == truth[i]) equal++;
    }
    if (equal == 0) {
        fprintf(stderr, RED"[check_type_in_list]type conflicts `%c`(%d), should be in [", valuetype, valuetype);
        for (int i = 0; i < length; i++){
            if (i == length-1) fprintf(stderr, "`%c`", truth[i]);
            else fprintf(stderr, "`%c`,", truth[i]);
        }
        fprintf(stderr, "]\n"RESET);
    }
    return equal;
}

int check_type_equal(int valuetype, int truth){
    int trutharray[] = {truth};
    return check_type_in_list(valuetype, trutharray, 1);
}

int check_current_scope(struct symboltable* global_tb, struct symboltableStack* local_tbstk, int expected_scope){
    int current_scope = LOCAL_ENV ? LOCAL_SCOPE : GLOBAL_SCOPE;
    if (current_scope != expected_scope) {
        fprintf(stderr, RED"[check_current_scope]: scope not expected. get `%c`, should be `%c`.\n"RESET, current_scope, expected_scope);
        return 0;
    }
    return 1;
}

// test prpose only
void mock_local_env(struct symboltable* global_tb, struct symboltableStack* local_tbstk){
    struct symboltable* tb = init_symboltable(MAX_SYMBOLTABLE_SIZE, LOCAL_SCOPE);
    push_symboltableStack(tb, local_tbstk);
}

// calclate NODETYPE_LHS depth
int get_node_depth(struct pNode* p){
    if (p == NULL) return 0;
    switch (p->pnodetype)
    {
        case NODETYPE_SINGLE_ID_AS_LHSITEM:
        case NODETYPE_ARRAY_REF_AS_LHSITEM:
        case NODETYPE_TUPLE_REF_AS_LHSITEM:
            return 1;
        case NODETYPE_COMMA_LHSITEN_LIST:
        case NODETYPE_LHS:{
            int sum = 0;
            for (int i = 0; i < p->childscount; i++){
                sum += get_node_depth(p->childs[i]);
            }
            return sum;
        }
        default:
            return 0;
    }
}

// determine type of expression: if A = expr(B), then A's type can be synthesised from right hand side
int type_synthesis(struct pNode* exprnode, struct symboltable* global_tb, struct symboltableStack* local_tbstk){
    if (exprnode == NULL) return -1;
    switch(exprnode->pnodetype){
        case NODETYPE_EXPR_COMMA_EXPR:{
            /* For expr including ',', make sure left and right are both int or tuple
             *
             * expr: expr OP_COMMA expr { $$ = newpNode(NODETYPE_EXPR_COMMA_EXPR, 3, $1, newplaceholderNode(OP_COMMA), $3);}
             */
            int left_valuetype = type_synthesis(exprnode->childs[0], global_tb, local_tbstk);
            int right_valuetype = type_synthesis(exprnode->childs[2], global_tb, local_tbstk);
            int truthlist[] = {VALUETYPE_TUPLE, VALUETYPE_INT};
            check_type_in_list(left_valuetype, truthlist, 2); 
            check_type_in_list(right_valuetype, truthlist, 2); 
            return VALUETYPE_TUPLE;
            break;
        }
        case NODETYPE_EXPR_MINUS_EXPR:
        case NODETYPE_EXPR_PLUS_EXPR:
        case NODETYPE_EXPR_DIV_EXPR:
        case NODETYPE_EXPR_MULT_EXPR:{
            /* For expr including `+`,`-`,`*`,`/` make sure left and right are both int
             *
             * expr: expr OP_MINUS expr { $$ = newpNode(NODETYPE_EXPR_MINUS_EXPR, 3, $1, newplaceholderNode(OP_MINUS), $3);}
             *  | expr OP_PLUS expr { $$ = newpNode(NODETYPE_EXPR_PLUS_EXPR, 3, $1, newplaceholderNode(OP_PLUS), $3);}
             *  | expr OP_DIV expr { $$ = newpNode(NODETYPE_EXPR_DIV_EXPR, 3, $1, newplaceholderNode(OP_DIV), $3);}
             *  | expr OP_MULT expr { $$ = newpNode(NODETYPE_EXPR_MULT_EXPR, 3, $1, newplaceholderNode(OP_MULT), $3);}
             */
            int left_valuetype = type_synthesis(exprnode->childs[0], global_tb, local_tbstk);
            int right_valuetype = type_synthesis(exprnode->childs[2], global_tb, local_tbstk);
            check_type_equal(left_valuetype, VALUETYPE_INT); 
            check_type_equal(right_valuetype, VALUETYPE_INT); 
            return VALUETYPE_INT;
            break;
        }
        case NODETYPE_LPAR_EXPR_RPAR:{
           /* Return no matter whatever is a type of the inside expression. No type check here. 
            *
            * expr: OP_LPAR expr OP_RPAR { $$ = newpNode(NODETYPE_LPAR_EXPR_RPAR, 3, newplaceholderNode(OP_LPAR), $2, newplaceholderNode(OP_RPAR));} %prec EXPR_LPAR_RPAR_INCLUSICE
            */
           int valuetype = type_synthesis(exprnode->childs[1], global_tb, local_tbstk);
           return valuetype;
           break;
        }
        case NODETYPE_SINGLE_ID_AS_EXPR:{
           /* Return no matter whatever is a type of the in the . No type check here. 
            *
            * expr: ID { $$ = newpNode(NODETYPE_SINGLE_ID_AS_EXPR, 1, newsNode($1));} %prec EXPR_NORMAL_ID
            */
           struct sNode * snode = (struct sNode *)(exprnode->childs[0]);
           int valuetype = type_lookup(snode->sval, global_tb, local_tbstk);
           return valuetype;
           break;
        }
        case NODETYPE_FUNC_CALL_AS_EXPR:{
            /* TODO
            * 
            * expr: ID expr { $$ = newpNode(NODETYPE_FUNC_CALL_AS_EXPR, 2, newsNode($1), $2); } %prec EXPR_FUNCTION_ID
            */
           fprintf(stderr, RED"type_synthesis for function call, not implemented"RESET); //TODO
           exit(-1);
        }
        case NODETYPE_TUPLE_REF_AS_EXPR:{
            /* For expr including `.` make sure left is a id for tuple.
            *
            * expr: ID OP_DOT INT_LIT { $$ = newpNode(NODETYPE_TUPLE_REF_AS_EXPR, 3, newsNode($1), newplaceholderNode(OP_DOT), newiNode($3)); } %prec EXPR_TUPLE_ID
            */
            struct sNode * snode = (struct sNode *)(exprnode->childs[0]);
            int valuetype = type_lookup(snode->sval, global_tb, local_tbstk);
            check_type_equal(valuetype, VALUETYPE_TUPLE);
            return VALUETYPE_INT;
        }
        case NODETYPE_ARRAY_REF_AS_EXPR:{
            /* For expr including `[ ]` make sure left is a id for array. right is a int expr
            *
            * expr: ID OP_LBRAK expr OP_RBRAK { $$ = newpNode(NODETYPE_ARRAY_REF_AS_EXPR, 4, newsNode($1), newplaceholderNode(OP_LBRAK), $3, newplaceholderNode(OP_RBRAK)); } %prec EXPR_ARRAY_ID
            */
            struct sNode * snode = (struct sNode *)(exprnode->childs[0]);
            int left_valuetype = type_lookup(snode->sval, global_tb, local_tbstk);
            int right_valuetype = type_synthesis(exprnode->childs[2], global_tb, local_tbstk);
            check_type_equal(left_valuetype, VALUETYPE_ARRAY);
            check_type_equal(right_valuetype, VALUETYPE_INT);
           
        }
        case NODETYPE_SINGLE_INT_AS_EXPR:{
            /* Base case.
            *
            * expr: INT_LIT { $$ = newpNode(NODETYPE_SINGLE_INT_AS_EXPR, 1, newiNode($1));} %prec EXPR_INT
            */
            return VALUETYPE_INT;
            break;
        }
        default: {
            printf("visit unsupported node type for type_synthesis: %d\n", exprnode->pnodetype);
        }
    }
    return -1;
}

// determine the lhs's type based on the way it's used.
int type_inference(struct pNode* p, struct symboltable* global_tb, struct symboltableStack* local_tbstk){
    if (p == NULL) return -1;
    switch (p->pnodetype)
    {
        case NODETYPE_SINGLE_ID_AS_LHSITEM:{
            // only int, tuple, unknown are allowed as a single id in lhs. 
            struct sNode * snode = (struct sNode *)(p->childs[0]);
            int valuetype = type_lookup(snode->sval, global_tb, local_tbstk);
            int truthlist[] = {VALUETYPE_TUPLE, VALUETYPE_INT, VALUETYPE_UNKNOWN};
            check_type_in_list(valuetype, truthlist, 3); 
            return valuetype;
            break;
        }
        case NODETYPE_TUPLE_REF_AS_LHSITEM:{
            // only tuple is allowed in tuple ref
            struct sNode * snode = (struct sNode *)(p->childs[0]);
            int valuetype = type_lookup(snode->sval, global_tb, local_tbstk);
            check_type_equal(valuetype, VALUETYPE_TUPLE); 
            return VALUETYPE_INT;
            break;
        }
        case NODETYPE_ARRAY_REF_AS_LHSITEM:{
            // only array is allowed in array ref 
            struct sNode * snode = (struct sNode *)(p->childs[0]);
            int valuetype = type_lookup(snode->sval, global_tb, local_tbstk);
            check_type_equal(valuetype, VALUETYPE_ARRAY); 
            return VALUETYPE_INT;
            break;
        }
        case NODETYPE_COMMA_LHSITEN_LIST:{
            // return the type of first lhsitem as return type
            struct pNode * lhsnode = p->childs[1];
            struct pNode * listnode = p->childs[2];
            int truthlist[] = {VALUETYPE_TUPLE, VALUETYPE_INT};
            int valuetype = type_inference(lhsnode, global_tb, local_tbstk);
            check_type_in_list(valuetype, truthlist, 2); 
            if (listnode){
                type_inference(listnode, global_tb, local_tbstk);//recusively check the rest
            }
            return valuetype;
            break;
        }
        case NODETYPE_LHS:{
            // only check the type of first lhsitem
            struct pNode * lhsitem = p->childs[0];
            struct pNode * listnode = p->childs[1];
            if (listnode){
                type_inference(listnode, global_tb, local_tbstk);//recusively check the rest
                return VALUETYPE_TUPLE;
            } else {
                return type_inference(lhsitem, global_tb, local_tbstk);
            }
            break;
        }
        default: {
            printf("visit unsupported node type for type_inference: %d\n", p->pnodetype);
        }
    }
    return -1;
}

void treewalker(struct pNode* p, struct symboltable* global_tb, struct symboltableStack* local_tbstk){
    if (p == NULL) return;
    switch(p->pnodetype){
        case NODETYPE_ROOT_INPUT:{
            printf("> Start walking on parse tree:\n\n");
            treewalker(p->childs[0], global_tb, local_tbstk);
            break;
        }
        case NODETYPE_SDD_LIST:
        case NODETYPE_DECL_AS_SDD:
        case NODETYPE_STATEMENT_AS_SDD:
        case NODETYPE_STATEMENT_LIST:
        // the type checking with in control flow, will just fall through
        case NODETYPE_WHILE_STATEMENT:
        case NODETYPE_IF_STATEMENT:
        case NODETYPE_IF_ELSE_STATEMENT:
        case NODETYPE_ELSIF_SENTENCE:
        case NODETYPE_ELSE_SENTENCE:
        case NODETYPE_ELSIF_SENTENCE_LIST:
        case NODETYPE_PLACEHOLDER:
        {
            for (int i = 0; i < p->childscount; i++)
                treewalker(p->childs[i], global_tb, local_tbstk);
            break;
        }
        case NODETYPE_LHS_ASSIGN_EXPR_AS_STATEMENT:{
            //lhs OP_ASSIGN expr OP_SEMI
            struct pNode* lhsnode = p->childs[0]; 
            struct pNode* exprnode = p->childs[2];
            int lhstype = type_inference(lhsnode, global_tb, local_tbstk);
            int lhsdepth = get_node_depth(lhsnode);
            int exprtype = type_synthesis(exprnode, global_tb, local_tbstk);
            int truthlist[] = {VALUETYPE_INT, VALUETYPE_TUPLE};
            check_type_in_list(exprtype, truthlist, 2); // only {int,tuple} are allowed assign, {array, func, unknown} forbidden
            if (lhsdepth > 1) {
                fprintf(stderr,RED"todo, multiple assign statement, not implemented yet\n"RESET); //TODO
                exit(999);
            }
            // lhs has depth 1, if lhs is unknown type, update type. otherwise check equal.
            if (lhstype == VALUETYPE_UNKNOWN) {
                struct pNode* lhsitem = p->childs[0];
                struct pNode* singleid_as_lhsitem = lhsitem->childs[0];
                struct sNode* snode = (struct sNode*) singleid_as_lhsitem->childs[0];
                struct symboltable* matched_tb = get_matched_symboltable(snode->sval, global_tb, local_tbstk);
                if (matched_tb) {
                    set_symbol_type(snode->sval, exprtype, matched_tb->scope, snode->line, matched_tb);
                } else{
                    fprintf(stderr, RED"[treewalker] assign unknown value failed. in line %d"RESET, p->line);
                    exit(999);
                }
            } else {
                check_type_equal(lhstype, exprtype);
            }
            break;
        }
        case NODETYPE_LHS_EXCHANGE_LHS_AS_STATEMENT:{
            //left and right should have the same type, and same depth
            struct pNode* left_lhsnode = p->childs[0]; 
            struct pNode* right_lhsnode = p->childs[2];
            int left_lhstype = type_inference(left_lhsnode, global_tb, local_tbstk);
            int right_lhstype = type_inference(right_lhsnode, global_tb, local_tbstk);
            check_type_equal(left_lhstype, right_lhstype);
            if(get_node_depth(left_lhsnode)!=get_node_depth(right_lhsnode)){
                fprintf(stderr, RED"[treewalker] exchange failed. unequal length. in line %d"RESET, p->line);
                exit(999);
            }
            break;
        }
        case NODETYPE_FOREACH_RANGE_STATEMENT:{
            // the temp iterator id is treated as a temprary global id, will remove it after this statement
            struct sNode* snode = (struct sNode*) p->childs[1]->childs[0];
            struct pNode* rangenode = p->childs[3];
            struct pNode* endfornode = p->childs[7];
            declare_symbol(snode->sval, VALUETYPE_INT, GLOBAL_SCOPE, snode->line, global_tb);
            check_type_equal(type_synthesis(rangenode->childs[0], global_tb, local_tbstk), VALUETYPE_INT);
            check_type_equal(type_synthesis(rangenode->childs[2], global_tb, local_tbstk), VALUETYPE_INT);
            for (int i = 4; i < p->childscount; i++)
                treewalker(p->childs[i], global_tb, local_tbstk);
            remove_symbol(snode->sval, GLOBAL_SCOPE, endfornode->line, global_tb);
            break;
        }
        case NODETYPE_FOREACH_ARRAYID_STATEMENT:{
            // the temp iterator id is treated as a temprary global id, will remove it after this statement
            struct sNode* snode = (struct sNode*) p->childs[1]->childs[0];
            struct pNode* arrayidnode = p->childs[3];
            struct sNode* arraynamenode = (struct sNode*) arrayidnode->childs[0];
            struct pNode* endfornode = p->childs[7];
            declare_symbol(snode->sval, VALUETYPE_INT, GLOBAL_SCOPE, snode->line, global_tb);
            check_type_equal(type_lookup(arraynamenode->sval, global_tb, local_tbstk), VALUETYPE_ARRAY);
            for (int i = 4; i < p->childscount; i++)
                treewalker(p->childs[i], global_tb, local_tbstk);
            remove_symbol(snode->sval, GLOBAL_SCOPE, endfornode->line, global_tb);
            break;
        }
        case NODETYPE_PRINT_STATEMENT:{
            // when print a expr, the type of expr should be known and in {arrat, tuple, int}
            struct pNode* exprnode = p->childs[1];
            int valuetype = type_synthesis(exprnode, global_tb, local_tbstk);
            int truthlist[] = {VALUETYPE_ARRAY, VALUETYPE_INT, VALUETYPE_TUPLE};
            check_type_in_list(valuetype, truthlist, 3);
            break;
        }
        case NODETYPE_RETURN_STATEMENT:{
            // when return a expr, the type of expr should be known and in {tuple, int}
            // must be in a local enviroment
            if (LOCAL_ENV){
                struct pNode* exprnode = p->childs[1];
                int valuetype = type_synthesis(exprnode, global_tb, local_tbstk);
                int truthlist[] = {VALUETYPE_INT, VALUETYPE_TUPLE};
                check_type_in_list(valuetype, truthlist, 2);
                pop_symboltableStack(local_tbstk);
            } else {
                fprintf(stderr,RED"can't use return statementout side local enviroment.\n"RESET);
                exit(999);
            }
            break;
        }
        case NODETYPE_NO_EXPR_LOCAL_DECL:{
            // only declare local id with no expr in local env
            if (check_current_scope(global_tb, local_tbstk, LOCAL_SCOPE) == 0){
                fprintf(stderr, RED"[treewalker] can't declare local id with no expr outside local env. in line %d"RESET, p->line);
                exit(999);
            }
            struct symboltable* local_tb = top_symboltableStack(local_tbstk);
            struct sNode * snode = (struct sNode *)(p->childs[1]); 
            declare_symbol(snode->sval, VALUETYPE_UNKNOWN, LOCAL_SCOPE, snode->line, local_tb);
            break;
        }
        case NODETYPE_LOCAL_DECL:{
            // only declare local id in local env
            if (check_current_scope(global_tb, local_tbstk, LOCAL_SCOPE) == 0){
                fprintf(stderr, RED"[treewalker] can't declare local id with expr outside local env. in line %d"RESET, p->line);
                exit(999);
            }
            struct symboltable* local_tb = top_symboltableStack(local_tbstk);
            struct sNode * snode = (struct sNode *)(p->childs[1]); 
            struct pNode * exprnode = p->childs[3]; 
            int valuetype = type_synthesis(exprnode, global_tb, local_tbstk);
            int truthlist[] = {VALUETYPE_INT, VALUETYPE_TUPLE};
            check_type_in_list(valuetype, truthlist, 2);
            declare_symbol(snode->sval, valuetype, LOCAL_SCOPE, snode->line,local_tb);
            break;
        }
        case NODETYPE_NO_EXPR_GLOBAL_DECL:{
            // declare global id with no expr in any env
            // if in local, then declare a local `link` and check global variable has a 'known' type
            // if in global, then declare a global `unknown` symbol
            struct sNode * snode = (struct sNode *)(p->childs[1]);
            if (check_current_scope(global_tb, local_tbstk, GLOBAL_SCOPE)){ 
                declare_symbol(snode->sval, VALUETYPE_UNKNOWN, GLOBAL_SCOPE, snode->line, global_tb);
            } else if (check_current_scope(global_tb, local_tbstk, LOCAL_SCOPE)){
                struct symboltableRecord* record = lookup_symbol(snode->sval, GLOBAL_SCOPE, global_tb);
                if (record != NULL) {
                    int truthlist[] = {VALUETYPE_ARRAY, VALUETYPE_INT, VALUETYPE_TUPLE}; 
                    check_type_in_list(record->valuetype, truthlist, 3);
                } else {
                    fprintf(stderr, RED"[treewalker] can't access unknown global id in local env. in line %d"RESET, p->line);
                    exit(999);
                }
            }
            break;
        }
        case NODETYPE_GLOBAL_DECL:{
            // only declare global id with expr in global env
            if (check_current_scope(global_tb, local_tbstk, GLOBAL_SCOPE) == 0){
                fprintf(stderr, RED"[treewalker] can't declare global id with expr outside global env. in line %d"RESET, p->line);
                exit(999);
            }
            struct sNode * snode = (struct sNode *)(p->childs[1]); 
            struct pNode * exprnode = p->childs[3]; 
            int valuetype = type_synthesis(exprnode, global_tb, local_tbstk);
            int truthlist[] = {VALUETYPE_INT, VALUETYPE_TUPLE};
            check_type_in_list(valuetype, truthlist, 2);
            declare_symbol(snode->sval, valuetype, GLOBAL_SCOPE, snode->line, global_tb);
            break;
        }
        case NODETYPE_ARRAY_DECL:
        case NODETYPE_ARRAY_DECL_WITH_ANONY_FUNC:{
            //KW_ARRAY ID OP_LBRAK expr OP_DOTDOT expr OP_RBRAK OP_SEMI 
            struct sNode * snode = (struct sNode *)(p->childs[1]); 
            struct pNode * exprnode_beg = p->childs[3];
            struct pNode * exprnode_end = p->childs[5];
            check_type_equal(type_lookup(snode->sval, global_tb, local_tbstk), VALUETYPE_UNKNOWN);
            check_type_equal(type_synthesis(exprnode_beg, global_tb, local_tbstk), VALUETYPE_INT);
            check_type_equal(type_synthesis(exprnode_end, global_tb, local_tbstk), VALUETYPE_INT);
            struct symboltable* matched_tb = get_matched_symboltable(snode->sval, global_tb, local_tbstk);
            set_symbol_type(snode->sval, VALUETYPE_ARRAY, matched_tb->scope, snode->line, matched_tb);
            break;
        }
        case NODETYPE_BOOLEXPR:{
            //expr bool_op expr
            // only allow int in bool expr compare
            struct pNode * exprnode_beg = p->childs[0];
            struct pNode * exprnode_end = p->childs[2];
            check_type_equal(type_synthesis(exprnode_beg, global_tb, local_tbstk), VALUETYPE_INT);
            check_type_equal(type_synthesis(exprnode_end, global_tb, local_tbstk), VALUETYPE_INT);
            break;
        }
        default: printf("visit unsupported node type for treewalker: %d\n", p->pnodetype);
    }
}
