SYSTEM=dos
NAME=testing
LIBS=lib/util.lib

bomb-gen:
	flex ./bomb-lexer.l 
	bison ./bomb-parser.y
	gcc ./lex.yy.c ./bomb-funcs.c -lfl -o bomb-gen
gen: bomb-gen
	./bomb-gen < testing.bmb > testing.asm
assemble: gen
	jwasm $(NAME).asm
	jwlink SYS $(SYSTEM) F $(NAME) L $(LIBS)
run: assemble
	dosbox $(NAME).exe
clean: 
	rm ./bomb-parser.tab.c ./lex.yy.c ./bomb-gen ./*.asm ./*.o ./*.exe
