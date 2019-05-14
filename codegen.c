/*
* LLVM code generators for the boris compiler
*
* Author: Yi Zhou
* May 5, 2019
*
*/

# include <stdio.h>
# include <stdlib.h>
# include <stdarg.h>
# include <string.h>
# include "boris.h"

// The strlen and printf for llvm-c implementation is 
// from https://github.com/0vercl0k/stuffz/blob/master/llvm-funz/llvm-c-frontend-playing-with-ir.c
LLVMValueRef create_strlen_function(LLVMModuleRef *Module)
{
    LLVMValueRef Zero8 = LLVMConstInt(LLVMInt8Type(), 0, 0);
    LLVMValueRef Zero32 = LLVMConstInt(LLVMInt32Type(), 0, 0);
    LLVMValueRef One32 = LLVMConstInt(LLVMInt32Type(), 1, 0);

    LLVMBuilderRef Builder = LLVMCreateBuilder();

    /// 1. int strlen(char *);
    LLVMTypeRef StrlenArgsTyList[] = { LLVMPointerType(LLVMInt8Type(), 0) };
    LLVMTypeRef StrlenTy = LLVMFunctionType(
        LLVMInt32Type(),
        StrlenArgsTyList,
        1,
        0
    );

    LLVMValueRef StrlenFunction = LLVMAddFunction(*Module, "strlen", StrlenTy);
    LLVMValueRef s = LLVMGetParam(StrlenFunction, 0);
    LLVMSetValueName(s, "s");

    LLVMBasicBlockRef InitBasicBlock = LLVMAppendBasicBlock(StrlenFunction, "init");
    LLVMBasicBlockRef CheckBasicBlock = LLVMAppendBasicBlock(StrlenFunction, "check");
    LLVMBasicBlockRef BodyBasicBlock = LLVMAppendBasicBlock(StrlenFunction, "body");
    LLVMBasicBlockRef EndBasicBlock = LLVMAppendBasicBlock(StrlenFunction, "end");

    LLVMPositionBuilderAtEnd(Builder, InitBasicBlock);
    /// 2. int i = 0;
    LLVMValueRef i = LLVMBuildAlloca(Builder, LLVMInt32Type(), "i");
    LLVMBuildStore(Builder, Zero32, i);

    LLVMBuildBr(Builder, CheckBasicBlock);

    /// 3. check:
    LLVMPositionBuilderAtEnd(Builder, CheckBasicBlock);
    /// 4. if(s[i] == 0)
    LLVMValueRef id_if[] = { LLVMBuildLoad(Builder, i, "") };
    LLVMValueRef If = LLVMBuildICmp(
        Builder,
        LLVMIntNE,
        Zero8,
        LLVMBuildLoad(
            Builder,
            LLVMBuildGEP(Builder, s, id_if, 1, ""),
            ""
        ),
        ""
    );

    /// 5. goto end;
    LLVMBuildCondBr(Builder, If, BodyBasicBlock, EndBasicBlock);

    /// 6. body:
    LLVMPositionBuilderAtEnd(Builder, BodyBasicBlock);
    /// 7. i += 1;
    LLVMBuildStore(
        Builder,
        LLVMBuildAdd(
            Builder,
            LLVMBuildLoad(
                Builder,
                i,
                ""
            ),
            One32,
            ""
        ),
        i
    );

    /// 8. goto check;
    LLVMBuildBr(Builder, CheckBasicBlock);

    /// 9. end:
    LLVMPositionBuilderAtEnd(Builder, EndBasicBlock);
    /// 10. return i;
    LLVMBuildRet(Builder, LLVMBuildLoad(Builder, i, ""));

    return StrlenFunction;
}

// Generates an LLVM value object for a NODETYPE_INT.
// Returns an LLVM value reference.
LLVMValueRef boris_codegen_int(struct pNode* node){
    struct iNode* inode = (struct iNode*) node;
    return LLVMConstInt(LLVMInt32Type(), inode->ival, 0);
}

// Generates an LLVM value object for NODETYPE_EXPR_PLUS_EXPR.
// Returns an LLVM value reference.
LLVMValueRef boris_codegen_expr_plus_expr(struct pNode* node, LLVMBuilderRef builder,  LLVMModuleRef module, \
                                        struct symboltable* global_tb, struct symboltableStack* local_tbstk){
    LLVMValueRef left =  boris_codegen_expr(node->childs[0], builder, module, global_tb, local_tbstk);
    LLVMValueRef right =  boris_codegen_expr(node->childs[2], builder, module, global_tb, local_tbstk);
    return LLVMBuildAdd(builder, left, right, "expr_plus_expr_tmp");
}

