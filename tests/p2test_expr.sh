#!/bin/bash

#[TEST CASE] expr OP_COMMA expr 
echo "----------------------------------------------------------------"
echo "INPUT: a,b" 
echo "a,b" | ./boris_parser
echo "----------------------------------------------------------------"
echo "INPUT: a,b,c" 
echo "a,b,c" | ./boris_parser

#[TEST CASE] expr OP_MINUS expr 
echo "----------------------------------------------------------------"
echo "INPUT: a-b" 
echo "a-b" | ./boris_parser
echo "----------------------------------------------------------------"
echo "INPUT: a-b-c" 
echo "a-b-c" | ./boris_parser

#[TEST CASE] expr OP_PLUS expr 
echo "----------------------------------------------------------------"
echo "INPUT: a+b" 
echo "a+b" | ./boris_parser
echo "----------------------------------------------------------------"
echo "INPUT: a+b+c" 
echo "a+b+c" | ./boris_parser

#[TEST CASE] expr OP_DIV expr 
echo "----------------------------------------------------------------"
echo "INPUT: a/b" 
echo "a/b" | ./boris_parser
echo "----------------------------------------------------------------"
echo "INPUT: a/b/c" 
echo "a/b/c" | ./boris_parser

#[TEST CASE] expr OP_MULT expr
echo "----------------------------------------------------------------"
echo "INPUT: a*b" 
echo "a*b" | ./boris_parser
echo "----------------------------------------------------------------"
echo "INPUT: a*b*c" 
echo "a*b*c" | ./boris_parser

#[TEST CASE] OP_LPAR expr OP_RPAR
echo "----------------------------------------------------------------"
echo "INPUT: (a)" 
echo "(a)" | ./boris_parser
echo "----------------------------------------------------------------"
echo "INPUT: (a+b)" 
echo "(a+b)" | ./boris_parser
echo "----------------------------------------------------------------"
echo "INPUT: ((a))" 
echo "((a))" | ./boris_parser

#[TEST CASE] ID 
echo "----------------------------------------------------------------"
echo "INPUT: some_id" 
echo "some_id" | ./boris_parser
echo "----------------------------------------------------------------"
echo "INPUT: some_very_very_very_very_very_very_very_very_very_very_very_very_very_very_very_very_long_id" 
echo "some_very_very_very_very_very_very_very_very_very_very_very_very_very_very_very_very_long_id" | ./boris_parser

#[TEST CASE] ID expr 
echo "----------------------------------------------------------------"
echo "INPUT: f g args" 
echo "f g args" | ./boris_parser
echo "----------------------------------------------------------------"
echo "INPUT: f (a+b)" 
echo "f (a+b)" | ./boris_parser

#[TEST CASE] ID OP_DOT INT_LIT 
echo "----------------------------------------------------------------"
echo "INPUT: tup.0" 
echo "tup.0" | ./boris_parser

#[TEST CASE] ID OP_LBRAK expr OP_RBRAK 
echo "----------------------------------------------------------------"
echo "INPUT: arr[arg]" 
echo "arr[arg]" | ./boris_parser

#[TEST CASE for precedence]
echo "----------------------------------------------------------------"
echo "INPUT: a+b*c" 
echo "a+b*c" | ./boris_parser
echo "----------------------------------------------------------------"
echo "INPUT: (a+b)*c" 
echo "(a+b)*c" | ./boris_parser
echo "----------------------------------------------------------------"
echo "INPUT: a,b+c" 
echo "a,b+c" | ./boris_parser
echo "----------------------------------------------------------------"
echo "INPUT: a,(b,c),d" 
echo "a,(b,c),d" | ./boris_parser
echo "----------------------------------------------------------------"
echo "INPUT: f (a,b)" 
echo "f (a,b)" | ./boris_parser
echo "----------------------------------------------------------------"
echo "INPUT: f [a,b]" 
echo "f [a,b]" | ./boris_parser