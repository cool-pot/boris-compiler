lex.yy.c: boris.l
	flex boris.l

build: lex.yy.c
	g++ lex.yy.c -o boris

clean:
	rm lex.yy.c boris