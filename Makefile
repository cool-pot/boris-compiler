clean:
	rm -rf lex.yy.c boris_scanner boris\
		boris.lex.c boris.tab.c boris.tab.h \
		example.input \

build: boris.l boris.y boris.h borisfuncs.c
	bison -d boris.y && \
	flex -oboris.lex.c boris.l && \
	cc -o boris boris.tab.c boris.lex.c borisfuncs.c