/*
* Symbol Table Data Structure for the boris compiler
*
* Author: Yi Zhou
* April 13, 2019
*
*/
# include <stdio.h>
# include <stdlib.h>
# include <stdarg.h>
# include <string.h>
# include "boris.h"
# include "boris.tab.h" // yylloc definition, token number

struct symboltable* init_symboltable(int length){
    struct symboltable* tb = malloc(sizeof(struct symboltable));//TODO free
    struct symboltableRecord* records = malloc(length * sizeof(struct symboltableRecord));//TODO free
    if(!records || !tb) {
        yyerror("Out of space.");
        exit(0);
    }
    for (int i = 0; i < length; i++) { 
        // default setting for symboltableRecord
        records[i].valid = 0;
        records[i].line = -1; 
        records[i].scope = -1;
        records[i].sval = NULL;
        records[i].value = NULL;
        records[i].valuetype = 'U';
    }
    tb->length = length;
    tb->records = records;
    return tb;
}

struct symboltableRecord* lookup_symbol(char* sval, int scope, struct symboltable* tb){
    if (scope != GLOBAL_SCOPE && scope != LOCAL_SCOPE){
        yyerror("internal error, unsupported scope: %d", scope);
        exit(997);
    } else if (tb == NULL || tb->length <= 0 || tb->records == NULL) {
        yyerror("internal error, invalid symbol table");
        exit(998);
    } 
    struct symboltableRecord* ret = NULL;
    struct symboltableRecord* p = tb->records;
    for (int i = 0; i < tb->length; i++, p++){
        if((p->scope == scope) && (strcmp(p->sval, sval) == 0) && (p->valid != 0)){
            if (ret != NULL) {
                yyerror("duplicated declaration for %s in this scope", sval);
                exit(999);
            } else {
                ret = p;
            } 
        }
    }
    return ret;
}

int next_available_symbol_slot(struct symboltable* tb){
    if (tb == NULL || tb->length <= 0 || tb->records == NULL) {
        yyerror("internal error, invalid symbol table");
        exit(998);
    }
    for (int i = 0; i < tb->length; i++){
        if(tb->records[i].valid == 0){
            return i;
        }
    }
    return -1;
}

void declare_symbol(char* sval, int valuetype, int scope, int line, struct symboltable* tb){
    if (valuetype != VALUETYPE_FUNC && valuetype != VALUETYPE_ARRAY && valuetype != VALUETYPE_TUPLE && valuetype != VALUETYPE_INT){
        yyerror("internal error, bad value type");
        exit(994);
    }
    if (lookup_symbol(sval, scope, tb) != NULL) {
        yyerror("duplicated declare for %s in this scope", sval);
        exit(999);
    } 
    int index = next_available_symbol_slot(tb);
    if (index == -1) {
        yyerror("symbol tabel overflow, need more slots", sval);
        exit(995);
    }
    // declare a symbol here
    tb->records[index].sval = sval;
    tb->records[index].valuetype = valuetype;
    tb->records[index].scope = scope;
    tb->records[index].line = line;
    tb->records[index].valid += 1;
    tb->records[index].value = NULL;
    
    return;
}

void update_int_symbol(char* sval, int scope, int ival, struct symboltable* tb){
    struct symboltableRecord* r = lookup_symbol(sval, scope, tb);
    if (r == NULL) {
        yyerror("update a undeclared symbol %s in this scope is not valid", sval);
        exit(993);
    }
    if (r->valuetype != VALUETYPE_INT) {
        yyerror("can't update symbol %s in this scope with a int value", sval);
        exit(992);
    }
    // unpdate the symbol value here
    struct symboltableRecordValue* value = malloc(sizeof(struct symboltableRecordValue)); //TODO Closed. will be free in remove_symbol.
    if(!value) {
        yyerror("Out of space.");
        exit(0);
    }
    value->ival = ival;
    r->value = value;
    return;
}

void update_int_list_symbol(char* sval, int scope, int* ivallist, int ivallistlength, struct symboltable* tb){
    struct symboltableRecord* r = lookup_symbol(sval, scope, tb);
    if (r == NULL) {
        yyerror("update a undeclared symbol %s in this scope is not valid", sval);
        exit(993);
    }
    if (r->valuetype != VALUETYPE_TUPLE &&r->valuetype != VALUETYPE_ARRAY) {
        yyerror("can't update symbol %s in this scope with a list of int value", sval);
        exit(991);
    }
    // unpdate the symbol value here
    struct symboltableRecordValue* value = malloc(sizeof(struct symboltableRecordValue)); //TODO Closed. will be free in remove_symbol.
    if(!value) {
        yyerror("Out of space.");
        exit(0);
    }
    value->ivallist = ivallist;
    value->ivallistlength = ivallistlength;
    r->value = value;
    return;
}

