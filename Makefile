bomb-gen:
	flex ./bomb-lexer.l 
	bison ./bomb-parser.y
	gcc ./lex.yy.c ./bomb-funcs.c -lfl -o bomb-gen
gen: bomb-gen
	./bomb-gen < testing.bmb
run: bomb-gen
	./bomb-gen < testing.bmb > testing.asm
	jwasm testing.asm
	jwlink SYS dos F testing L util.lib
	dosbox testing.exe
clean: bomb-gen
	rm ./bomb-parser.tab.c ./lex.yy.c ./bomb-gen
