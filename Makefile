lex.yy.c: boris_scanner.l
	flex boris_scanner.l

build: lex.yy.c Token.cpp Scanner.cpp
	g++ Token.cpp Scanner.cpp lex.yy.c -o boris_scanner

clean:
	rm lex.yy.c boris_scanner