void remove_symbol(char* sval, int scope, struct symboltable* tb){
    if (scope != GLOBAL_SCOPE && scope != LOCAL_SCOPE){
        yyerror("internal error, unsupported scope: %d", scope);
        exit(997);
    } else if (tb == NULL || tb->length <= 0 || tb->records == NULL) {
        yyerror("internal error, invalid symbol table");
        exit(998);
    }
    struct symboltableRecord* record = lookup_symbol(sval, scope, tb);
    if (record != NULL) {
        record->valid = 0;
        if (record->value != NULL) {
            switch (record->valuetype) {
                case VALUETYPE_INT:{
                    free(record->value);
                    break;
                }
                case VALUETYPE_ARRAY:
                case VALUETYPE_TUPLE:{
                    free(record->value->ivallist);
                    free(record->value);
                    break;
                }
                default:
                    printf("internal error, remove unsupported valuetype %c", record->valuetype);
            }
        }
    }
    return;
}

void print_symboltableRecord(struct symboltableRecord* record){
    printf("[Record]");
    if (record->valid){
        printf("sval:%s, line:%d, scope:%c, valuetype:%c, ", record->sval, record->line, record->scope, record->valuetype);
        switch (record->valuetype) {
            case VALUETYPE_INT:{
                if (record->value != NULL) {
                     printf("value:%d", record->value->ival);
                } else { 
                    printf("value:uninitialized");
                }
                break;
            }
            case VALUETYPE_ARRAY:{
                if (record->value != NULL) { 
                    printf("value:[");
                    for (int i = 0; i < record->value->ivallistlength; i++) 
                        {
                            if (i != 0) printf(",");
                            printf("%d", record->value->ivallist[i]);
                        }
                     printf("]");
                }else {
                     printf("value:uninitialized");
                }
                break;
            }
            case VALUETYPE_TUPLE:{
                if (record->value != NULL) { 
                    printf("value:(");
                    for (int i = 0; i < record->value->ivallistlength; i++) 
                        {
                            if (i != 0) printf(",");
                            printf("%d", record->value->ivallist[i]);
                        }
                     printf(")");
                }else {
                     printf("value:uninitialized");
                }
                break;
            }
            default:
                printf("internal error, unsupported valuetype %c", record->valuetype);
        }
        printf("\n");
    } else{
        printf("not valid\n");
    }
    return;
}

void print_symboltable(struct symboltable* tb){
    printf("> SymbolTable[length: %d]\n", tb->length);
    for( int i = 0; i < tb->length; i++){
        print_symboltableRecord(tb->records+i);
    }
    return;
}


void symbolTableTester(){
    struct symboltable* tb = init_symboltable(3);
    
    // init a global int value "hello" in line 10
    declare_symbol("hello", VALUETYPE_INT, GLOBAL_SCOPE, 10, tb);
    update_int_symbol("hello", GLOBAL_SCOPE, 1, tb);

    // init a local int value "world" in line 12
    declare_symbol("world", VALUETYPE_INT, LOCAL_SCOPE, 12, tb);
    update_int_symbol("world", LOCAL_SCOPE, 2, tb);
    
    // update a global int value "hello" 
    update_int_symbol("hello", GLOBAL_SCOPE, 3, tb);

    // declare an already exists symbol. it should fail
    // declare_symbol("world", VALUETYPE_INT, LOCAL_SCOPE, 12, tb); 

    // declare too many different symbols. it should fail
    // declare_symbol("far", VALUETYPE_INT, LOCAL_SCOPE, 13, tb); declare_symbol("bar", VALUETYPE_INT, LOCAL_SCOPE, 14, tb);

    // declare symbols with a bad valuetype. it should fail
    // declare_symbol("badvaluetype", VALUETYPE_INT+999, LOCAL_SCOPE, 15, tb);

    print_symboltable(tb);

    // remove hello
    remove_symbol("hello", GLOBAL_SCOPE, tb);
    print_symboltable(tb);

    // init a local array 
    declare_symbol("myarray", VALUETYPE_ARRAY, LOCAL_SCOPE, 99, tb);
    int listvalue[] = {1,2,3};
    update_int_list_symbol("myarray", LOCAL_SCOPE, listvalue, 3, tb);
    print_symboltable(tb);

    // init a global tuple
    declare_symbol("mytuple", VALUETYPE_TUPLE, GLOBAL_SCOPE, 32, tb);
    int listvalue2[] = {9,9,9,9,9,9,9,9};
    update_int_list_symbol("mytuple", GLOBAL_SCOPE, listvalue2, 8, tb);
    print_symboltable(tb);


}