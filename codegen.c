/*
* LLVM code generators for the boris compiler
*
* Author: Yi Zhou
* May 5, 2019
*
*/

# include <stdio.h>
# include "boris.h"
# include <llvm-c/Core.h>
# include <llvm-c/Analysis.h>



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