// Generates an LLVM value object for NODETYPE_EXPR_MINUS_EXPR.
// Returns an LLVM value reference.
LLVMValueRef boris_codegen_expr_minus_expr(struct pNode* node, LLVMBuilderRef builder,  LLVMModuleRef module, \
                                        struct symboltable* global_tb, struct symboltableStack* local_tbstk){
    LLVMValueRef left =  boris_codegen_expr(node->childs[0], builder, module, global_tb, local_tbstk);
    LLVMValueRef right =  boris_codegen_expr(node->childs[2], builder, module, global_tb, local_tbstk);
    return LLVMBuildSub(builder, left, right, "expr_minus_expr_tmp");
}


// Generates an LLVM value object for NODETYPE_EXPR_DIV_EXPR.
// Returns an LLVM value reference.
LLVMValueRef boris_codegen_expr_div_expr(struct pNode* node, LLVMBuilderRef builder,  LLVMModuleRef module, \
                                        struct symboltable* global_tb, struct symboltableStack* local_tbstk){
    LLVMValueRef left =  boris_codegen_expr(node->childs[0], builder, module, global_tb, local_tbstk);
    LLVMValueRef right =  boris_codegen_expr(node->childs[2], builder, module, global_tb, local_tbstk);
    return LLVMBuildUDiv(builder, left, right, "expr_div_expr_tmp");
}


// Generates an LLVM value object for NODETYPE_EXPR_MULT_EXPR.
// Returns an LLVM value reference.
LLVMValueRef boris_codegen_expr_mult_expr(struct pNode* node, LLVMBuilderRef builder,  LLVMModuleRef module, \
                                        struct symboltable* global_tb, struct symboltableStack* local_tbstk){
    LLVMValueRef left =  boris_codegen_expr(node->childs[0], builder, module, global_tb, local_tbstk);
    LLVMValueRef right =  boris_codegen_expr(node->childs[2], builder, module, global_tb, local_tbstk);
    return  LLVMBuildMul(builder, left, right, "expr_mul_expr_tmp");
}


// Recursively generates LLVM objects to build the code.
// Returns an LLVM value reference.
LLVMValueRef boris_codegen_expr(struct pNode *node,  LLVMBuilderRef builder, LLVMModuleRef module, \
                                        struct symboltable* global_tb, struct symboltableStack* local_tbstk){
    // Recursively generate a value refenrence for expr
    switch(node->pnodetype) {
        case NODETYPE_INT: {
            return boris_codegen_int(node);
        }
        case NODETYPE_SINGLE_INT_AS_EXPR:{
            return boris_codegen_int(node->childs[0]);
        }
        case NODETYPE_EXPR_PLUS_EXPR: {
            return boris_codegen_expr_plus_expr(node, builder, module, global_tb, local_tbstk);
        }
        case NODETYPE_EXPR_MINUS_EXPR: {
            return boris_codegen_expr_minus_expr(node, builder, module, global_tb, local_tbstk);
        }
        case NODETYPE_EXPR_MULT_EXPR: {
            return boris_codegen_expr_mult_expr(node, builder, module, global_tb, local_tbstk);
        }
        case NODETYPE_EXPR_DIV_EXPR: {
            return boris_codegen_expr_div_expr(node, builder, module, global_tb, local_tbstk);
        }
        case NODETYPE_LPAR_EXPR_RPAR: {
            return boris_codegen_expr(node->childs[1], builder, module, global_tb, local_tbstk);
        }
        case NODETYPE_SINGLE_ID_AS_EXPR:{
            struct sNode* snode = (struct sNode*)node->childs[0];
            struct symboltable* matched_tb = get_matched_symboltable(snode->sval, global_tb, local_tbstk);
            struct symboltableRecord* record = lookup_symbol(snode->sval, matched_tb->scope, matched_tb);
            if (record->valuetype == VALUETYPE_INT) {
                return LLVMBuildLoad(builder, record->value->address, "load_singleid_int_temp");
            } else {
                printf("codegen. not allowed id type\n");
                exit(666);
            }
        }
        case NODETYPE_ARRAY_REF_AS_EXPR:{
            struct sNode* snode = (struct sNode*)node->childs[0];
            struct pNode* exprnode = node->childs[2];
            struct symboltable* matched_tb = get_matched_symboltable(snode->sval, global_tb, local_tbstk);
            struct symboltableRecord* record = lookup_symbol(snode->sval, matched_tb->scope, matched_tb);
            LLVMValueRef array_address = record->value->address;
            LLVMValueRef offset = LLVMBuildSub(builder, boris_codegen_expr(exprnode, builder, module, global_tb, local_tbstk), LLVMConstInt(LLVMInt32Type(), record->value->ivallist_start, 0), ""); // array access should consider its start point
            LLVMValueRef indices[] = { offset };
            LLVMValueRef element_address = LLVMBuildGEP(builder, array_address, indices, 1, "");
            return LLVMBuildLoad(builder, element_address, "load_array_ref_int_temp");
        }
        case NODETYPE_TUPLE_REF_AS_EXPR:{
            struct sNode* snode = (struct sNode*)node->childs[0];
            struct pNode* exprnode = node->childs[2];
            struct symboltable* matched_tb = get_matched_symboltable(snode->sval, global_tb, local_tbstk);
            struct symboltableRecord* record = lookup_symbol(snode->sval, matched_tb->scope, matched_tb);
            LLVMValueRef tuple_address = record->value->address;
            LLVMValueRef offset = boris_codegen_expr(exprnode, builder, module, global_tb, local_tbstk);
            LLVMValueRef indices[] = { offset };
            LLVMValueRef element_address = LLVMBuildGEP(builder, tuple_address, indices, 1, "");
            return LLVMBuildLoad(builder, element_address, "load_tuple_ref_int_temp");
        }
        case NODETYPE_FUNC_CALL_AS_EXPR: {
            struct sNode* func_name = (struct sNode*)node->childs[0];
            struct pNode* exprnode = node->childs[1];
            struct symboltableRecord* func_record = lookup_symbol(func_name->sval, GLOBAL_SCOPE, global_tb);
            struct symboltableRecordFunction* func_value = (struct symboltableRecordFunction*)func_record->value;
            if (func_value->formal_parameter_valuetype == VALUETYPE_INT && func_value->return_valuetype == VALUETYPE_INT){
                LLVMValueRef Function = LLVMGetNamedFunction(module, func_name->sval);
                LLVMValueRef FunctionArgs[] = { boris_codegen_expr(exprnode, builder, module, global_tb, local_tbstk) };
                LLVMValueRef ret = LLVMBuildCall(
                    builder,
                    Function,
                    FunctionArgs,
                    1,
                    ""
                );
                return ret;
            } else {
                printf("no implementation - func call\n" );
            }  
        }
    }
    return NULL;
}


