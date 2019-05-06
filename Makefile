CC = clang
LD = clang++
CFLAGS = -std=gnu11 -Wall -Wno-incompatible-pointer-types-discards-qualifiers
CFLAGS += -g `llvm-config --cflags`
LLVMLDFLAGS =`llvm-config --cxxflags --ldflags --libs core executionengine mcjit interpreter analysis native bitwriter --system-libs`

clean:
	rm -rf lex.yy.c boris_scanner boris_parser boris parser scanner\
		boris.lex.c boris.tab.c boris.tab.h \
		boris.output \
		symboltable_test \
		*.o \
		*.bc \
		*.ll \
		a.out \

parser-debug: boris.y
	bison -d --report=look-ahead,itemset boris.y && \
	echo "" && \
	echo "> Next:" && \
	echo "> Take a look at 'boris.output'"

all-objects: boris.l boris.y boris.h borisfuncs.c drivers/parser.c symboltable.c codegen.c drivers/scanner.c drivers/symboltable_test.c
	bison -d boris.y && \
	flex -oboris.lex.c boris.l && \
	$(CC) $(CFLAGS) -c boris.tab.c boris.lex.c borisfuncs.c drivers/parser.c symboltable.c codegen.c drivers/scanner.c drivers/symboltable_test.c

symboltable_test:  drivers/symboltable_test.c symboltable.c borisfuncs.c boris.tab.c boris.lex.c
	$(LD) boris.lex.o  boris.tab.o symboltable.o codegen.o borisfuncs.o symboltable_test.o $(LLVMLDFLAGS) -o $@

parser: borisfuncs.o boris.lex.o  boris.tab.o  codegen.o symboltable.o parser.o
	$(LD) boris.lex.o  boris.tab.o symboltable.o codegen.o borisfuncs.o parser.o $(LLVMLDFLAGS) -o $@

scanner: borisfuncs.o boris.lex.o  boris.tab.o  codegen.o symboltable.o parser.o
	$(LD) boris.lex.o  boris.tab.o symboltable.o codegen.o borisfuncs.o scanner.o $(LLVMLDFLAGS) -o $@

a.ll: a.bc
	llvm-dis a.bc

a.out: a.ll
	clang a.ll -o a.out