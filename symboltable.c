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

#define SYMBOLTABLE_VERBOSE 1

struct symboltable* init_symboltable(int length){
    struct symboltable* tb = malloc(sizeof(struct symboltable));//TODO free
    struct symboltableRecord* records = malloc(length * sizeof(struct symboltableRecord));//TODO free
    if(!records || !tb) {
        fprintf(stderr, RED"[symbol table error]Out of space."RESET);
        exit(0);
    }
    for (int i = 0; i < length; i++) { 
        // default setting for symboltableRecord
        records[i].valid = 0;
        records[i].line = -1; 
        records[i].scope = -1;
        records[i].sval = NULL;
        records[i].value = NULL;
        records[i].valuetype = VALUETYPE_UNKNOWN;
    }
    tb->length = length;
    tb->records = records;
    return tb;
}

struct symboltableRecord* lookup_symbol(char* sval, int scope, struct symboltable* tb){
    if (scope != GLOBAL_SCOPE && scope != LOCAL_SCOPE){
        fprintf(stderr, RED"[symbol table error]internal error, unsupported scope: %d"RESET, scope);
        exit(997);
    } else if (tb == NULL || tb->length <= 0 || tb->records == NULL) {
        fprintf(stderr, RED"[symbol table error]internal error, invalid symbol table"RESET);
        exit(998);
    } 
    struct symboltableRecord* ret = NULL;
    struct symboltableRecord* p = tb->records;
    for (int i = 0; i < tb->length; i++, p++){
        if((p->scope == scope) && (strcmp(p->sval, sval) == 0) && (p->valid != 0)){ // only valid record will be returned
            if (ret != NULL) {
                fprintf(stderr, RED"[symbol table error]duplicated declaration for %s in this scope"RESET, sval);
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
        fprintf(stderr, RED"[symbol table error]internal error, invalid symbol table"RESET);
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
    if (valuetype != VALUETYPE_FUNC && valuetype != VALUETYPE_ARRAY && valuetype != VALUETYPE_TUPLE && valuetype != VALUETYPE_INT && valuetype != VALUETYPE_UNKNOWN){
        fprintf(stderr, RED"[symbol table error]internal error, bad value type"RESET);
        exit(994);
    }
    if (lookup_symbol(sval, scope, tb) != NULL) {
        fprintf(stderr, RED"[symbol table error]duplicated declare for %s in this scope"RESET, sval);
        exit(999);
    } 
    int index = next_available_symbol_slot(tb);
    if (index == -1) {
        fprintf(stderr, RED"[symbol table error]symbol tabel overflow for %s , need more slots"RESET, sval);
        exit(995);
    }
    // declare a symbol here
    tb->records[index].sval = sval;
    tb->records[index].valuetype = valuetype;
    tb->records[index].scope = scope;
    tb->records[index].line = line;
    tb->records[index].valid = 1;
    tb->records[index].value = NULL;
    // verbose print
    if (SYMBOLTABLE_VERBOSE) {
        char symboltype[10];
        char symbolscope[7];
        if (valuetype == VALUETYPE_INT) { strcpy(symboltype, "int"); } 
        else if (valuetype == VALUETYPE_FUNC) { strcpy(symboltype, "func"); } 
        else if (valuetype == VALUETYPE_TUPLE) { strcpy(symboltype, "tuple"); } 
        else if (valuetype == VALUETYPE_ARRAY) { strcpy(symboltype, "array"); }
        else if (valuetype == VALUETYPE_UNKNOWN) { strcpy(symboltype, "unknown"); }
        if (scope == GLOBAL_SCOPE) { strcpy(symbolscope, "global"); } 
        else if (scope == LOCAL_SCOPE) { strcpy(symbolscope, "local"); }
        fprintf(stderr, GREEN"[symbol table decl symbol]declare %s %s symbol `%s` in line %d.\n"RESET, symbolscope, symboltype, sval, line);
    }
    
    return;
}

void init_int_symbol(char* sval, int scope, int line, struct symboltable* tb){
    struct symboltableRecord* r = lookup_symbol(sval, scope, tb);
    if (r == NULL) {
        fprintf(stderr, RED"[symbol table error]update a undeclared symbol %s in this scope is not valid"RESET, sval);
        exit(993);
    }
    if (r->valuetype != VALUETYPE_INT) {
        fprintf(stderr, RED"[symbol table error]wrong type, can't init a int value to non-int symbol %s in this scope"RESET, sval);
        exit(992);
    }
    // init the symbol value here
    struct symboltableRecordValue* value = malloc(sizeof(struct symboltableRecordValue)); //TODO Closed. will be free in remove_symbol.
    if(!value) {
        fprintf(stderr, RED"[symbol table error]Out of space."RESET);
        exit(0);
    }
    value->ival = 0; // default value 0
    r->value = value;
    // verbose print
    if (SYMBOLTABLE_VERBOSE) {
        char symbolscope[7];
        if (scope == GLOBAL_SCOPE) { strcpy(symbolscope, "global"); } 
        else if (scope == LOCAL_SCOPE) { strcpy(symbolscope, "local"); }
        fprintf(stderr, GREEN"[symbol table update]init %s int symbol `%s` in line %d.\n"RESET, symbolscope, sval, line);
    }
    return;
}

void update_int_symbol(char* sval, int scope, int ival, int line, struct symboltable* tb){
    struct symboltableRecord* r = lookup_symbol(sval, scope, tb);
    if (r == NULL) {
        fprintf(stderr, RED"[symbol table error]update a undeclared symbol `%s` in this scope is not valid"RESET, sval);
        exit(993);
    }
    if (r->valuetype != VALUETYPE_INT) {
        fprintf(stderr, RED"[symbol table error]wrong type, can't set symbol `%s` in this scope with a int value"RESET, sval);
        exit(992);
    }
    if (r->value == NULL) {
        fprintf(stderr, RED"[symbol table error]not initalized symbol `%s` can't be set."RESET, sval);
        exit(933);
    }
    // set value here
    r->value->ival = ival;
    // verbose print
    if (SYMBOLTABLE_VERBOSE) {
        char symbolscope[7];
        if (scope == GLOBAL_SCOPE) { strcpy(symbolscope, "global"); } 
        else if (scope == LOCAL_SCOPE) { strcpy(symbolscope, "local"); }
        fprintf(stderr, GREEN"[symbol table update]update %s int symbol `%s` to %d in line %d.\n"RESET, symbolscope, sval, ival, line);
    }
    return;
}

void init_int_list_symbol(char* sval, int scope, int ivallist_start, int ivallistlength, int line, struct symboltable* tb){
    struct symboltableRecord* r = lookup_symbol(sval, scope, tb);
    if (r == NULL) {
        fprintf(stderr, RED"[symbol table error]update a undeclared symbol %s in this scope is not valid"RESET, sval);
        exit(993);
    }
    if (r->valuetype != VALUETYPE_TUPLE &&r->valuetype != VALUETYPE_ARRAY) {
        fprintf(stderr, RED"[symbol table error]can't update symbol %s in this scope with a list of int value"RESET, sval);
        exit(991);
    }
    if (r->valuetype == VALUETYPE_TUPLE && ivallist_start != 0) {
        fprintf(stderr, RED"[symbol table error]%s, tuple index must start from 0"RESET, sval);
        exit(991);
    }
    // init the symbol value here
    struct symboltableRecordValue* value = malloc(sizeof(struct symboltableRecordValue)); //TODO Closed. will be free in remove_symbol.
    if(!value) {
        fprintf(stderr, RED"[symbol table error]Out of space."RESET);
        exit(0);
    }
    int* ivallist = malloc(ivallistlength*sizeof(int));
    for (int i = 0; i < ivallistlength; i++) {
        ivallist[i] = 0;
    }
    value->ivallist = ivallist;
    value->ivallist_start = ivallist_start;
    value->ivallistlength = ivallistlength;
    r->value = value;
    // verbose print
    if (SYMBOLTABLE_VERBOSE) {
        char symboltype[6];
        char symbolscope[7];
        if (r->valuetype == VALUETYPE_TUPLE) { strcpy(symboltype, "tuple"); } 
        else if (r->valuetype == VALUETYPE_ARRAY) { strcpy(symboltype, "array"); }
        if (scope == GLOBAL_SCOPE) { strcpy(symbolscope, "global"); } 
        else if (scope == LOCAL_SCOPE) { strcpy(symbolscope, "local"); }
        fprintf(stderr, GREEN"[symbol table update]init %s %s symbol `%s` in line %d.\n"RESET,symbolscope, symboltype, sval, line);
    }
    return;
}

void update_int_list_symbol_itemwise(char* sval, int scope, int updateval, int updateindex, int line, struct symboltable* tb){
    struct symboltableRecord* r = lookup_symbol(sval, scope, tb);
    if (r == NULL) {
        fprintf(stderr, RED"[symbol table error]update a undeclared symbol %s in this scope is not valid"RESET, sval);
        exit(993);
    }
    if (r->valuetype != VALUETYPE_TUPLE && r->valuetype != VALUETYPE_ARRAY) {
        fprintf(stderr, RED"[symbol table error]can't update symbol %s in this scope with a list of int value"RESET, sval);
        exit(991);
    }
    if (r->value == NULL) {
        fprintf(stderr, RED"[symbol table error]can't set uninitialized symbol %s"RESET, sval);
        exit(990);
    }
    if (updateindex < r->value->ivallist_start || updateindex >= ( r->value->ivallist_start + r->value->ivallistlength)){
        fprintf(stderr, RED"[symbol table error]can't update index out of range %s"RESET, sval);
        exit(989);
    }
    // unpdate the symbol value here
    r->value->ivallist[updateindex-r->value->ivallist_start] = updateval;
    // verbose print
    if (SYMBOLTABLE_VERBOSE) {
        char symboltype[6];
        char symbolscope[7];
        if (r->valuetype == VALUETYPE_TUPLE) { strcpy(symboltype, "tuple"); } 
        else if (r->valuetype == VALUETYPE_ARRAY) { strcpy(symboltype, "array"); }
        if (scope == GLOBAL_SCOPE) { strcpy(symbolscope, "global"); } 
        else if (scope == LOCAL_SCOPE) { strcpy(symbolscope, "local"); }
        fprintf(stderr, GREEN"[symbol table update]update %s %s symbol `%s` in line %d. set %s[%d]=%d\n"RESET,symbolscope, symboltype, sval, line, sval, updateindex, updateval);
    }
    return;
}

void remove_symbol(char* sval, int scope, int line, struct symboltable* tb){
    if (scope != GLOBAL_SCOPE && scope != LOCAL_SCOPE){
        fprintf(stderr, RED"[symbol table error]internal error, unsupported scope: %d"RESET, scope);
        exit(997);
    } else if (tb == NULL || tb->length <= 0 || tb->records == NULL) {
        fprintf(stderr, RED"[symbol table error]internal error, invalid symbol table"RESET);
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
                case VALUETYPE_UNKNOWN:{
                    printf(YELLOW"[symbol table warning] remove a never used symbol `%s`. maybe redundant%c"RESET, sval, record->valuetype);
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
                    printf(RED"[symbol table error]`%s`, internal error, remove unsupported valuetype %c"RESET, sval, record->valuetype);
                    exit(984);
            }
        }
        // verbose print
        if (SYMBOLTABLE_VERBOSE) {
            int valuetype = record->valuetype;
            char symboltype[10];
            char symbolscope[7];
            if (valuetype == VALUETYPE_INT) { strcpy(symboltype, "int"); } 
            else if (valuetype == VALUETYPE_FUNC) { strcpy(symboltype, "func"); } 
            else if (valuetype == VALUETYPE_TUPLE) { strcpy(symboltype, "tuple"); } 
            else if (valuetype == VALUETYPE_ARRAY) { strcpy(symboltype, "array"); }
            else if (valuetype == VALUETYPE_UNKNOWN) { strcpy(symboltype, "unknown"); }
            if (scope == GLOBAL_SCOPE) { strcpy(symbolscope, "global"); } 
            else if (scope == LOCAL_SCOPE) { strcpy(symbolscope, "local"); }
            fprintf(stderr, GREEN"[symbol table remove symbol]remove %s %s symbol `%s` in line %d.\n"RESET, symbolscope, symboltype, sval, line);
        }
    }
    return;
}

void set_symbol_type(char* sval, int valuetype, int scope, int line, struct symboltable* tb){
    if (valuetype != VALUETYPE_FUNC && valuetype != VALUETYPE_ARRAY && valuetype != VALUETYPE_TUPLE && valuetype != VALUETYPE_INT && valuetype != VALUETYPE_UNKNOWN){
        fprintf(stderr, RED"[symbol table error]internal error, bad value type"RESET);
        exit(994);
    }
    if (valuetype == VALUETYPE_UNKNOWN){ // only unknown type can be reset
        fprintf(stderr, RED"[symbol table error]can't reset to unknown type"RESET);
        exit(949);
    }
    struct symboltableRecord* record = lookup_symbol(sval, scope, tb);
    if (record == NULL) {
        fprintf(stderr, RED"[symbol table error]symbol `%s` not found in this scope"RESET, sval);
        exit(955);
    } 
    // set symbol type here
    record->valuetype = valuetype;
    // verbose print
    if (SYMBOLTABLE_VERBOSE) {
        char symboltype[10];
        char symbolscope[7];
        if (valuetype == VALUETYPE_INT) { strcpy(symboltype, "int"); } 
        else if (valuetype == VALUETYPE_FUNC) { strcpy(symboltype, "func"); } 
        else if (valuetype == VALUETYPE_TUPLE) { strcpy(symboltype, "tuple"); } 
        else if (valuetype == VALUETYPE_ARRAY) { strcpy(symboltype, "array"); }
        else if (valuetype == VALUETYPE_UNKNOWN) { strcpy(symboltype, "unknown"); }
        if (scope == GLOBAL_SCOPE) { strcpy(symbolscope, "global"); } 
        else if (scope == LOCAL_SCOPE) { strcpy(symbolscope, "local"); }
        fprintf(stderr, GREEN"[symbol table update]%s unknown symbol `%s` has been set to type %s.\n"RESET, symbolscope, sval, symboltype);
    }
    return;
}


void print_symboltableRecord(struct symboltableRecord* record){
    printf("[Record]");
    if (record->valid){
        printf("sval:%s, declared line:%d, scope:%c, valuetype:%c, ", record->sval, record->line, record->scope, record->valuetype);
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
                     printf("], indexed from %d, length:%d", record->value->ivallist_start, record->value->ivallistlength);
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
                     printf("), indexed from %d, length:%d", record->value->ivallist_start, record->value->ivallistlength);
                }else {
                     printf("value:uninitialized");
                }
                break;
            }
            case VALUETYPE_UNKNOWN:{
                if (record->value != NULL) { 
                    printf(RED"[symbol table error]`%s`, internal error, unknown valuetype has not null value %c"RESET, record->sval, record->valuetype);
                    exit(975);
                }else {
                     printf("value:uninitialized");
                }
                break;
            }
            default:
                printf(RED"[symbol table error]internal error, print reecord with unsupported valuetype %c"RESET, record->valuetype);
                exit(974);
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
    
    // global hello = 1; # line 1
    declare_symbol("hello", VALUETYPE_UNKNOWN, GLOBAL_SCOPE, 1, tb);
    set_symbol_type("hello", VALUETYPE_INT, GLOBAL_SCOPE, 1, tb);
    init_int_symbol("hello", GLOBAL_SCOPE, 1, tb);
    update_int_symbol("hello", GLOBAL_SCOPE, 10, 1, tb);
    print_symboltable(tb);

    // local myarray; # line 2
    declare_symbol("myarray", VALUETYPE_UNKNOWN, LOCAL_SCOPE, 2, tb);
    print_symboltable(tb);
    
    // array myarray[10..14] # line 3
    set_symbol_type("myarray", VALUETYPE_ARRAY, LOCAL_SCOPE, 3, tb);
    init_int_list_symbol("myarray", LOCAL_SCOPE, 10, 5, 3, tb);
    print_symboltable(tb);

    //myarray[11] = 3
    update_int_list_symbol_itemwise("myarray", LOCAL_SCOPE, 3, 11, 4, tb);
    print_symboltable(tb);

    //local mytuple = 1,2,3 # line 4
    declare_symbol("mytuple", VALUETYPE_UNKNOWN, LOCAL_SCOPE, 4, tb);
    set_symbol_type("mytuple", VALUETYPE_TUPLE, LOCAL_SCOPE, 4, tb);
    init_int_list_symbol("mytuple", LOCAL_SCOPE, 0, 3, 4, tb);
    int buffer[] = {1, 2, 3};
    int buffer_size = 3;
    for ( int i = 0; i < buffer_size; i++) {
        update_int_list_symbol_itemwise("mytuple", LOCAL_SCOPE, buffer[i], i, 4, tb);
    } 
    print_symboltable(tb);

    // remove hello # line 14
    remove_symbol("hello", GLOBAL_SCOPE, 14, tb);
    print_symboltable(tb);

}