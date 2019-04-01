lex.yy.c: boris_scanner.l
	flex boris_scanner.l

scanner: lex.yy.c Token.cpp Scanner.cpp
	g++ Token.cpp Scanner.cpp lex.yy.c -o boris_scanner

clean:
	rm -rf lex.yy.c boris_scanner boris\
		boris.lex.c boris.tab.c boris.tab.h \
		example.input \

build: boris.l boris.y boris.h borisfuncs.c
	bison -d boris.y && \
	flex -oboris.lex.c boris.l && \
	cc -o boris boris.tab.c boris.lex.c borisfuncs.c