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
    parent->tok = tok; 
    printf("[Succesfully] build a placeholderNode[%d]: %d\n", parent->pnodetype, parent->tok); //TODO, DEBUG
    return (struct pNode *)parent;
}

void printManySpace(int count){
    for (int i = 0; i < count; i++) printf(" ");
}

void visualize(struct pNode *p, int level){
    char nodetype2nodestr[][50] = {
        "ID",
        "INT",
        "PLACEHOLDER",
        "EXPR_COMMA_EXPR",
        "EXPR_MINUS_EXPR",
        "EXPR_PLUS_EXPR",
        "EXPR_DIV_EXPR",
        "EXPR_MULT_EXPR",
        "LPAR_EXPR_LPAR",
        "SINGLE_ID_AS_EXPR",
        "FUNC_CALL_AS_EXPR"
    };

    switch(p->pnodetype) {
        case NODETYPE_ID: {
            printManySpace(level*4);
            char* nodestr = nodetype2nodestr[p->pnodetype-NODETYPE_ID];
            printf("[%s]%s\n", nodestr, ((struct sNode*)p)->sval);
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
        case NODETYPE_SINGLE_ID_AS_EXPR:{
            printManySpace(level*4); 
            char* nodestr = nodetype2nodestr[p->pnodetype-NODETYPE_ID];
            printf("[%s]\n", nodestr);
            for (int i = 0; i < PARSE_TREE_MAX_CHILD; i++){
                struct pNode *child = p->childs[i];
                if (child == NULL) break;
                visualize(child, level+1);
            }
            break;
        }
        default: printf("internal error: bad node type%c\n", p->pnodetype);
    }
}

int main(){
    return yyparse();
}