// customized llvm module for boris language
void begin_boris_module(LLVMBuilderRef builder,LLVMModuleRef module){
    //set target (= "x86_64-pc-linux-gnu")
    char *triple = LLVMGetDefaultTargetTriple();
    LLVMSetTarget(module, triple);

    // builtin module print utility
    
    /// define strlen(char*, ...)
    create_strlen_function(&module);
    
    /// extern int printf(char*, ...)
    LLVMTypeRef PrintfArgsTyList[] = { LLVMPointerType(LLVMInt8Type(), 0) };
    LLVMTypeRef PrintfTy = LLVMFunctionType(
        LLVMInt32Type(),
        PrintfArgsTyList,
        0,
        1 // IsVarArg
    );
    LLVMAddFunction(module, "printf", PrintfTy);
    
    
    /// void main(void)
    LLVMTypeRef MainFunctionTy = LLVMFunctionType(
        LLVMVoidType(), NULL, 0, 0
    );
    LLVMValueRef MainFunction = LLVMAddFunction(module, "main", MainFunctionTy);
    LLVMBasicBlockRef MainEntry = LLVMAppendBasicBlock(MainFunction, "MainEntry");
    LLVMPositionBuilderAtEnd(builder, MainEntry);
    //print > Hello From LLVM
    //char* message = "> Hello from compiled boris-LLVM object\n";
    //boris_codegen_message(message, strlen(message), builder, module);
}

// wrap up llvm module for boris language
void end_boris_module(LLVMBuilderRef builder,LLVMModuleRef module){
    LLVMValueRef MainFunction = LLVMGetNamedFunction(module, "main");
    LLVMBasicBlockRef EndEntry = LLVMAppendBasicBlock(MainFunction, "EndEntry");
    LLVMBuildBr(builder, EndEntry);
    LLVMPositionBuilderAtEnd(builder, EndEntry);
    LLVMBuildRetVoid(builder);
}


// handle the generated LLVM module before output
void verify_llvm_module_and_output(LLVMModuleRef module){
    char *verify_error = NULL;
    char *print_error = NULL;
    LLVMVerifyModule(module, LLVMAbortProcessAction, &verify_error);
    LLVMDisposeMessage(verify_error);
    LLVMWriteBitcodeToFile(module, "a.o");
    LLVMPrintModuleToFile(module, "a.ll", &print_error);
    LLVMDisposeMessage(print_error);
}
