/*
* Main Functions for the boris compiler
*
* including parse-tree-builder, parse-tree-walker, type-checker. 
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
    parent->true_block = NULL;
    parent->false_block = NULL;
    parent->next_block = NULL;
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
    if (level == 0) printf("Start parse tree visualization:\n"); 
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
    printf("matched symboltb not found for %s\n", sval);
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

//calculate expr width
int get_expr_width(struct pNode* p, struct symboltable* global_tb, struct symboltableStack* local_tbstk){
    if (p == NULL) return 0;
    switch (p->pnodetype){
        case NODETYPE_EXPR_COMMA_EXPR:
            return get_expr_width(p->childs[0], global_tb, local_tbstk) + get_expr_width(p->childs[2], global_tb, local_tbstk);
        case NODETYPE_SINGLE_INT_AS_EXPR:
        case NODETYPE_TUPLE_REF_AS_EXPR:
        case NODETYPE_ARRAY_REF_AS_EXPR:
        case NODETYPE_EXPR_MINUS_EXPR:
        case NODETYPE_EXPR_PLUS_EXPR:
        case NODETYPE_EXPR_MULT_EXPR:
        case NODETYPE_EXPR_DIV_EXPR:
            return 1;
        case NODETYPE_SINGLE_ID_AS_EXPR:{
            struct sNode * snode = (struct sNode *)(p->childs[0]);
            struct symboltable* matched_tb = get_matched_symboltable(snode->sval, global_tb, local_tbstk);
            struct symboltableRecord* record = lookup_symbol(snode->sval, matched_tb->scope, matched_tb);
            if (record->valuetype == VALUETYPE_LINK_TO_GLOBAL) {
                //replace if it's a link to global table.
                record = lookup_symbol(snode->sval,GLOBAL_SCOPE, global_tb);
            }
            if (record->valuetype == VALUETYPE_INT) {
                return 1;
            } else if (record->valuetype == VALUETYPE_TUPLE) {
                return record->value->ivallistlength;
            }
        } 
        default:
            printf("get_expr_width with bad nodetype %d, tuple creation may get bad size\n",p->pnodetype);
            return 1;
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
           struct sNode * snode = (struct sNode *)(exprnode->childs[0]);
           struct symboltableRecord* record = lookup_symbol(snode->sval, GLOBAL_SCOPE, global_tb);
           if (record == NULL || record->valuetype != VALUETYPE_FUNC){
               fprintf(stderr, RED"type_synthesis meet undefined function `%s`"RESET, snode->sval); //TODO
               exit(999);
           }
           struct symboltableRecordFunction* function = (struct symboltableRecordFunction*)record->value;
           int input_valuetype = type_synthesis(exprnode->childs[1], global_tb, local_tbstk);
           check_type_equal(input_valuetype, function->formal_parameter_valuetype);
           return function->return_valuetype;
           break;
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
            if (exprnode->pnodetype == NODETYPE_PLACEHOLDER) {
                printf("tok:%s\n", ((struct placeholderNode*)exprnode)->tokstr);
            }
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
            int truthlist[] = {VALUETYPE_TUPLE, VALUETYPE_INT, VALUETYPE_UNKNOWN, VALUETYPE_ARRAY};
            check_type_in_list(valuetype, truthlist, 4); 
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

// helper function for `determine_formal_parameter_valuetype`
// return if the sval has been referenced as a tuple in p
int _any_tuple_reference(struct pNode* p, char* sval){
    if(p == NULL){
        return 0;
    }
    switch (p->pnodetype)
    {
        case NODETYPE_TUPLE_REF_AS_EXPR:
        case NODETYPE_TUPLE_REF_AS_LHSITEM:{
            struct sNode * snode = (struct sNode *)(p->childs[0]);
            if(strcmp(snode->sval, sval)==0) return 1;
            break;
        }
        default:{
            for (int i = 0; i < p->childscount; i++){
                if(_any_tuple_reference(p->childs[i], sval)) return 1;
            }
            break;
        }
    }
    return 0;
}

// no side effects, default is int
// if there are any tuple reference of formal_para, then formal_para is a tuple
int determine_formal_parameter_valuetype(struct pNode* p){
    if (p == NULL || p->pnodetype != NODETYPE_FUNC_DEFN){
        fprintf(stderr, RED"[determine_formal_parameter_valuetype] bad input node. in line %d"RESET, p->line);
        exit(999);
    }
    struct sNode * formal_parameter_name = (struct sNode *)(p->childs[3]);
    if (_any_tuple_reference(p, formal_parameter_name->sval)){
        return VALUETYPE_TUPLE;
    }
    return VALUETYPE_INT;
}


// helper function for `determine_return_valuetype`.
// if no return statement, return 0.
// otherwise return the the type expr of first return statement in p.
// should be a tree structure included in non-terminal `body`.
// will only do minimun type checking
int _get_first_return_statement_type(struct pNode* p, struct symboltable* global_tb,  struct symboltableStack* local_tbstk){
    if (p == NULL) return 0;
    if (check_current_scope(global_tb, local_tbstk, LOCAL_SCOPE) == 0){
        fprintf(stderr, RED"[defn node error] in line %d"RESET, p->line);
        exit(999);
    }
    // now the following is in a local env
    // do declarations in local_tb only
    // global_tb is treated as read only
    switch (p->pnodetype)
    {   
        //return nodes
        case NODETYPE_RETURN_STATEMENT:{
            struct pNode* exprnode = p->childs[1];
            return type_synthesis(exprnode, global_tb, local_tbstk);
            break;
        }
        //decl nodes
        case NODETYPE_NO_EXPR_LOCAL_DECL:{
            struct symboltable* local_tb = top_symboltableStack(local_tbstk);
            struct sNode * snode = (struct sNode *)(p->childs[1]); 
            declare_symbol(snode->sval, VALUETYPE_UNKNOWN, LOCAL_SCOPE, snode->line, local_tb);
            break;
        }
        case NODETYPE_LOCAL_DECL:{
            // only declare local id in local env
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
            struct sNode * snode = (struct sNode *)(p->childs[1]);
            struct symboltableRecord* record = lookup_symbol(snode->sval, GLOBAL_SCOPE, global_tb);
            struct symboltable* local_tb = top_symboltableStack(local_tbstk);
            if (record != NULL) {
                int truthlist[] = {VALUETYPE_ARRAY, VALUETYPE_INT, VALUETYPE_TUPLE}; 
                check_type_in_list(record->valuetype, truthlist, 3);
                declare_symbol(snode->sval, VALUETYPE_LINK_TO_GLOBAL, LOCAL_SCOPE, snode->line, local_tb);
            } else {
                fprintf(stderr, RED"[defn node error] can't access unknown global id in local env. in line %d"RESET, p->line);
                exit(999);
            }
            break;
        }
        case NODETYPE_GLOBAL_DECL:{
            fprintf(stderr, RED"[defn node error] global decl with expr in defn is not allowed. in line %d"RESET, p->line);
            exit(999);
        }
        case NODETYPE_ARRAY_DECL:
        case NODETYPE_ARRAY_DECL_WITH_ANONY_FUNC:{
            //KW_ARRAY ID OP_LBRAK expr OP_DOTDOT expr OP_RBRAK OP_SEMI 
            struct sNode * snode = (struct sNode *)(p->childs[1]); 
            struct symboltable* local_tb = top_symboltableStack(local_tbstk);
            struct symboltableRecord* record = lookup_symbol(snode->sval, LOCAL_SCOPE, local_tb);
            if (record != NULL) {
                check_type_equal(record->valuetype, VALUETYPE_UNKNOWN);
                declare_symbol(snode->sval, VALUETYPE_ARRAY, LOCAL_SCOPE, snode->line, local_tb);
            } else {
                fprintf(stderr, RED"[defn node error] array set error, id not found in local env%d"RESET, p->line);
                exit(999);
            }
            break;
        }
        //lhs assign node
        case NODETYPE_LHS_ASSIGN_EXPR_AS_STATEMENT:{
            struct pNode* lhsnode = p->childs[0]; 
            struct pNode* exprnode = p->childs[2];
            struct symboltable* local_tb = top_symboltableStack(local_tbstk);
            int lhsdepth = get_node_depth(lhsnode);
            if (lhsdepth > 1) {
                fprintf(stderr,RED"todo, multiple assign statement, not implemented yet\n"RESET); //TODO
                exit(999);
            }
            struct sNode * snode = (struct sNode *)(lhsnode->childs[0]->childs[0]); 
            struct symboltableRecord* record = lookup_symbol(snode->sval, LOCAL_SCOPE, local_tb);
            if (record != NULL) {
                check_type_equal(record->valuetype, VALUETYPE_UNKNOWN);
                int exprtype = type_synthesis(exprnode, global_tb, local_tbstk);
                set_symbol_type(snode->sval, exprtype, LOCAL_SCOPE, snode->line, local_tb);
            } else{
                fprintf(stderr,RED"global id `%s` are read-only inside a function\n"RESET, snode->sval); //TODO
                print_symboltable(local_tb);
                exit(999);
            }
            break;
        }
        default:
        {
            for (int i = 0; i < p->childscount; i++){
                int first_return = _get_first_return_statement_type(p->childs[i], global_tb, local_tbstk);
                if (first_return) return first_return;
            }
            break;
        }
    }
    return 0;   
}

// no side effects, default is void
// make a simulated run to checkout the type.
// will use the expr of first return statement to determine type
int determine_return_valuetype(struct pNode* p, int formal_paramter_valuetype, struct symboltable* global_tb){
    if (p == NULL || p->pnodetype != NODETYPE_FUNC_DEFN){
        fprintf(stderr, RED"[determine_return_valuetype] bad input node. in line %d"RESET, p->line);
        exit(999);
    }
    // set up a simulated local symboltable
    struct symboltableStack* simualated_local_tbstk = init_symboltableStack(MAX_SYMBOLTABLE_STACK_SIZE);
    struct symboltable* tb = init_symboltable(MAX_SYMBOLTABLE_SIZE, LOCAL_SCOPE);
    push_symboltableStack(tb, simualated_local_tbstk);
    // declare formal_parameter in simulated local symboltable 
    struct sNode * formal_parameter_name = (struct sNode *)(p->childs[3]);
    declare_symbol(formal_parameter_name->sval, formal_paramter_valuetype, LOCAL_SCOPE, formal_parameter_name->line, top_symboltableStack(simualated_local_tbstk));
    // determine the return type here
    int return_type = _get_first_return_statement_type(p, global_tb, simualated_local_tbstk);
    // clean up
    pop_symboltableStack(simualated_local_tbstk);
    remove_symboltableStack(simualated_local_tbstk);
    if (return_type == 0){
        fprintf(stderr, RED"[determine_return_valuetype] a function must have at least one return statement. in line %d"RESET, p->line);
        exit(999);
    }
    return return_type;
}

// from a exprnode (EXPR_COMMA_EXPR), read all the values into results.
void _dfs_read_values(struct pNode* exprnode, LLVMValueRef* results, int* index, struct symboltable* global_tb, struct symboltableStack* local_tbstk, LLVMBuilderRef builder, LLVMModuleRef module){
    if (exprnode->pnodetype == NODETYPE_EXPR_COMMA_EXPR) {
        _dfs_read_values(exprnode->childs[0], results, index, global_tb, local_tbstk, builder, module);
        _dfs_read_values(exprnode->childs[2], results, index, global_tb, local_tbstk, builder, module);
    } else if (exprnode->pnodetype == NODETYPE_SINGLE_INT_AS_EXPR) {
        struct iNode* inode  = (struct iNode*)exprnode->childs[0];
        results[(*index)++] = LLVMConstInt(LLVMInt32Type(), inode->ival, 0);
    } else if (exprnode->pnodetype == NODETYPE_SINGLE_ID_AS_EXPR) {
        struct sNode* snode = (struct sNode*)exprnode->childs[0];
        struct symboltable* matched_tb = get_matched_symboltable(snode->sval, global_tb, local_tbstk);
        struct symboltableRecord* record = lookup_symbol(snode->sval, matched_tb->scope, matched_tb);
        if (record->valuetype == VALUETYPE_INT) {
            results[(*index)++] = boris_codegen_expr(exprnode, builder, module, global_tb, local_tbstk);
        } else if (record->valuetype == VALUETYPE_TUPLE) {
            int width = record->value->ivallistlength;
            LLVMValueRef tuple_address = record->value->address;
            for (int i = 0; i < width; i++) {
                LLVMValueRef offset = LLVMConstInt(LLVMInt32Type(), i, 0);
                LLVMValueRef indices[] = { offset };
                LLVMValueRef element_address = LLVMBuildGEP(builder, tuple_address, indices, 1, "");
                results[(*index)++] = LLVMBuildLoad(builder, element_address, "");
            }    
        }
    }
}



void read_all_values_in_expr(struct pNode* exprnode, LLVMValueRef* results, struct symboltable* global_tb, struct symboltableStack* local_tbstk, LLVMBuilderRef builder, LLVMModuleRef module){
    int index = 0;
    _dfs_read_values(exprnode, results, &index, global_tb, local_tbstk, builder, module);
}

//populate the results into a array address
void populate_into_address(LLVMValueRef results[], int width, LLVMValueRef address, LLVMBuilderRef builder){
    for (int i = 0; i < width; i++){
        LLVMValueRef offset = LLVMConstInt(LLVMInt32Type(), i, 0); // array access should consider its start point
        LLVMValueRef indices[] = { offset };
        LLVMValueRef element_address = LLVMBuildGEP(builder, address, indices, 1, "");
        LLVMValueRef expr_code = results[i];
        LLVMBuildStore(builder, expr_code, element_address);
    }
}

//global_temp_string_count
//used in treewalker, to generate a temp string
int global_temp_string_count = 0; 
void get_temp_string(char* temp_string){
    sprintf(temp_string, "_temp_id_%d", global_temp_string_count++);
}

void treewalker(struct pNode* p, struct symboltable* global_tb, struct symboltableStack* local_tbstk,  \
                                                        LLVMBuilderRef builder, LLVMModuleRef module){
    if (p == NULL) return;
    switch(p->pnodetype){
        case NODETYPE_ROOT_INPUT:{
            printf("> Start walking on parse tree:\n");
            treewalker(p->childs[0], global_tb, local_tbstk, builder, module);
            break;
        }
        //nodes in `input``
        case NODETYPE_STATEMENT_LIST:                                                  //1046
        case NODETYPE_STATEMENT_AS_SDD:                                                //1069
        case NODETYPE_DECL_AS_SDD:                                                     //1070
        case NODETYPE_DEFN_AS_SDD:                                                     //1071
        case NODETYPE_SDD_LIST:                                                        //1072
        //leaf - keyword place holder
        case NODETYPE_PLACEHOLDER:                                                     //1026
        //#function calls
        case NODETYPE_STATEMENT_AS_SD:                                                 //1063
        case NODETYPE_DECL_AS_SD:                                                      //1064
        case NODETYPE_SD_LIST:                                                         //1065
        case NODETYPE_BODY:                                                            //1066
        {
            for (int i = 0; i < p->childscount; i++)
                treewalker(p->childs[i], global_tb, local_tbstk, builder, module);
            break;
        }
        case NODETYPE_WHILE_STATEMENT: {                                                //1047
            struct pNode* boolnode = p->childs[1];
            struct pNode* stmtsnode = p->childs[3];
            LLVMValueRef MainFunction = LLVMGetNamedFunction(module, "main");
            LLVMBasicBlockRef InitBasicBlock = LLVMAppendBasicBlock(MainFunction, "while_init_");
            LLVMBasicBlockRef CheckBasicBlock = LLVMAppendBasicBlock(MainFunction, "while_check_");
            LLVMBasicBlockRef BodyBasicBlock = LLVMAppendBasicBlock(MainFunction, "while_body_");
            LLVMBasicBlockRef EndBasicBlock = LLVMAppendBasicBlock(MainFunction, "while_end_");
            LLVMBuildBr(builder, InitBasicBlock);
            LLVMPositionBuilderAtEnd(builder, InitBasicBlock);
            LLVMBuildBr(builder, CheckBasicBlock); // build unconditional jump to check block.
            // build bool code in CheckBasicBlock
            LLVMPositionBuilderAtEnd(builder, CheckBasicBlock);
            boolnode->true_block = BodyBasicBlock;
            boolnode->false_block = EndBasicBlock;
            treewalker(boolnode, global_tb, local_tbstk, builder, module);
            // build statement_list
            LLVMPositionBuilderAtEnd(builder, BodyBasicBlock);
            treewalker(stmtsnode, global_tb, local_tbstk, builder, module);
            LLVMBuildBr(builder, CheckBasicBlock); // build unconditional jump to check block.
            // put builder at end
            LLVMPositionBuilderAtEnd(builder, EndBasicBlock);
            break;
        }
        case NODETYPE_IF_STATEMENT: {                                                   //1051
            struct pNode* boolnode = p->childs[1];
            struct pNode* stmtsnode = p->childs[3];
            struct pNode* elslistsnode = p->childs[4];
            LLVMValueRef MainFunction = LLVMGetNamedFunction(module, "main");
            LLVMBasicBlockRef InitBasicBlock = LLVMAppendBasicBlock(MainFunction, "if_init_");
            LLVMBasicBlockRef CheckBasicBlock = LLVMAppendBasicBlock(MainFunction, "if_check_");
            LLVMBasicBlockRef ConditionOneBasicBlock = LLVMAppendBasicBlock(MainFunction, "if_condition_one_");
            LLVMBasicBlockRef ConditionTwoBasicBlock = LLVMAppendBasicBlock(MainFunction, "if_condition_two_");
            LLVMBasicBlockRef NextBasicBlock = LLVMAppendBasicBlock(MainFunction, "if_next_");
            if (elslistsnode != NULL) elslistsnode->next_block = NextBasicBlock;
            LLVMBuildBr(builder, InitBasicBlock);
            LLVMPositionBuilderAtEnd(builder, InitBasicBlock);
            LLVMBuildBr(builder, CheckBasicBlock); // build unconditional jump to check block.
            // build bool code in CheckBasicBlock
            LLVMPositionBuilderAtEnd(builder, CheckBasicBlock);
            boolnode->true_block = ConditionOneBasicBlock;
            boolnode->false_block = ConditionTwoBasicBlock;
            treewalker(boolnode, global_tb, local_tbstk, builder, module);
            // build statement_list in condition_one
            LLVMPositionBuilderAtEnd(builder, ConditionOneBasicBlock);
            treewalker(stmtsnode, global_tb, local_tbstk, builder, module);
            LLVMBuildBr(builder, NextBasicBlock); 
            // build elsif_sentence_list in condition_two
            LLVMPositionBuilderAtEnd(builder, ConditionTwoBasicBlock);
            treewalker(elslistsnode, global_tb, local_tbstk, builder, module);
            // put builder at end
            LLVMBuildBr(builder, NextBasicBlock);
            LLVMPositionBuilderAtEnd(builder, NextBasicBlock);
            break;
        }
        case NODETYPE_IF_ELSE_STATEMENT: {                                              //1052
            //KW_IF bool_expr KW_THEN statement_list elsif_sentence_list else_sentence KW_END KW_IF
            struct pNode* boolnode = p->childs[1];
            struct pNode* stmtsnode = p->childs[3];
            struct pNode* elslistsnode = p->childs[4];
            struct pNode* elsenode = p->childs[5];
            LLVMValueRef MainFunction = LLVMGetNamedFunction(module, "main");
            LLVMBasicBlockRef InitBasicBlock = LLVMAppendBasicBlock(MainFunction, "if_init_");
            LLVMBasicBlockRef CheckBasicBlock = LLVMAppendBasicBlock(MainFunction, "if_check_");
            LLVMBasicBlockRef ConditionOneBasicBlock = LLVMAppendBasicBlock(MainFunction, "if_condition_one_");
            LLVMBasicBlockRef ConditionTwoBasicBlock = LLVMAppendBasicBlock(MainFunction, "if_condition_two_");
            LLVMBasicBlockRef NextBasicBlock = LLVMAppendBasicBlock(MainFunction, "if_next_");
            if (elslistsnode != NULL) elslistsnode->next_block = NextBasicBlock;
            LLVMBuildBr(builder, InitBasicBlock);
            LLVMPositionBuilderAtEnd(builder, InitBasicBlock);
            LLVMBuildBr(builder, CheckBasicBlock); // build unconditional jump to check block.
            // build bool code in CheckBasicBlock
            LLVMPositionBuilderAtEnd(builder, CheckBasicBlock);
            boolnode->true_block = ConditionOneBasicBlock;
            boolnode->false_block = ConditionTwoBasicBlock;
            treewalker(boolnode, global_tb, local_tbstk, builder, module);
            // build statement_list in condition_one
            LLVMPositionBuilderAtEnd(builder, ConditionOneBasicBlock);
            treewalker(stmtsnode, global_tb, local_tbstk, builder, module);
            LLVMBuildBr(builder, NextBasicBlock); 
            // build elsif_sentence_list in condition_two
            LLVMPositionBuilderAtEnd(builder, ConditionTwoBasicBlock);
            treewalker(elslistsnode, global_tb, local_tbstk, builder, module);
            // build else_sentence (builder now should in a condition two block)
            treewalker(elsenode, global_tb, local_tbstk, builder, module);
            // put builder at end
            LLVMBuildBr(builder, NextBasicBlock);
            LLVMPositionBuilderAtEnd(builder, NextBasicBlock);
            break;
        }
        case NODETYPE_ELSE_SENTENCE:    {                                               //1049
            struct pNode* stmtsnode = p->childs[1];
            treewalker(stmtsnode, global_tb, local_tbstk, builder, module);
            break;
        }
        case NODETYPE_ELSIF_SENTENCE_LIST: {                                            //1050
            for (int i = 0; i < p->childscount; i++){
                if (p->childs[i] != NULL) {
                    p->childs[i]->next_block = p->next_block;
                    treewalker(p->childs[i], global_tb, local_tbstk, builder, module);
                }
            }
            break;
        }
        case NODETYPE_ELSIF_SENTENCE: {                                                 //1048
            // KW_ELSIF bool_expr KW_THEN statement_list
            struct pNode* boolnode = p->childs[1];
            struct pNode* stmtsnode = p->childs[3];
            LLVMValueRef MainFunction = LLVMGetNamedFunction(module, "main");
            LLVMBasicBlockRef InitBasicBlock = LLVMAppendBasicBlock(MainFunction, "else_init_");
            LLVMBasicBlockRef CheckBasicBlock = LLVMAppendBasicBlock(MainFunction, "else_check_");
            LLVMBasicBlockRef ConditionOneBasicBlock = LLVMAppendBasicBlock(MainFunction, "else_condition_one_");
            LLVMBasicBlockRef ConditionTwoBasicBlock = LLVMAppendBasicBlock(MainFunction, "else_condition_two_");
            LLVMBuildBr(builder, InitBasicBlock);
            LLVMPositionBuilderAtEnd(builder, InitBasicBlock);
            LLVMBuildBr(builder, CheckBasicBlock); // build unconditional jump to check block.
            // build bool code in CheckBasicBlock
            LLVMPositionBuilderAtEnd(builder, CheckBasicBlock);
            boolnode->true_block = ConditionOneBasicBlock;
            boolnode->false_block = ConditionTwoBasicBlock;
            treewalker(boolnode, global_tb, local_tbstk, builder, module);
            // build statement_list in condition_one
            LLVMPositionBuilderAtEnd(builder, ConditionOneBasicBlock);
            treewalker(stmtsnode, global_tb, local_tbstk, builder, module);
            LLVMBuildBr(builder, p->next_block);
            // put builder to condition_two
            LLVMPositionBuilderAtEnd(builder, ConditionTwoBasicBlock);
            break;
        }

        case NODETYPE_LHS_ASSIGN_EXPR_AS_STATEMENT: {                                   //1044
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
            if (lhsnode->childs[0]->pnodetype == NODETYPE_SINGLE_ID_AS_LHSITEM) {
                struct pNode* singleid_as_lhsitem = lhsnode->childs[0];
                struct sNode* snode = (struct sNode*) singleid_as_lhsitem->childs[0];
                if (lhstype == VALUETYPE_UNKNOWN) {
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
                //update value in lhs address
                struct symboltable* matched_tb = get_matched_symboltable(snode->sval, global_tb, local_tbstk);
                struct symboltableRecord* record = lookup_symbol(snode->sval, matched_tb->scope, matched_tb);
                if (record->valuetype == VALUETYPE_INT) {
                    // int assign
                    if (record->value == NULL){ 
                        init_int_symbol(snode->sval, matched_tb->scope, snode->line, matched_tb);
                        update_int_symbol(snode->sval, matched_tb->scope, 1, snode->line, matched_tb);
                        record->value->address= LLVMBuildAlloca(builder, LLVMInt32Type(), snode->sval);
                    }
                    LLVMValueRef expr_code = boris_codegen_expr(p->childs[2], builder, module, global_tb, local_tbstk);
                    LLVMValueRef lhs = record->value->address;
                    LLVMBuildStore(builder, expr_code, lhs);
                } else {
                    // tuple assign
                    int width = get_expr_width(exprnode, global_tb, local_tbstk);
                    if (record->value == NULL){ 
                        init_int_list_symbol(snode->sval, GLOBAL_SCOPE, 0, width, snode->line, global_tb);
                    } else {
                        //update the expected length of tuple
                        record->value->ivallistlength = width;
                    }
                    // allocate tuple
                    LLVMValueRef tuple_address =  LLVMBuildArrayAlloca(builder, LLVMInt32Type(), LLVMConstInt(LLVMInt32Type(), width, 0), snode->sval);
                    record->value->address = tuple_address;
                    // populate tuple
                    LLVMValueRef results[width];
                    read_all_values_in_expr(exprnode, results, global_tb, local_tbstk, builder, module);
                    populate_into_address(results, width, tuple_address, builder);
                }
            } else if (lhsnode->childs[0]->pnodetype == NODETYPE_ARRAY_REF_AS_LHSITEM){
                struct pNode* arrayref_as_lhsitem = lhsnode->childs[0];
                struct sNode* snode = (struct sNode*) arrayref_as_lhsitem->childs[0];
                struct pNode* exprnode = arrayref_as_lhsitem->childs[2];
                struct symboltable* matched_tb = get_matched_symboltable(snode->sval, global_tb, local_tbstk);
                struct symboltableRecord* record = lookup_symbol(snode->sval, matched_tb->scope, matched_tb);
                LLVMValueRef array_address = record->value->address;
                LLVMValueRef offset = LLVMBuildSub(builder, boris_codegen_expr(exprnode, builder, module, global_tb, local_tbstk), LLVMConstInt(LLVMInt32Type(), record->value->ivallist_start, 0), ""); // array access should consider its start point
                LLVMValueRef indices[] = { offset };
                LLVMValueRef element_address = LLVMBuildGEP(builder, array_address, indices, 1, "");
                LLVMValueRef expr_code = boris_codegen_expr(p->childs[2], builder, module, global_tb, local_tbstk);
                LLVMBuildStore(builder, expr_code, element_address);
            } else if (lhsnode->childs[0]->pnodetype == NODETYPE_TUPLE_REF_AS_LHSITEM){
                struct pNode* tupleref_as_lhsitem = lhsnode->childs[0];
                struct sNode* snode = (struct sNode*) tupleref_as_lhsitem->childs[0];
                struct pNode* exprnode = tupleref_as_lhsitem->childs[2];
                struct symboltable* matched_tb = get_matched_symboltable(snode->sval, global_tb, local_tbstk);
                struct symboltableRecord* record = lookup_symbol(snode->sval, matched_tb->scope, matched_tb);
                LLVMValueRef tuple_address = record->value->address;
                LLVMValueRef offset = boris_codegen_expr(exprnode, builder, module, global_tb, local_tbstk);
                LLVMValueRef indices[] = { offset };
                LLVMValueRef element_address = LLVMBuildGEP(builder, tuple_address, indices, 1, "");
                LLVMValueRef expr_code = boris_codegen_expr(p->childs[2], builder, module, global_tb, local_tbstk);
                LLVMBuildStore(builder, expr_code, element_address);
            }else {
                printf("lhs type, no implementation\n");
                exit(666);
            }
                        
            break;
        }
        case NODETYPE_LHS_EXCHANGE_LHS_AS_STATEMENT: {                                  //1045
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
            if(get_node_depth(left_lhsnode) != 1){
                fprintf(stderr, RED"[treewalker] exchange failed. only support length 1 %d"RESET, p->line);
                exit(999);
            }
            struct pNode* left_lhsitem = left_lhsnode->childs[0];
            struct pNode* right_lhsitem = right_lhsnode->childs[0];
            if(left_lhsitem->pnodetype != NODETYPE_SINGLE_ID_AS_LHSITEM || right_lhsitem->pnodetype != NODETYPE_SINGLE_ID_AS_LHSITEM){
                fprintf(stderr, RED"[treewalker] exchange failed. only support single id %d"RESET, p->line);
                exit(999);
            }
            struct sNode* left_snode = (struct sNode*)left_lhsitem->childs[0];
            struct sNode* right_snode = (struct sNode*)right_lhsitem->childs[0];
            struct symboltable* left_matched_tb = get_matched_symboltable(left_snode->sval, global_tb, local_tbstk);
            struct symboltableRecord* left_record = lookup_symbol(left_snode->sval, left_matched_tb->scope, left_matched_tb);
            struct symboltable* right_matched_tb = get_matched_symboltable(right_snode->sval, global_tb, local_tbstk);
            struct symboltableRecord* right_record = lookup_symbol(right_snode->sval, right_matched_tb->scope, right_matched_tb);
            if(left_record->valuetype == VALUETYPE_INT) {
                LLVMValueRef temp = left_record->value->address;
                left_record->value->address = right_record->value->address;
                right_record->value->address = temp;
            } else if (left_record->valuetype == VALUETYPE_TUPLE) {
                LLVMValueRef temp = left_record->value->address;
                left_record->value->address = right_record->value->address;
                right_record->value->address = temp;
                int templength = left_record->value->ivallistlength;
                left_record->value->ivallistlength = right_record->value->ivallistlength;
                right_record->value->ivallistlength = templength;
            } else if (left_record->valuetype == VALUETYPE_ARRAY) {
                LLVMValueRef temp = left_record->value->address;
                left_record->value->address = right_record->value->address;
                right_record->value->address = temp;
                int templength = left_record->value->ivallistlength;
                left_record->value->ivallistlength = right_record->value->ivallistlength;
                right_record->value->ivallistlength = templength;
                int tempstart = left_record->value->ivallist_start;
                left_record->value->ivallist_start = right_record->value->ivallist_start;
                right_record->value->ivallist_start = tempstart;
            }
            break;
        }
        case NODETYPE_FOREACH_RANGE_STATEMENT: {                                        //1053
            // the temp iterator id is treated as a temprary global id, will remove it after this statement
            struct sNode* snode = (struct sNode*) p->childs[1]->childs[0];
            struct pNode* rangenode = p->childs[3];
            struct pNode* endfornode = p->childs[7];
            struct pNode* stmtsnode = p->childs[5];
            struct symboltableRecord* record_item = declare_symbol(snode->sval, VALUETYPE_INT, GLOBAL_SCOPE, snode->line, global_tb);
            check_type_equal(type_synthesis(rangenode->childs[0], global_tb, local_tbstk), VALUETYPE_INT);
            check_type_equal(type_synthesis(rangenode->childs[2], global_tb, local_tbstk), VALUETYPE_INT);
            // implemented like a while loop
            LLVMValueRef MainFunction = LLVMGetNamedFunction(module, "main");
            LLVMBasicBlockRef InitBasicBlock = LLVMAppendBasicBlock(MainFunction, "foreachr_init_");
            LLVMBasicBlockRef CheckBasicBlock = LLVMAppendBasicBlock(MainFunction, "foreachr_check_");
            LLVMBasicBlockRef BodyBasicBlock = LLVMAppendBasicBlock(MainFunction, "foreachr_body_");
            LLVMBasicBlockRef EndBasicBlock = LLVMAppendBasicBlock(MainFunction, "foreachr_end_");
            
            LLVMBuildBr(builder, InitBasicBlock);
            LLVMPositionBuilderAtEnd(builder, InitBasicBlock);
            // assign beg_value to temp
            init_int_symbol(snode->sval, GLOBAL_SCOPE, snode->line, global_tb);
            LLVMValueRef item_address = LLVMBuildAlloca(builder, LLVMInt32Type(), snode->sval);
            record_item->value->address = item_address;
            LLVMValueRef beg_value = boris_codegen_expr(rangenode->childs[0], builder, module, global_tb, local_tbstk);
            LLVMValueRef end_value = boris_codegen_expr(rangenode->childs[2], builder, module, global_tb, local_tbstk);
            LLVMBuildStore(builder, beg_value, record_item->value->address);
            LLVMBuildBr(builder, CheckBasicBlock); // build unconditional jump to check block.
            LLVMPositionBuilderAtEnd(builder, CheckBasicBlock);
            // build bool code in CheckBasicBlock
            LLVMIntPredicate prdicate = LLVMIntSLE;
            LLVMValueRef If = LLVMBuildICmp(
                builder,
                prdicate,
                LLVMBuildLoad(builder, item_address, ""),
                end_value,
                ""
            );
            LLVMBuildCondBr(builder, If, BodyBasicBlock, EndBasicBlock);
            // build statement_list
            LLVMPositionBuilderAtEnd(builder, BodyBasicBlock);
            treewalker(stmtsnode, global_tb, local_tbstk, builder, module);
            // item++
            LLVMValueRef One32 = LLVMConstInt(LLVMInt32Type(), 1, 0);
            LLVMValueRef item_plus_plus =  LLVMBuildAdd(builder, LLVMBuildLoad(builder, item_address, ""), One32, "");
            LLVMBuildStore(builder, item_plus_plus, record_item->value->address);
            LLVMBuildBr(builder, CheckBasicBlock);
            LLVMPositionBuilderAtEnd(builder, EndBasicBlock);
             // remove the item
            remove_symbol(snode->sval, GLOBAL_SCOPE, endfornode->line, global_tb);
            break;
        }
        case NODETYPE_FOREACH_ARRAYID_STATEMENT: {                                      //1055
            // the temp iterator id is treated as a temprary global id, will remove it after this statement
            char temp_string[20];
            get_temp_string(temp_string);
            struct sNode* snode = (struct sNode*) p->childs[1]->childs[0];
            struct pNode* arrayidnode = p->childs[3];
            struct sNode* arraynamenode = (struct sNode*) arrayidnode->childs[0];
            struct pNode* endfornode = p->childs[7];
            struct symboltableRecord* record_item = declare_symbol(snode->sval, VALUETYPE_INT, GLOBAL_SCOPE, snode->line, global_tb); //iter_item
            struct symboltableRecord* record_item_index = declare_symbol(temp_string, VALUETYPE_INT, GLOBAL_SCOPE, snode->line, global_tb); //iter_item_index
            struct symboltable* matched_tb = get_matched_symboltable(arraynamenode->sval, global_tb, local_tbstk);
            struct symboltableRecord* record_array = lookup_symbol(arraynamenode->sval, matched_tb->scope, matched_tb);
            check_type_equal(type_lookup(arraynamenode->sval, global_tb, local_tbstk), VALUETYPE_ARRAY);
            // simulate a boolexpr node
            struct pNode* simulated_index = newpNode(NODETYPE_SINGLE_ID_AS_EXPR, 1, newsNode(temp_string));
            struct pNode* simulated_boolop = newplaceholderNode(OP_LESS);
            struct pNode* simulated_length = newpNode(NODETYPE_SINGLE_INT_AS_EXPR, 1, newiNode(record_array->value->ivallistlength));
            struct pNode* simulated_boolexpr = newpNode(NODETYPE_BOOLEXPR, 3, simulated_index, simulated_boolop, simulated_length);
            struct pNode* stmtsnode = p->childs[5];
            
            // implemented like a while loop
            LLVMValueRef MainFunction = LLVMGetNamedFunction(module, "main");
            LLVMBasicBlockRef InitBasicBlock = LLVMAppendBasicBlock(MainFunction, "foreacha_init_");
            LLVMBasicBlockRef CheckBasicBlock = LLVMAppendBasicBlock(MainFunction, "foreacha_check_");
            LLVMBasicBlockRef BodyBasicBlock = LLVMAppendBasicBlock(MainFunction, "foreacha_body_");
            LLVMBasicBlockRef EndBasicBlock = LLVMAppendBasicBlock(MainFunction, "foreacha_end_");
            LLVMBuildBr(builder, InitBasicBlock);
            LLVMPositionBuilderAtEnd(builder, InitBasicBlock);
            // assign 0 to item_index
            LLVMValueRef Zero32 = LLVMConstInt(LLVMInt32Type(), 0, 0);
            LLVMValueRef One32 = LLVMConstInt(LLVMInt32Type(), 1, 0);
            LLVMValueRef item_index_address = LLVMBuildAlloca(builder, LLVMInt32Type(), temp_string);
            LLVMValueRef item_address = LLVMBuildAlloca(builder, LLVMInt32Type(), snode->sval);
            LLVMValueRef array_address = record_array->value->address;
            LLVMBuildStore(builder, Zero32, item_index_address);
            init_int_symbol(snode->sval, GLOBAL_SCOPE, snode->line, global_tb);
            update_int_symbol(snode->sval, GLOBAL_SCOPE, 1, snode->line, global_tb);
            init_int_symbol(temp_string, GLOBAL_SCOPE, snode->line, global_tb);
            update_int_symbol(temp_string, GLOBAL_SCOPE, 1, snode->line, global_tb);
            record_item_index->value->address = item_index_address;
            record_item->value->address = item_address;
            //load array[0] to item
            LLVMValueRef offset = boris_codegen_expr(simulated_index, builder, module, global_tb, local_tbstk);
            LLVMValueRef indices[] = { offset };
            LLVMValueRef element_address = LLVMBuildGEP(builder, array_address, indices, 1, "");
            LLVMBuildStore(builder, LLVMBuildLoad(builder, element_address, ""), record_item->value->address);
            LLVMBuildBr(builder, CheckBasicBlock); // build unconditional jump to check block.
            // build bool code in CheckBasicBlock
            LLVMPositionBuilderAtEnd(builder, CheckBasicBlock);
            simulated_boolexpr->true_block = BodyBasicBlock;
            simulated_boolexpr->false_block = EndBasicBlock;
            treewalker(simulated_boolexpr, global_tb, local_tbstk, builder, module);
            // build statement_list
            LLVMPositionBuilderAtEnd(builder, BodyBasicBlock);
            treewalker(stmtsnode, global_tb, local_tbstk, builder, module);
            // index++, load a new value to item
            LLVMValueRef index_plus_plus =  LLVMBuildAdd(builder, boris_codegen_expr(simulated_index, builder, module, global_tb, local_tbstk), One32, "");
            LLVMBuildStore(builder, index_plus_plus, record_item_index->value->address);
            LLVMValueRef loop_offset = boris_codegen_expr(simulated_index, builder, module, global_tb, local_tbstk);
            LLVMValueRef loop_indices[] = { loop_offset };
            LLVMValueRef loop_element_address = LLVMBuildGEP(builder, array_address, loop_indices, 1, "");
            LLVMBuildStore(builder, LLVMBuildLoad(builder, loop_element_address, ""), record_item->value->address);
            LLVMBuildBr(builder, CheckBasicBlock); // build unconditional jump to check block.
            // put builder at end
            LLVMPositionBuilderAtEnd(builder, EndBasicBlock);
            // remove the item, item_index, and clean up simluated node
            remove_symbol(snode->sval, GLOBAL_SCOPE, endfornode->line, global_tb);
            remove_symbol(temp_string, GLOBAL_SCOPE, endfornode->line, global_tb);
            free(simulated_index);
            free(simulated_boolop);
            free(simulated_length);
            free(simulated_boolexpr);
            break;
        }
        case NODETYPE_PRINT_STATEMENT: {                                                //1056
            // when print a expr, the type of expr should be known and in {arrat, tuple, int}
            struct pNode* exprnode = p->childs[1];
            int valuetype = type_synthesis(exprnode, global_tb, local_tbstk);
            int truthlist[] = {VALUETYPE_ARRAY, VALUETYPE_INT, VALUETYPE_TUPLE};
            check_type_in_list(valuetype, truthlist, 3);
            if (valuetype == VALUETYPE_INT) {
                LLVMValueRef expr_code = boris_codegen_expr(p->childs[1], builder, module, global_tb, local_tbstk);
                LLVMValueRef format_int = LLVMBuildGlobalStringPtr(
                    builder,
                    "%d\n",
                    "format_int"
                );
                LLVMValueRef PrintfArgs[] = { format_int, expr_code };
                LLVMValueRef PrintfFunction = LLVMGetNamedFunction(module, "printf");
                LLVMBuildCall(
                    builder,
                    PrintfFunction,
                    PrintfArgs,
                    2,
                    ""
                );
            } else if (valuetype == VALUETYPE_ARRAY) {
                struct sNode* snode = (struct sNode*)exprnode->childs[0];
                struct symboltable* matched_tb = get_matched_symboltable(snode->sval, global_tb, local_tbstk);
                struct symboltableRecord* record = lookup_symbol(snode->sval, matched_tb->scope, matched_tb);
                int width = record->value->ivallistlength;
                LLVMValueRef format_int_0 = LLVMBuildGlobalStringPtr(
                    builder,
                    "%d,",
                    "format_int_0"
                );
                LLVMValueRef format_int_1 = LLVMBuildGlobalStringPtr(
                    builder,
                    "%d",
                    "format_int_1"
                );
                LLVMValueRef format_array_beg = LLVMBuildGlobalStringPtr(builder, "[", "format_array_beg");
                LLVMValueRef format_array_end = LLVMBuildGlobalStringPtr(builder, "]\n", "format_array_end");
                LLVMValueRef PrintfArgsArrayBeg[] = { format_array_beg };
                LLVMValueRef PrintfArgsArrayEnd[] = { format_array_end };
                LLVMValueRef PrintfFunction = LLVMGetNamedFunction(module, "printf");
                LLVMBuildCall(
                    builder,
                    PrintfFunction,
                    PrintfArgsArrayBeg,
                    1,
                    ""
                );
                for (int i = 0; i < width; i++){
                    LLVMValueRef offset = LLVMConstInt(LLVMInt32Type(), i, 0); // array access should consider its start point
                    LLVMValueRef indices[] = { offset };
                    LLVMValueRef element_address = LLVMBuildGEP(builder, record->value->address, indices, 1, "");
                    LLVMValueRef expr_code = LLVMBuildLoad(builder, element_address, "");
                    if ( i < width-1){
                        LLVMValueRef PrintfArgs[] = { format_int_0, expr_code };
                        LLVMBuildCall(
                            builder,
                            PrintfFunction,
                            PrintfArgs,
                            2,
                            ""
                        );
                    } else {
                        LLVMValueRef PrintfArgs[] = { format_int_1, expr_code };
                        LLVMBuildCall(
                            builder,
                            PrintfFunction,
                            PrintfArgs,
                            2,
                            ""
                        );
                    }
                }
                LLVMBuildCall(
                    builder,
                    PrintfFunction,
                    PrintfArgsArrayEnd,
                    1,
                    ""
                );
            } else if (valuetype == VALUETYPE_TUPLE && exprnode->pnodetype == NODETYPE_SINGLE_ID_AS_EXPR) {
                struct sNode* snode = (struct sNode*)exprnode->childs[0];
                struct symboltable* matched_tb = get_matched_symboltable(snode->sval, global_tb, local_tbstk);
                struct symboltableRecord* record = lookup_symbol(snode->sval, matched_tb->scope, matched_tb);
                int width = record->value->ivallistlength;
                LLVMValueRef format_int_0 = LLVMBuildGlobalStringPtr(
                    builder,
                    "%d,",
                    "format_int_0"
                );
                LLVMValueRef format_int_1 = LLVMBuildGlobalStringPtr(
                    builder,
                    "%d",
                    "format_int_1"
                );
                LLVMValueRef format_tuple_beg = LLVMBuildGlobalStringPtr(builder, "(", "format_tuple_beg");
                LLVMValueRef format_tuple_end = LLVMBuildGlobalStringPtr(builder, ")\n", "format_tuple_end");
                LLVMValueRef PrintfArgsTupleBeg[] = { format_tuple_beg };
                LLVMValueRef PrintfArgsTupleEnd[] = { format_tuple_end };
                LLVMValueRef PrintfFunction = LLVMGetNamedFunction(module, "printf");
                LLVMBuildCall(
                    builder,
                    PrintfFunction,
                    PrintfArgsTupleBeg,
                    1,
                    ""
                );
                for (int i = 0; i < width; i++){
                    LLVMValueRef offset = LLVMConstInt(LLVMInt32Type(), i, 0); // array access should consider its start point
                    LLVMValueRef indices[] = { offset };
                    LLVMValueRef element_address = LLVMBuildGEP(builder, record->value->address, indices, 1, "");
                    LLVMValueRef expr_code = LLVMBuildLoad(builder, element_address, "");
                    if ( i < width-1){
                        LLVMValueRef PrintfArgs[] = { format_int_0, expr_code };
                        LLVMBuildCall(
                            builder,
                            PrintfFunction,
                            PrintfArgs,
                            2,
                            ""
                        );
                    } else {
                        LLVMValueRef PrintfArgs[] = { format_int_1, expr_code };
                        LLVMBuildCall(
                            builder,
                            PrintfFunction,
                            PrintfArgs,
                            2,
                            ""
                        );
                    }
                }
                LLVMBuildCall(
                    builder,
                    PrintfFunction,
                    PrintfArgsTupleEnd,
                    1,
                    ""
                );
            } else if (valuetype == VALUETYPE_TUPLE && exprnode->pnodetype == NODETYPE_EXPR_COMMA_EXPR){
                // need to create a simulated temp tuple.
                char temp_string[20];
                get_temp_string(temp_string);
                int width = get_expr_width(exprnode, global_tb, local_tbstk);
                // allocate a temp global tuple
                struct symboltableRecord* record_tuple= declare_symbol(temp_string, VALUETYPE_TUPLE, GLOBAL_SCOPE, exprnode->line, global_tb);
                init_int_list_symbol(temp_string, GLOBAL_SCOPE, 0, width, exprnode->line, global_tb);
                LLVMValueRef tuple_address =  LLVMBuildArrayAlloca(builder, LLVMInt32Type(), LLVMConstInt(LLVMInt32Type(), width, 0), temp_string);
                record_tuple->value->address = tuple_address;
                // populate tuple
                LLVMValueRef results[width];
                read_all_values_in_expr(exprnode, results, global_tb, local_tbstk, builder, module);
                populate_into_address(results, width, tuple_address, builder);
                // print 
                LLVMValueRef format_int_0 = LLVMBuildGlobalStringPtr(
                    builder,
                    "%d,",
                    "format_int_0"
                );
                LLVMValueRef format_int_1 = LLVMBuildGlobalStringPtr(
                    builder,
                    "%d",
                    "format_int_1"
                );
                LLVMValueRef format_tuple_beg = LLVMBuildGlobalStringPtr(builder, "(", "format_tuple_beg");
                LLVMValueRef format_tuple_end = LLVMBuildGlobalStringPtr(builder, ")\n", "format_tuple_end");
                LLVMValueRef PrintfArgsTupleBeg[] = { format_tuple_beg };
                LLVMValueRef PrintfArgsTupleEnd[] = { format_tuple_end };
                LLVMValueRef PrintfFunction = LLVMGetNamedFunction(module, "printf");
                LLVMBuildCall(
                    builder,
                    PrintfFunction,
                    PrintfArgsTupleBeg,
                    1,
                    ""
                );
                for (int i = 0; i < width; i++){
                    LLVMValueRef offset = LLVMConstInt(LLVMInt32Type(), i, 0); // array access should consider its start point
                    LLVMValueRef indices[] = { offset };
                    LLVMValueRef element_address = LLVMBuildGEP(builder, record_tuple->value->address, indices, 1, "");
                    LLVMValueRef expr_code = LLVMBuildLoad(builder, element_address, "");
                    if ( i < width-1){
                        LLVMValueRef PrintfArgs[] = { format_int_0, expr_code };
                        LLVMBuildCall(
                            builder,
                            PrintfFunction,
                            PrintfArgs,
                            2,
                            ""
                        );
                    } else {
                        LLVMValueRef PrintfArgs[] = { format_int_1, expr_code };
                        LLVMBuildCall(
                            builder,
                            PrintfFunction,
                            PrintfArgs,
                            2,
                            ""
                        );
                    }
                }
                LLVMBuildCall(
                    builder,
                    PrintfFunction,
                    PrintfArgsTupleEnd,
                    1,
                    ""
                );
                // clean up temp global tuple
                remove_symbol(temp_string, GLOBAL_SCOPE, exprnode->line, global_tb);
            }
            break;
        }
        case NODETYPE_RETURN_STATEMENT: {                                               //1057
            // when return a expr, the type of expr should be known and in {tuple, int}
            // must be in a local enviroment
            if (LOCAL_ENV){
                struct pNode* exprnode = p->childs[1];
                int valuetype = type_synthesis(exprnode, global_tb, local_tbstk);
                int truthlist[] = {VALUETYPE_INT, VALUETYPE_TUPLE};
                check_type_in_list(valuetype, truthlist, 2);
                if (valuetype == VALUETYPE_INT){
                    LLVMBuildRet(builder, boris_codegen_expr(exprnode, builder, module, global_tb, local_tbstk));
                } else {
                    printf("no implementation - return\n");
                    exit(999);
                }
                pop_symboltableStack(local_tbstk);
            } else {
                fprintf(stderr,RED"can't use return statementout side local enviroment.\n"RESET);
                exit(999);
            }
            break;
        }
        case NODETYPE_NO_EXPR_LOCAL_DECL: {                                             //1075 
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
        case NODETYPE_LOCAL_DECL: {                                                     //1059
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
        case NODETYPE_NO_EXPR_GLOBAL_DECL: {                                            //1074
            // declare global id with no expr in any env
            // if in local, then declare a local `link` and check global variable has a 'known' type
            // if in global, then declare a global `unknown` symbol
            struct sNode * snode = (struct sNode *)(p->childs[1]);
            if (check_current_scope(global_tb, local_tbstk, GLOBAL_SCOPE)){ 
                declare_symbol(snode->sval, VALUETYPE_UNKNOWN, GLOBAL_SCOPE, snode->line, global_tb);
            } else if (check_current_scope(global_tb, local_tbstk, LOCAL_SCOPE)){
                struct symboltableRecord* record = lookup_symbol(snode->sval, GLOBAL_SCOPE, global_tb);
                if (record != NULL && record->value != NULL) {
                    int truthlist[] = {VALUETYPE_ARRAY, VALUETYPE_INT, VALUETYPE_TUPLE}; 
                    check_type_in_list(record->valuetype, truthlist, 3);
                    struct symboltableRecord* local_record = declare_symbol(snode->sval, record->valuetype, LOCAL_SCOPE, snode->line, top_symboltableStack(local_tbstk));

                } else {
                    fprintf(stderr, RED"[treewalker] error, can't access this global id in local env. in line %d"RESET, p->line);
                    exit(999);
                }
            }
            break;
        }
        case NODETYPE_GLOBAL_DECL: {                                                    //1060
            // only declare global id with expr in global env
            if (check_current_scope(global_tb, local_tbstk, GLOBAL_SCOPE) == 0){
                fprintf(stderr, RED"[treewalker] can't declare global id with expr outside global env. in line %d"RESET, p->line);
                exit(999);
            }
            struct sNode * snode = (struct sNode *)(p->childs[1]); 
            struct pNode * exprnode = p->childs[3];
            // check type 
            int valuetype = type_synthesis(exprnode, global_tb, local_tbstk);
            int truthlist[] = {VALUETYPE_INT, VALUETYPE_TUPLE};
            check_type_in_list(valuetype, truthlist, 2);
            struct symboltableRecord* record = declare_symbol(snode->sval, valuetype, GLOBAL_SCOPE, snode->line, global_tb);
            // build global declare code
            if(valuetype == VALUETYPE_INT){
                LLVMValueRef expr_code = boris_codegen_expr(p->childs[3], builder, module, global_tb, local_tbstk);
                LLVMValueRef lhs = LLVMBuildAlloca(builder, LLVMInt32Type(), snode->sval);
                //LLVMValueRef lhs = LLVMAddGlobal(module, LLVMInt32Type(), snode->sval);
                LLVMBuildStore(builder, expr_code, lhs);
                init_int_symbol(snode->sval, GLOBAL_SCOPE, snode->line, global_tb);
                update_int_symbol(snode->sval, GLOBAL_SCOPE, 1, snode->line, global_tb);
                record->value->address = lhs;
            } else if (valuetype == VALUETYPE_TUPLE) {
                int width = get_expr_width(exprnode, global_tb, local_tbstk);
                // allocate tuple
                init_int_list_symbol(snode->sval, GLOBAL_SCOPE, 0, width, snode->line, global_tb);
                LLVMValueRef tuple_address =  LLVMBuildArrayAlloca(builder, LLVMInt32Type(), LLVMConstInt(LLVMInt32Type(), width, 0), snode->sval);
                struct symboltableRecord* record = lookup_symbol(snode->sval, GLOBAL_SCOPE, global_tb);
                record->value->address = tuple_address;
                // populate tuple
                LLVMValueRef results[width];
                read_all_values_in_expr(exprnode, results, global_tb, local_tbstk, builder, module);
                populate_into_address(results, width, tuple_address, builder);
            }
            break;
        }
        case NODETYPE_ARRAY_DECL_WITH_ANONY_FUNC:                                       //1062
        case NODETYPE_ARRAY_DECL: {                                                     //1061
            //KW_ARRAY ID OP_LBRAK expr OP_DOTDOT expr OP_RBRAK OP_SEMI 
            struct sNode * snode = (struct sNode *)(p->childs[1]); 
            struct pNode * exprnode_beg = p->childs[3];
            struct pNode * exprnode_end = p->childs[5];
            check_type_equal(type_lookup(snode->sval, global_tb, local_tbstk), VALUETYPE_UNKNOWN);
            check_type_equal(type_synthesis(exprnode_beg, global_tb, local_tbstk), VALUETYPE_INT);
            check_type_equal(type_synthesis(exprnode_end, global_tb, local_tbstk), VALUETYPE_INT);
            struct symboltable* matched_tb = get_matched_symboltable(snode->sval, global_tb, local_tbstk);
            set_symbol_type(snode->sval, VALUETYPE_ARRAY, matched_tb->scope, snode->line, matched_tb);
            // restrict the expr to be a SINGLE_INT_AS_EXPR in the following
            if (exprnode_beg->pnodetype != NODETYPE_SINGLE_INT_AS_EXPR || exprnode_end->pnodetype != NODETYPE_SINGLE_INT_AS_EXPR) {
                printf("array decl, only allow integers now, no implementation\n");
                exit(666);
            }
            int beg = ((struct iNode *)exprnode_beg->childs[0])->ival;
            int end = ((struct iNode *)exprnode_end->childs[0])->ival;
            init_int_list_symbol(snode->sval, matched_tb->scope, beg, end-beg+1, snode->line, matched_tb);
            LLVMValueRef array_address =  LLVMBuildArrayAlloca(builder, LLVMInt32Type(), LLVMConstInt(LLVMInt32Type(), end-beg+1, 0), snode->sval);
            struct symboltableRecord* record = lookup_symbol(snode->sval, matched_tb->scope, matched_tb);
            record->value->address = array_address;
            break;
        }
        
        case NODETYPE_BOOLEXPR:{                                                        //1042
            //expr bool_op expr
            // only allow int in bool expr compare
            struct pNode * exprnode_beg = p->childs[0];
            struct placeholderNode* op = (struct placeholderNode*) p->childs[1];
            struct pNode * exprnode_end = p->childs[2];
            check_type_equal(type_synthesis(exprnode_beg, global_tb, local_tbstk), VALUETYPE_INT);
            check_type_equal(type_synthesis(exprnode_end, global_tb, local_tbstk), VALUETYPE_INT);
            LLVMIntPredicate prdicate = LLVMIntNE;
            if (op->tok == OP_NOTEQUA) prdicate = LLVMIntNE;
            else if (op->tok == OP_EQUAL) prdicate = LLVMIntEQ;
            else if (op->tok == OP_GREATER) prdicate = LLVMIntSGT;
            else if (op->tok == OP_GREATEREQUAL) prdicate = LLVMIntSGE;
            else if (op->tok == OP_LESS) prdicate = LLVMIntSLT;
            else if (op->tok == OP_LESSEQUAL) prdicate = LLVMIntSLE;
            else printf("unsupported bool operator, treated as not-equal?");
            LLVMValueRef If = LLVMBuildICmp(
                builder,
                prdicate,
                boris_codegen_expr(exprnode_beg, builder, module, global_tb, local_tbstk),
                boris_codegen_expr(exprnode_end, builder, module, global_tb, local_tbstk),
                ""
            );
            LLVMBuildCondBr(builder, If, p->true_block, p->false_block);
            break;
        }
        case NODETYPE_FUNC_DEFN: {                                                      //1068
            // only allow 1 formal pararamter
            if (p->childs[4] != NULL) {
                fprintf(stderr, RED"[treewalker] bad function defn. only 1 formal pararamter is allowed. in line %d"RESET, p->line);
                exit(999);
            }
            struct sNode * func_name = (struct sNode *)(p->childs[1]); 
            struct sNode * para_name = (struct sNode *)(p->childs[3]); 
            struct pNode * body = p->childs[6];
            //printf("-------determine defn `%s` input and return type------\n", func_name->sval);
            int formal_parameter_valuetype = determine_formal_parameter_valuetype(p);
            int return_valuetype = determine_return_valuetype(p, formal_parameter_valuetype, global_tb);
            //printf("for function `%s`, input is `%c`, return type is `%c`\n",func_name->sval, formal_parameter_valuetype, return_valuetype);
            //printf("-------determine finish--------------------------\n");
            declare_symbol(func_name->sval, VALUETYPE_FUNC, GLOBAL_SCOPE, func_name->line, global_tb);
            init_func_symbol(func_name->sval, GLOBAL_SCOPE, formal_parameter_valuetype, return_valuetype, p, func_name->line, global_tb);
            //build functions here
            LLVMTypeRef ArgsTyList[1];
            LLVMTypeRef RetTy;
            if (formal_parameter_valuetype == VALUETYPE_INT) {
                ArgsTyList[0] = LLVMInt32Type();
            } else {
                ArgsTyList[0] =  LLVMPointerType(LLVMInt32Type(), 0);
            } 
            if (return_valuetype == VALUETYPE_INT) {
                RetTy = LLVMInt32Type();
            } else {
                RetTy =  LLVMPointerType(LLVMInt32Type(), 0);
            }
            LLVMTypeRef FuncTy = LLVMFunctionType(
                RetTy,
                ArgsTyList,
                1,
                0
            );
            LLVMValueRef Function = LLVMAddFunction(module, func_name->sval, FuncTy);
            LLVMBasicBlockRef InitBasicBlock = LLVMAppendBasicBlock(Function, "Entry");
            LLVMBuilderRef func_builder = LLVMCreateBuilder();
            LLVMPositionBuilderAtEnd(func_builder, InitBasicBlock);
            struct symboltable* local_tb = init_symboltable(MAX_SYMBOLTABLE_SIZE, LOCAL_SCOPE);
            push_symboltableStack(local_tb, local_tbstk);
            if (formal_parameter_valuetype == VALUETYPE_INT) {
                struct symboltableRecord* record = declare_symbol(para_name->sval, VALUETYPE_INT, LOCAL_SCOPE, para_name->line, local_tb);
                init_int_symbol(para_name->sval, LOCAL_SCOPE, para_name->line, local_tb);
                update_int_symbol(para_name->sval, LOCAL_SCOPE, 1, para_name->line, local_tb);
                LLVMValueRef local_address = LLVMBuildAlloca(func_builder, LLVMInt32Type(), para_name->sval);
                LLVMBuildStore(func_builder, LLVMGetParam(Function, 0), local_address);
                record->value->address = local_address;
                //record->value->isPara = 1; // set it to be a parameter. will change the way it's refered.
                //record->value->paraPassedValue = LLVMGetParam(Function, 0);
            } else {
                printf("no implementation - defun\n");
                exit(999);
            }
            treewalker(body, global_tb, local_tbstk, func_builder, module);
            LLVMDisposeBuilder(func_builder);
            break;
        }
        // leaf
        case NODETYPE_ID:                                                            //1024
        case NODETYPE_INT:                                                           //1025
        //#expr should not be viisted 
        case NODETYPE_EXPR_COMMA_EXPR:                                               //1027
        case NODETYPE_EXPR_MINUS_EXPR:                                               //1028
        case NODETYPE_EXPR_PLUS_EXPR:                                                //1029
        case NODETYPE_EXPR_DIV_EXPR:                                                 //1030
        case NODETYPE_EXPR_MULT_EXPR:                                                //1031
        case NODETYPE_LPAR_EXPR_RPAR:                                                //1032
        case NODETYPE_SINGLE_ID_AS_EXPR:                                             //1033
        case NODETYPE_FUNC_CALL_AS_EXPR:                                             //1034
        case NODETYPE_TUPLE_REF_AS_EXPR:                                             //1035
        case NODETYPE_ARRAY_REF_AS_EXPR:                                             //1036
        case NODETYPE_SINGLE_INT_AS_EXPR:                                            //1058
        //#lhsitem should not be visited
        case NODETYPE_SINGLE_ID_AS_LHSITEM:                                          //1037
        case NODETYPE_TUPLE_REF_AS_LHSITEM:                                          //1038
        case NODETYPE_ARRAY_REF_AS_LHSITEM:                                          //1039
        case NODETYPE_LHS:                                                           //1040
        case NODETYPE_COMMA_LHSITEN_LIST:                                            //1041
        //#inner parts
        case NODETYPE_RANGE:                                                         //1043
        case NODETYPE_ARRAY_ID:                                                      //1054
        case NODETYPE_ITER_ID:                                                       //1076
        case NODETYPE_COMMA_ID_LIST:                                                 //1067
        {
            printf("should not visit this node directly in treewalker: %d\n", p->pnodetype);
            exit(999);
        }
        default: printf("visit unsupported node type for treewalker: %d\n", p->pnodetype);
    }
}
