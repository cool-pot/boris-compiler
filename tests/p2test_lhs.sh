#!/bin/bash

#[TEST CASE] lhs: lhsitem comma_lhsitem_list || when comma_lhsitem_list is empty
echo "----------------------------------------------------------------"
echo "INPUT: a" 
echo "a" | ./boris_parser

#[TEST CASE] lhs: lhsitem comma_lhsitem_list || when comma_lhsitem_list is not empty
echo "----------------------------------------------------------------"
echo "INPUT: a,b" 
echo "a,b" | ./boris_parser

echo "----------------------------------------------------------------"
echo "INPUT: a,b,c" 
echo "a,b,c" | ./boris_parser

#[TEST CASE] lhs: lhsitem comma_lhsitem_list || when complex lhsitem is used
echo "----------------------------------------------------------------"
echo "INPUT: tup.2, a" 
echo "tup.2, a, arr[s.3,b]" | ./boris_parser


