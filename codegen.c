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
    LLVMValueRef ret = LLVMBuildAlloca(builder, LLVMInt32Type(), "");
    LLVMBuildStore(builder, LLVMBuildAdd(builder, left, right, ""), ret);
    return LLVMBuildLoad(builder, ret, "expr_plus_expr_tmp");
}

// Generates an LLVM value object for NODETYPE_EXPR_MINUS_EXPR.
// Returns an LLVM value reference.
LLVMValueRef boris_codegen_expr_minus_expr(struct pNode* node, LLVMBuilderRef builder,  LLVMModuleRef module, \
                                        struct symboltable* global_tb, struct symboltableStack* local_tbstk){
    LLVMValueRef left =  boris_codegen_expr(node->childs[0], builder, module, global_tb, local_tbstk);
    LLVMValueRef right =  boris_codegen_expr(node->childs[2], builder, module, global_tb, local_tbstk);
    LLVMValueRef ret = LLVMBuildAlloca(builder, LLVMInt32Type(), "");
    LLVMBuildStore(builder, LLVMBuildSub(builder, left, right, ""), ret);
    return LLVMBuildLoad(builder, ret, "expr_minus_expr_tmp");
}


// Generates an LLVM value object for NODETYPE_EXPR_DIV_EXPR.
// Returns an LLVM value reference.
LLVMValueRef boris_codegen_expr_div_expr(struct pNode* node, LLVMBuilderRef builder,  LLVMModuleRef module, \
                                        struct symboltable* global_tb, struct symboltableStack* local_tbstk){
    LLVMValueRef left =  boris_codegen_expr(node->childs[0], builder, module, global_tb, local_tbstk);
    LLVMValueRef right =  boris_codegen_expr(node->childs[2], builder, module, global_tb, local_tbstk);
    LLVMValueRef ret = LLVMBuildAlloca(builder, LLVMInt32Type(), "");
    LLVMBuildStore(builder, LLVMBuildUDiv(builder, left, right, ""), ret);
    return LLVMBuildLoad(builder, ret, "expr_div_expr_tmp");
}


// Generates an LLVM value object for NODETYPE_EXPR_MULT_EXPR.
// Returns an LLVM value reference.
LLVMValueRef boris_codegen_expr_mult_expr(struct pNode* node, LLVMBuilderRef builder,  LLVMModuleRef module, \
                                        struct symboltable* global_tb, struct symboltableStack* local_tbstk){
    LLVMValueRef left =  boris_codegen_expr(node->childs[0], builder, module, global_tb, local_tbstk);
    LLVMValueRef right =  boris_codegen_expr(node->childs[2], builder, module, global_tb, local_tbstk);
    LLVMValueRef ret = LLVMBuildAlloca(builder, LLVMInt32Type(), "");
    LLVMBuildStore(builder, LLVMBuildMul(builder, left, right, ""), ret);
    return LLVMBuildLoad(builder, ret, "expr_mult_expr_tmp");
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
            if (LOCAL_ENV){ // look at local table first if in local env. 
                struct symboltable* local_tb =  top_symboltableStack(local_tbstk);
                struct symboltableRecord* record = lookup_symbol(snode->sval, LOCAL_SCOPE, local_tb);
                if (record->valuetype == VALUETYPE_INT) {
                    LLVMValueRef ret = record->value->address;
                    return LLVMBuildLoad(builder, ret, "read_local_tb_int_temp");
                } else if (record->valuetype == VALUETYPE_LINK_TO_GLOBAL) {
                    break;
                } else {
                    printf("codegen. no implementation\n");
                    exit(666);
                }
            }
            //look at global table.
            struct symboltableRecord* record = lookup_symbol(snode->sval, GLOBAL_SCOPE, global_tb);
            if (record->valuetype == VALUETYPE_INT) {
                LLVMValueRef ret = record->value->address;
                return LLVMBuildLoad(builder, ret, "read_global_tb_int_temp");
            } else {
                printf("codegen. no implementation\n");
                exit(666);
            }
        }
    }
    return NULL;
}

// generate a list of putchar call
void boris_codegen_message(char* message, int length, LLVMBuilderRef builder, LLVMModuleRef module){
    LLVMValueRef putchar = LLVMGetNamedFunction(module, "putchar");
    for (int i = 0; i < length; i++){
        LLVMValueRef charactor = LLVMConstInt(LLVMInt32Type(), message[i], 0);
        LLVMValueRef putcharArgs[] = { charactor }; 
        LLVMBuildCall(
            builder,
            putchar,
            putcharArgs,
            1,
            ""
        );
    }
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
    //ugly hack, not safe, intend to handle empty input
    if (LLVMCountBasicBlocks(MainFunction) != 1) {
        LLVMBasicBlockRef EndEntry = LLVMAppendBasicBlock(MainFunction, "EndEntry");
        LLVMPositionBuilderAtEnd(builder, EndEntry);
    } else {
        LLVMBasicBlockRef EndEntry = LLVMGetLastBasicBlock(MainFunction);
        LLVMPositionBuilderAtEnd(builder, EndEntry);
    }
    LLVMBuildRetVoid(builder);
}


// handle the generated LLVM module before output
void verify_llvm_module_and_output(LLVMModuleRef module){
    char *verify_error = NULL;
    char *print_error = NULL;
    LLVMVerifyModule(module, LLVMAbortProcessAction, &verify_error);
    LLVMDisposeMessage(verify_error);
    LLVMWriteBitcodeToFile(module, "a.bc");
    LLVMPrintModuleToFile(module, "a.ll", &print_error);
    LLVMDisposeMessage(print_error);
}
