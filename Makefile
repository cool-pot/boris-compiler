CC = gcc
CFLAGS = -std=gnu11 -Wall
SCANNER_TARGET = boris_scanner
PARSER_TARGET = boris_parser

clean:
	rm -rf lex.yy.c boris_scanner boris_parser boris\
		boris.lex.c boris.tab.c boris.tab.h \
		example.input 

scanner: boris.l boris.y boris.h borisfuncs.c parser_driver.c
	bison -d boris.y && \
	flex -oboris.lex.c boris.l && \
	$(CC) $(CFLAGS) -o $(SCANNER_TARGET) boris.tab.c boris.lex.c borisfuncs.c scanner_driver.c

parser: boris.l boris.y boris.h borisfuncs.c parser_driver.c
	bison -d boris.y && \
	flex -oboris.lex.c boris.l && \
	$(CC) $(CFLAGS) -o $(PARSER_TARGET) boris.tab.c boris.lex.c borisfuncs.c parser_driver.c

parser-debug: boris.y
	bison -d --report=look-ahead,itemset boris.y && \
	echo "" && \
	echo "> Next:" && \
	echo "> Take a look at 'boris.output'"
