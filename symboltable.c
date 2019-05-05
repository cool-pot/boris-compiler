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

struct symboltable* init_symboltable(int length, int scope){
    struct symboltable* tb = malloc(sizeof(struct symboltable));//TODO, Closed, will be free in remove_symboltable
    if(!tb) {
        fprintf(stderr, RED"[symbol table error]Out of space."RESET);
        exit(0);
    }
    if (scope != GLOBAL_SCOPE && scope != LOCAL_SCOPE){
        fprintf(stderr, RED"[symbol table error]internal error, unsupported scope: %d"RESET, scope);
        exit(997);
    }
    
    for (int i = 0; i < MAX_SYMBOLTABLE_SIZE; i++) { 
        // default setting for symboltableRecord
        struct symboltableRecord* record = malloc(sizeof(struct symboltableRecord));
        if (!record) {
            fprintf(stderr, RED"[symbol table error]Out of space."RESET);
            exit(0);
        }
        tb->records[i] = NULL;
        record->valid = 0;
        record->line = -1; 
        record->scope = -1;
        record->sval = NULL;
        record->value = NULL;
        record->valuetype = VALUETYPE_UNKNOWN;
        tb->records[i] = record;
    }
    tb->length = length;
    tb->scope = scope;
    return tb;
}

struct symboltableRecord* lookup_symbol(char* sval, int scope, struct symboltable* tb){
    if (scope != GLOBAL_SCOPE && scope != LOCAL_SCOPE){
        fprintf(stderr, RED"[symbol table error]internal error, unsupported scope: %d"RESET, scope);
        exit(997);
    } else if (tb == NULL || tb->length <= 0 ) {
        fprintf(stderr, RED"[symbol table error]internal error, invalid symbol table in lookup_symbol");
        exit(998);
    } 
    struct symboltableRecord* ret = NULL;
    struct symboltableRecord* p;
    for (int i = 0; i < tb->length; i++){
        p = tb->records[i];
        if((p!= NULL) && (p->scope == scope) && (strcmp(p->sval, sval) == 0) && (p->valid != 0)){ // only valid record will be returned
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
    if (tb == NULL || tb->length <= 0 ) {
        fprintf(stderr, RED"[symbol table error]internal error, invalid symbol table in next_available_symbol_slot"RESET);
        exit(998);
    }
    for (int i = 0; i < tb->length; i++){
        if(tb->records[i]->valid == 0){
            return i;
        }
    }
    return -1;
}

void declare_symbol(char* sval, int valuetype, int scope, int line, struct symboltable* tb){
    printf("debug call declare_symbol\n" );
    if (valuetype != VALUETYPE_FUNC && valuetype != VALUETYPE_ARRAY && valuetype != VALUETYPE_TUPLE && valuetype != VALUETYPE_INT && valuetype != VALUETYPE_UNKNOWN && valuetype != VALUETYPE_LINK_TO_GLOBAL){
        fprintf(stderr, RED"[symbol table error]internal error, bad value type"RESET);
        exit(994);
    }
    if (scope != LOCAL_SCOPE && valuetype == VALUETYPE_LINK_TO_GLOBAL) {
        fprintf(stderr, RED"[symbol table error]internal error, global link can only be declared in local scope"RESET);
        exit(999);
    }
    if (lookup_symbol(sval, scope, tb) != NULL) {
        fprintf(stderr, RED"[symbol table error]duplicated declare for %s in this scope"RESET, sval);
        exit(999);
    } 
    if (tb == NULL) {
        fprintf(stderr, RED"[symbol table error]empty symbol table for `%s`"RESET, sval);
        exit(999);
    }
    if (tb->scope != scope) {
        fprintf(stderr, RED"[symbol table error]`%s` symbol table scope conflicts"RESET, sval);
        exit(999);
    }
    int index = next_available_symbol_slot(tb);
    if (index == -1) {
        fprintf(stderr, RED"[symbol table error]symbol tabel overflow for %s , need more slots"RESET, sval);
        exit(995);
    }
    // declare a symbol here
    tb->records[index]->sval = sval;
    tb->records[index]->valuetype = valuetype;
    tb->records[index]->scope = scope;
    tb->records[index]->line = line;
    tb->records[index]->valid = 1;
    tb->records[index]->value = NULL;
    // verbose print
    if (SYMBOLTABLE_VERBOSE) {
        char symboltype[20];
        char symbolscope[7];
        if (valuetype == VALUETYPE_INT) { strcpy(symboltype, "int"); } 
        else if (valuetype == VALUETYPE_FUNC) { strcpy(symboltype, "func"); } 
        else if (valuetype == VALUETYPE_TUPLE) { strcpy(symboltype, "tuple"); } 
        else if (valuetype == VALUETYPE_ARRAY) { strcpy(symboltype, "array"); }
        else if (valuetype == VALUETYPE_UNKNOWN) { strcpy(symboltype, "unknown"); }
        else if (valuetype == VALUETYPE_LINK_TO_GLOBAL) { strcpy(symboltype, "link_to_global"); }
        if (scope == GLOBAL_SCOPE) { strcpy(symbolscope, "global"); } 
        else if (scope == LOCAL_SCOPE) { strcpy(symbolscope, "local"); }
        fprintf(stderr, GREEN"[symbol table decl symbol]declare %s %s symbol `%s` in line %d.\n"RESET, symbolscope, symboltype, sval, line);
    }

    return;
}

void init_int_symbol(char* sval, int scope, int line, struct symboltable* tb){
    printf("call init_int_symbol\n");
    struct symboltableRecord* r = lookup_symbol(sval, scope, tb);
    if (r == NULL) {
        fprintf(stderr, RED"[symbol table error]update a undeclared symbol %s in this scope is not valid"RESET, sval);
        exit(993);
    }
    if (r->valuetype != VALUETYPE_INT) {
        fprintf(stderr, RED"[symbol table error]wrong type, can't init a int value to non-int symbol %s in this scope"RESET, sval);
        exit(992);
    }
    if (r->value != NULL) {
        fprintf(stderr, RED"[symbol table error]init a non-empty symbol `%s` in this scope"RESET, sval);
        exit(992);
    }
    printf("debug value not ok\n");
    // init the symbol value here
    struct symboltableRecordValue* value = malloc(sizeof(struct symboltableRecordValue)); //TODO Closed. will be free in remove_symbol.
    if(!value) {
        fprintf(stderr, RED"[symbol table error]Out of space."RESET);
        exit(0);
    }
    printf("debug value ok\n");
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
    if (ivallistlength > MAX_TUPLE_ARRAY_SIZE) {
        fprintf(stderr, RED"[symbol table error]%s, too long, unsupported"RESET, sval);
        exit(991);
    }
    // init the symbol value here
    struct symboltableRecordValue* value = malloc(sizeof(struct symboltableRecordValue)); //TODO Closed. will be free in remove_symbol.
    if(!value) {
        fprintf(stderr, RED"[symbol table error]Out of space."RESET);
        exit(0);
    }
    for (int i = 0; i < MAX_TUPLE_ARRAY_SIZE; i++) {
        value->ivallist[i] = 0;
    }
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

void init_func_symbol(char* sval, int scope, int formal_parameter_valuetype, int return_valuetype, struct pNode* defnnode, int line, struct symboltable* tb){
    struct symboltableRecord* r = lookup_symbol(sval, scope, tb);
    if (r == NULL) {
        fprintf(stderr, RED"[symbol table error]update a undeclared symbol %s in this scope is not valid"RESET, sval);
        exit(993);
    }
    if (r->valuetype != VALUETYPE_FUNC) {
        fprintf(stderr, RED"[symbol table error]wrong type, can't init a func value to non-func symbol %s in this scope"RESET, sval);
        exit(992);
    }
    // init the symbol function here
    struct symboltableRecordFunction* function = malloc(sizeof(struct symboltableRecordFunction)); //TODO Closed. will be free in remove_symbol.
    if(!function) {
        fprintf(stderr, RED"[symbol table error]Out of space."RESET);
        exit(0);
    }
    function->defnnode = defnnode;
    function->formal_parameter_valuetype = formal_parameter_valuetype;
    function->return_valuetype = return_valuetype;
    r->value = (struct symboltableRecordValue*)function;
    // verbose print
    if (SYMBOLTABLE_VERBOSE) {
        char symbolscope[7];
        if (scope == GLOBAL_SCOPE) { strcpy(symbolscope, "global"); } 
        else if (scope == LOCAL_SCOPE) { strcpy(symbolscope, "local"); }
        fprintf(stderr, GREEN"[symbol table update]init %s function symbol `%s` in line %d.\n"RESET, symbolscope, sval, line);
    }
    return;
}

void remove_symbol(char* sval, int scope, int line, struct symboltable* tb){
    if (scope != GLOBAL_SCOPE && scope != LOCAL_SCOPE){
        fprintf(stderr, RED"[symbol table error]internal error, unsupported scope: %d"RESET, scope);
        exit(997);
    } else if (tb == NULL || tb->length <= 0 ) {
        fprintf(stderr, RED"[symbol table error]internal error, invalid symbol table in remove_symbol"RESET);
        exit(998);
    }
    struct symboltableRecord* record = lookup_symbol(sval, scope, tb);
    if (record != NULL) {
        record->valid = 0;
        if (record->value != NULL) { 
            switch (record->valuetype) {
                case VALUETYPE_LINK_TO_GLOBAL: //should not happend actually
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
            char symboltype[20];
            char symbolscope[7];
            if (valuetype == VALUETYPE_INT) { strcpy(symboltype, "int"); } 
            else if (valuetype == VALUETYPE_FUNC) { strcpy(symboltype, "func"); } 
            else if (valuetype == VALUETYPE_TUPLE) { strcpy(symboltype, "tuple"); } 
            else if (valuetype == VALUETYPE_ARRAY) { strcpy(symboltype, "array"); }
            else if (valuetype == VALUETYPE_UNKNOWN) { strcpy(symboltype, "unknown"); }
            else if (valuetype == VALUETYPE_LINK_TO_GLOBAL) { strcpy(symboltype, "link_to_global"); }
            if (scope == GLOBAL_SCOPE) { strcpy(symbolscope, "global"); } 
            else if (scope == LOCAL_SCOPE) { strcpy(symbolscope, "local"); }
            fprintf(stderr, GREEN"[symbol table remove symbol]remove %s %s symbol `%s` in line %d.\n"RESET, symbolscope, symboltype, sval, line);
        }
        //make it invalid
        record->valid  = 0;
    }
    return;
}

void set_symbol_type(char* sval, int valuetype, int scope, int line, struct symboltable* tb){
    printf("debug call set_symbol_type\n" );
    if (valuetype != VALUETYPE_FUNC && valuetype != VALUETYPE_ARRAY && valuetype != VALUETYPE_TUPLE && valuetype != VALUETYPE_INT){
        fprintf(stderr, RED"[symbol table error]internal error, bad value type for set_symbol_type"RESET);
        exit(994);
    }
    struct symboltableRecord* record = lookup_symbol(sval, scope, tb);
    if (record == NULL) {
        fprintf(stderr, RED"[symbol table error]symbol `%s` not found in this scope"RESET, sval);
        exit(955);
    }
    if (record->valuetype != VALUETYPE_UNKNOWN) {
        fprintf(stderr, RED"[symbol table error]can't reset type for `%s`"RESET, sval);
        exit(932);
    } 
    // set symbol type here
    record->valuetype = valuetype;
    // verbose print
    if (SYMBOLTABLE_VERBOSE) {
        char symboltype[20];
        char symbolscope[7];
        if (valuetype == VALUETYPE_INT) { strcpy(symboltype, "int"); } 
        else if (valuetype == VALUETYPE_FUNC) { strcpy(symboltype, "func"); } 
        else if (valuetype == VALUETYPE_TUPLE) { strcpy(symboltype, "tuple"); } 
        else if (valuetype == VALUETYPE_ARRAY) { strcpy(symboltype, "array"); }
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
            case VALUETYPE_LINK_TO_GLOBAL:{
                if (record->value != NULL) { 
                    printf(RED"[symbol table error]`%s`, internal error, link_to_global valuetype has not null value %c"RESET, record->sval, record->valuetype);
                    exit(975);
                }else {
                     printf("value:link_to_global");
                }
                break;
            }
            case VALUETYPE_FUNC:{
                if (record->value == NULL) { 
                    printf(RED"[symbol table error]`%s`, internal error, func valuetype has null value %c"RESET, record->sval, record->valuetype);
                    exit(975);
                } else{
                    struct symboltableRecordFunction* function = (struct symboltableRecordFunction*) record->value;
                    printf("formal_parameter: `%c`, return: `%c`", function->formal_parameter_valuetype, function->return_valuetype);
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
    if (tb != NULL){
        printf("> SymbolTable[length: %d]\n", tb->length);
        for( int i = 0; i < tb->length; i++){
            print_symboltableRecord(tb->records[i]);
        }
    } else {
        printf("> Empty SymbolTable\n");
    }
    return;
}

struct symboltableStack* init_symboltableStack(int capacity){
    struct symboltableStack* tbstk = malloc(sizeof(struct symboltableStack));//TODO, Closed, free in remove_symboltableStack
    if(!tbstk) {
        fprintf(stderr, RED"[symbol table error]Out of space."RESET);
        exit(0);
    }
    tbstk->capacity = capacity;
    tbstk->current_length = 0;
    for (int i = 0; i < capacity; i++){
        tbstk->tbptrs[i] = NULL;
    }
    return tbstk;
}

void remove_symboltable(struct symboltable* tb){
    if(tb!= NULL){
        for (int i = 0; i < MAX_SYMBOLTABLE_SIZE; i++){
            free(tb->records[i]);
        }
        free(tb);
    }
    return;
}

void remove_symboltableStack(struct symboltableStack* tbstk){
    if(tbstk!= NULL){
        free(tbstk);
    }
    return;
}

void pop_symboltableStack(struct symboltableStack* tbstk){
    if (tbstk != NULL){
        if (tbstk->current_length <= 0) {
            fprintf(stderr, RED"[symbol table error]can't pop empty symboltable stack"RESET);
            exit(999);
        }
        struct symboltable* tb_to_delete = tbstk->tbptrs[tbstk->current_length-1];
        remove_symboltable(tb_to_delete);
        tbstk->current_length -= 1;
    }
    return;
}

void push_symboltableStack(struct symboltable* tb, struct symboltableStack* tbstk){
    if (tbstk != NULL && tb != NULL){
        if (tbstk->current_length >= tbstk->capacity) {
            fprintf(stderr, RED"[symbol table error]can't push full symboltable stack"RESET);
            exit(999);
        }
        tbstk->tbptrs[tbstk->current_length] = tb;
        tbstk->current_length = tbstk->current_length + 1;
    }
    return;
}

struct symboltable* top_symboltableStack(struct symboltableStack* tbstk){
    if (tbstk != NULL && tbstk->current_length > 0){
        return tbstk->tbptrs[tbstk->current_length-1];
    }
    return NULL;
}

void print_symboltableStack(struct symboltableStack* tbstk){
    if (tbstk != NULL){
        printf(">> SymbolTable Stack[current_length: %d]>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n", tbstk->current_length);
        for( int i = 0; i < tbstk->current_length; i++){
            print_symboltable(tbstk->tbptrs[i]);
        }
    } else {
        printf("> Empty SymbolTable Stack\n");
    }
    return;
}

void symbolTableTester(){
    struct symboltable* tb = init_symboltable(3, GLOBAL_SCOPE);
    struct symboltableStack* tbstk = init_symboltableStack(2);
    
    
    // global hello = 1; # line 1
    declare_symbol("hello", VALUETYPE_UNKNOWN, GLOBAL_SCOPE, 1, tb);
    
    set_symbol_type("hello", VALUETYPE_INT, GLOBAL_SCOPE, 1, tb);
    init_int_symbol("hello", GLOBAL_SCOPE, 1, tb);
    update_int_symbol("hello", GLOBAL_SCOPE, 10, 1, tb);
    print_symboltable(tb);
    
    // global myarray; # line 2
    declare_symbol("myarray", VALUETYPE_UNKNOWN, GLOBAL_SCOPE, 2, tb);
    print_symboltable(tb);
    
    
    // array myarray[10..14] # line 3
    set_symbol_type("myarray", VALUETYPE_ARRAY, GLOBAL_SCOPE, 3, tb);
    init_int_list_symbol("myarray", GLOBAL_SCOPE, 10, 5, 3, tb);
    print_symboltable(tb);

    //myarray[11] = 3
    update_int_list_symbol_itemwise("myarray", GLOBAL_SCOPE, 3, 11, 4, tb);
    print_symboltable(tb);

    
    //global mytuple = 1,2,3 # line 4
    declare_symbol("mytuple", VALUETYPE_UNKNOWN, GLOBAL_SCOPE, 4, tb);
    set_symbol_type("mytuple", VALUETYPE_TUPLE, GLOBAL_SCOPE, 4, tb);
    init_int_list_symbol("mytuple", GLOBAL_SCOPE, 0, 3, 4, tb);
    int buffer[] = {1, 2, 3};
    int buffer_size = 3;
    for ( int i = 0; i < buffer_size; i++) {
        update_int_list_symbol_itemwise("mytuple", GLOBAL_SCOPE, buffer[i], i, 4, tb);
    } 
    print_symboltable(tb);

    // remove hello # line 5
    remove_symbol("hello", GLOBAL_SCOPE, 5, tb);
    print_symboltable(tb);
    
    // f(arg1, g(arg2), arg3) #line 6
    //call a function, will push a new table
    struct symboltable* new_tb = init_symboltable(3, LOCAL_SCOPE);
    push_symboltableStack(new_tb, tbstk);
    declare_symbol("arg1", VALUETYPE_ARRAY, LOCAL_SCOPE, 6, top_symboltableStack(tbstk));
    
    struct symboltable* newnew_tb = init_symboltable(3, LOCAL_SCOPE);
    push_symboltableStack(newnew_tb, tbstk);
    declare_symbol("arg2", VALUETYPE_INT, LOCAL_SCOPE, 6, top_symboltableStack(tbstk));
    //print_symboltable(top_symboltableStack(tbstk));
    print_symboltableStack(tbstk);

    pop_symboltableStack(tbstk);
    declare_symbol("arg3", VALUETYPE_LINK_TO_GLOBAL, LOCAL_SCOPE, 6, top_symboltableStack(tbstk));
    print_symboltable(top_symboltableStack(tbstk));
    
}
