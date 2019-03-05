# boris-compiler

> From NYU CS-UY3943 BK06/CS-GY6413: Compiler Design & Construction.

The project involves building several stages of a complete compiler for a made-up language made by Prof **Boris Aronov**. 


The following parts will be necessary:

- a scanner 
- a parser and symbol table handler
- a type checker
- a code generator

## scanner

- Build the scanner

~~~ bash
>> make scanner
~~~

- Run tests

~~~ bash
>> cat tests/p1test1.txt | ./boris_scanner >> tests/p1test1.out
~~~
~~~
[Warning]INT_LIT in a invalid format. Non-zero integer can't start with 0. No guarantee what will happen. in Line 43, [1,3]
[Warning]The length of INT_LIT exceeds max length, No guarantee what will happen. in Line 47, [1,22]
[Warning]The length of INT_LIT exceeds max length, No guarantee what will happen. in Line 49, [1,22]
[Warning]The length of ID exceeds max length, ID will be truncated. in Line 59, [1,93]
~~~
~~~ bash
>> cat tests/p1test1.out
~~~
~~~
[Token 297] in Line 1, [1,75].
[Token 299] in Line 2, [0,0].
[Token 297] in Line 2, [1,21].
...
[Token 295] in Line 49, [1,22].With value 0
[Token 299] in Line 58, [0,0].
[Token 297] in Line 58, [1,39].
[Token 299] in Line 59, [0,0].
[Token 296] in Line 59, [1,93].With value 'this_is_a_definitely_way_too_long_ID_no_questions_'
~~~

- Igonore the nonsense tokens?

~~~cpp
int main(int argc, char **argv)
{
    // define the Scanner
    bool skip_nonsense_token = true;
    Scanner myScanner(stdin, skip_nonsense_token);

    // lex through the file
    while(myScanner.peek()->getTok() != END_OF_FILE){
        myScanner.next()->printTokenSummary();
    }
}
~~~

In this way, token types as `WS`, `COMMENT`, `END_OF_LINE`, `UNKNOWN` will be ignored.