#!/bin/bash

#[TEST CASE 1]
echo "----------------------------------------------------------------"
echo "
***This is a simple boris script to demonstrate the syntax of the boris programming language
global globalArg = 1024;
local c = 1;
local d = 2;

defun myFun(a,b)
    print a,b; ***debug and check out the args
    sum = a+b;
    return sum*globalArg;
end defun

print myFun (c,d);"

echo  "
***This is a simple boris script to demonstrate the syntax of the boris programming language
global globalArg = 1024;
local c = 1;
local d = 2;

defun myFun(a,b)
    print a,b; ***debug and check out the args
    sum = a+b;
    return sum*globalArg;
end defun

print myFun (c,d);"| ./boris_parser