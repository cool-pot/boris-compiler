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
    } while (tok);
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
    for ( int i = 0; i < num; i++ ) {
        parent->childs[i] = va_arg ( arguments, struct pNode * ); 
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
    parent->pnodetype = NODETYPE_STRING;
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

int main(){
    return yyparse();
}