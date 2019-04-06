#!/bin/bash

#[TEST CASE] decl: KW_LOCAL ID OP_ASSIGN expr OP_SEMI 
echo "----------------------------------------------------------------"
echo "local myTuple = 1+a,2*b;" 
echo "local myTuple = 1+a,2*b;"  | ./boris_parser

#[TEST CASE] decl: KW_GLOBAL ID OP_ASSIGN expr OP_SEMI
echo "----------------------------------------------------------------"
echo "global myElement = yourArray[4];" 
echo "global myElement = yourArray[4];"  | ./boris_parser

#[TEST CASE] decl: KW_ARRAY ID OP_LBRAK expr OP_DOTDOT expr OP_RBRAK OP_SEMI
echo "----------------------------------------------------------------"
echo "array myArray[zero..five];"
echo "array myArray[zero..five];" |./boris_parser   

#[TEST CASE] decl: KW_ARRAY ID OP_LBRAK expr OP_DOTDOT expr OP_RBRAK ID OP_ASSIGN expr OP_SEMI
echo "----------------------------------------------------------------"
echo "array myArray[1..100] i = 6*i+2;"
echo "array myArray[1..100] i = 6*i+2;" |./boris_parser   
