/*
* LLVM code generators for the boris compiler
*
* Author: Yi Zhou
* May 5, 2019
*
*/

# include <stdio.h>
# include <string.h>
# include "boris.h"



// Generates an LLVM value object for a NODETYPE_INT.
// Returns an LLVM value reference.
LLVMValueRef boris_codegen_int(struct pNode* node){
    struct iNode* inode = (struct iNode*) node;
    return LLVMConstReal(LLVMInt32Type(), inode->ival);
}

// Generates an LLVM value object for NODETYPE_EXPR_PLUS_EXPR.
// Returns an LLVM value reference.
LLVMValueRef boris_codegen_expr_plus_expr(struct pNode* node, LLVMBuilderRef builder,  LLVMModuleRef module){
    LLVMValueRef left =  boris_codegen(node->childs[0], builder, module);
    LLVMValueRef right =  boris_codegen(node->childs[2], builder, module);
    LLVMValueRef expr_plus_expr_tmp = LLVMBuildAdd(builder, left, right, "expr_plus_expr_tmp");
    return expr_plus_expr_tmp;
}


// Recursively generates LLVM objects to build the code.
// Returns an LLVM value reference.
LLVMValueRef boris_codegen(struct pNode *node,  LLVMBuilderRef builder, LLVMModuleRef module)
{
    // Recursively free dependent data.
    switch(node->pnodetype) {
        case NODETYPE_INT: {
            return boris_codegen_int(node);
        }
        case NODETYPE_EXPR_PLUS_EXPR: {
            return boris_codegen_expr_plus_expr(node, builder, module);
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

    // builtin module utility
    // extern i32 @putchar(i32)
    LLVMTypeRef putcharArgsTyList[] = { LLVMInt32Type() };
    LLVMTypeRef putcharTy = LLVMFunctionType(
        LLVMInt32Type(), putcharArgsTyList, 1, 0
    );
    LLVMValueRef putchar =LLVMAddFunction(module, "putchar", putcharTy);
    
    /// void main(void)
    LLVMTypeRef MainFunctionTy = LLVMFunctionType(
        LLVMVoidType(), NULL, 0, 0
    );
    LLVMValueRef MainFunction = LLVMAddFunction(module, "main", MainFunctionTy);
    LLVMBasicBlockRef MainEntry = LLVMAppendBasicBlock(MainFunction, "MainEntry");
    LLVMPositionBuilderAtEnd(builder, MainEntry);
    //print > Hello From LLVM
    char* message = "> Hello from compiled boris-LLVM object\n";
    boris_codegen_message(message, strlen(message), builder, module);
    

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
    char *error = NULL;
    //LLVMVerifyModule(module, LLVMAbortProcessAction, &error);
    LLVMDisposeMessage(error);
    LLVMWriteBitcodeToFile(module, "a.bc");
}
