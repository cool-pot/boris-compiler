# boris-compiler

> From NYU CS-UY3943 BK06/CS-GY6413: Compiler Design & Construction.

The project involves building several stages of a complete compiler for a made-up language made by Prof **Boris Aronov**. 

The following parts will be necessary:

- [x] scanner 
- [x] parser
- [x] symbol table handler and type checker
- [x] code generator

## Build the compiler

~~~bash
# Step 1, Compile all object files
> make all-objects

# Step 2, Build the compiler
> make compiler
~~~

## Test the compiler

~~~bash
# Step 1, Generate a compiled LLVM instruction file called "a.ll"
> cat sample_input.txt | ./compiler

# Step 2, Have a look at the compiled file
> cat a.ll

# Step 3, Compile using clang
> clang a.ll -o a.out

# Step 4, Run the execuatable
> ./a.out
~~~

Dependency: 

- clang-6.0.0
- llvm-6.0.0

Some Known Issues:

- no global link in local environment
- `function defun` only support one parameter, can be int or tuple
- return type must be int, no tuple
- `exchange` only support length 1 id on both side
- `assgin` only suppory length 1 id on left hand side
- no return statement in control flow
- no recursion support

