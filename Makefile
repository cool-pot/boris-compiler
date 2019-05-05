CC = clang
LD = clang++
CFLAGS = -std=gnu11 -Wall
LLVMCFALGS = -g `llvm-config --cflags`
LLVMLDFLAGS=`llvm-config --cxxflags --ldflags --libs core executionengine mcjit interpreter analysis native bitwriter --system-libs`
MACCOMP = -I/usr/local/opt/llvm/include

clean:
	rm -rf lex.yy.c boris_scanner boris_parser boris\
		boris.lex.c boris.tab.c boris.tab.h \
		example.input boris.output \
		symboltable_test

scanner: boris.l boris.y boris.h borisfuncs.c drivers/parser_driver.c
	bison -d boris.y && \
	flex -oboris.lex.c boris.l && \
	$(CC) $(CFLAGS) -o $@ boris.tab.c boris.lex.c borisfuncs.c drivers/scanner_driver.c

parser-debug: boris.y
	bison -d --report=look-ahead,itemset boris.y && \
	echo "" && \
	echo "> Next:" && \
	echo "> Take a look at 'boris.output'"

symboltable_test:  drivers/symboltable_test_driver.c symboltable.c borisfuncs.c boris.tab.c boris.lex.c
	$(CC) $(CFLAGS) -o $@ drivers/symboltable_test_driver.c symboltable.c borisfuncs.c boris.tab.c boris.lex.c

parser.o: boris.l boris.y boris.h borisfuncs.c drivers/parser.c symboltable.c codegen.c
	bison -d boris.y && \
	flex -oboris.lex.c boris.l && \
	$(CC) $(CFLAGS) $(LLVMCFALGS) -c boris.tab.c boris.lex.c borisfuncs.c drivers/parser.c symboltable.c codegen.c

parser: parser.o
	$(LD) parser.o $(LDFLAGS) -o $@