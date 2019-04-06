# include <stdio.h>
# include <stdlib.h>
# include <stdarg.h>
# include <string.h>
# include "boris.h"
# include "boris.tab.h" // yylloc definition, token number


void yyerror(char *s, ...){
    va_list ap;
    va_start(ap, s);
    fprintf(stderr, RED);
    vfprintf(stderr, s, ap);
    fprintf(stderr, " In position %d,%d-%d,%d.",yylloc.first_line,yylloc.first_column, yylloc.last_line, yylloc.last_column);
    fprintf(stderr, RESET);
    fprintf(stderr, "\n");
}

void lexerTester(){ 
    printf("> Hello from lexerTester for boris compiler!\n");
    int tok;
    do {
        tok = yylex();
        if(tok == 0) break;
        printf("token[%d].", tok);
        if (tok == END_OF_LINE) {
                printf("[\\n]");\
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
    parent->childscount = 0;
    parent->sval = sval; 
    printf("[Succesfully] build a sNode[%d]: %s\n", parent->pnodetype, parent->sval); //TODO, DEBUG
    return (struct pNode *)parent;
}

struct pNode *newiNode(int ival){
    struct iNode * parent = malloc(sizeof(struct iNode));
    if(!parent) {
        yyerror("Out of space.");
        exit(0);
    }
    parent->pnodetype = NODETYPE_INT;
    parent->childscount = 0;
    parent->ival = ival; 
    printf("[Succesfully] build a iNode[%d]: %d\n", parent->pnodetype, parent->ival); //TODO, DEBUG
    return (struct pNode *)parent;
}


struct pNode *newplaceholderNode(int tok){
    struct placeholderNode * parent = malloc(sizeof(struct placeholderNode));
    if(!parent) {
        yyerror("Out of space.");
        exit(0);
    }
    parent->pnodetype = NODETYPE_PLACEHOLDER;
    parent->childscount = 0;
    parent->tok = tok; 
    printf("[Succesfully] build a placeholderNode[%d]: %d\n", parent->pnodetype, parent->tok); //TODO, DEBUG
    return (struct pNode *)parent;
}

void printManySpace(int count){
    for (int i = 0; i < count; i++) printf(" ");
}

void visualize(struct pNode *p, int level){
    if (p == NULL) {
        printManySpace(level*4);
        printf("[EMPTY_NODE]\n"); 
        return;
    }
    char nodetype2nodestr[][50] = {
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
    };

    switch(p->pnodetype) {
        case NODETYPE_ID: {
            printManySpace(level*4);
            char* nodestr = nodetype2nodestr[p->pnodetype-NODETYPE_ID];
            printf("[%s]%s\n", nodestr, ((struct sNode*)p)->sval);
            break;
        }
        case NODETYPE_INT: {
            printManySpace(level*4); 
            char* nodestr = nodetype2nodestr[p->pnodetype-NODETYPE_ID];
            printf("[%s]%d\n", nodestr, ((struct iNode*)p)->ival);
            break;
        }
        case NODETYPE_PLACEHOLDER: {
            printManySpace(level*4); 
            char* nodestr = nodetype2nodestr[p->pnodetype-NODETYPE_ID];
            printf("[%s]%d\n", nodestr, ((struct placeholderNode*)p)->tok);
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
        case NODETYPE_ARRAY_DECL_WITH_ANONY_FUNC:{
            printManySpace(level*4); 
            char* nodestr = nodetype2nodestr[p->pnodetype-NODETYPE_ID];
            printf("[%s-%d]\n", nodestr, p->pnodetype);
            for (int i = 0; i < p->childscount; i++){
                struct pNode *child = p->childs[i];
                visualize(child, level+1);
            }
            break;
        }
        default: printf("internal error: bad node type%d\n", p->pnodetype);
    }
}

int main(){
    return yyparse();
}