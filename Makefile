all:
	flex ./bomb-lexer.l 
	bison ./bomb-parser.y
	gcc ./lex.yy.c ./bomb-funcs.c -lfl -o bomb-gen
test:
	make
	./bomb-gen < testing.bmb
clean:
	rm ./bomb-parser.tab.c ./lex.yy.c ./bomb-gen
