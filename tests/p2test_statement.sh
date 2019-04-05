#!/bin/bash

#[TEST CASE] statement: lhs OP_ASSIGN expr OP_SEMI 
echo "----------------------------------------------------------------"
echo "a,b = b,a;" 
echo "a,b = b,a;" | ./boris_parser

#[TEST CASE] statement: lhs OP_EXCHANGE lhs OP_SEMI 
echo "----------------------------------------------------------------"
echo "a <-> b;" 
echo "a <-> b;" | ./boris_parser

#[TEST CASE] statement: KW_WHILE bool_expr KW_DO statement_list KW_END KW_WHILE 
echo "----------------------------------------------------------------"
echo "
while i < count 
    do a = a+b; 
       b = constant; 
end while" 
echo "
while i < count 
    do a = a+b; 
       b = constant; 
end while"  | ./boris_parser

#[TEST CASE] statement: KW_IF bool_expr KW_THEN statement_list elsif_sentence_list KW_END KW_IF
echo "----------------------------------------------------------------"
echo "
if a < b 
    then return a; 
end if"
echo "
if a < b 
    then return a; 
end if"  | ./boris_parser

echo "----------------------------------------------------------------"
echo "
if a < b 
    then return a; 
elsif a > b
    then return b;
elsif a == b
    then return c;
end if"
echo "
if a < b 
    then return a; 
elsif a > b
    then return b;
elsif a == b
    then return c;
end if"  | ./boris_parser

#[TEST CASE] statement: KW_IF bool_expr KW_THEN statement_list elsif_sentence_list else_sentence KW_END KW_IF 
echo "----------------------------------------------------------------"
echo "
if a < b 
    then return a; 
elsif a > b
    then return b;
else 
    return c;
end if"
echo "
if a < b 
    then return a; 
elsif a > b
    then return b;
else 
    return c;
end if"  | ./boris_parser

#[TEST CASE] statement: KW_FOREACH ID KW_IN range KW_DO statement_list KW_END KW_FOR
echo "----------------------------------------------------------------"
echo "
foreach i in zero..max
    do sum = sum + i;
       count = count * i;
end for
"
echo "
foreach i in zero..max
    do sum = sum + i;
       count = count * i;
end for
"  | ./boris_parser

#[TEST CASE] statement: KW_FOREACH ID KW_IN array_id KW_DO statement_list KW_END KW_FOR
echo "----------------------------------------------------------------"
echo "
foreach i in myarray
    do sum = sum + i;
       count = count * i;
end for
"
echo "
foreach i in myarray
    do sum = sum + i;
       count = count * i;
end for
"  | ./boris_parser

#[TEST CASE] statement: KW_RETURN expr OP_SEMI 
echo "----------------------------------------------------------------"
echo "return ans;"
echo "return ans;" | ./boris_parser

#[TEST CASE] statement: KW_PRINT expr OP_SEMI 
echo "----------------------------------------------------------------"
echo "print ans;"
echo "print ans;" | ./boris_parser
