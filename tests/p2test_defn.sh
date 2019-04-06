#!/bin/bash

#[TEST CASE] KW_DEFUN ID OP_LPAR ID comma_id_list OP_RPAR body KW_END KW_DEFUN 
echo "----------------------------------------------------------------"
echo "
defun myFun(a,b)
    print a,b;
    sum = a+b;
    return sum*sum;
end defun"
echo "
defun myFun(a,b)
    print a,b;
    sum = a+b;
    return sum*sum;
end defun" | ./boris_parser