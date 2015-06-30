%{
#include "./bomb-parser.tab.c"
%}

letter [a-zA-Z_]
digit [0-9] 
letter_or_digit {letter}|{digit}

number {digit}+
symbol {letter}{letter_or_digit}*

whitespace [\n\t ]*

%%

"=" { return EQUAL; } 
"==" { return EQUAL_EQUAL; } 
">=" { return GREATER_THEN_EQUAL; }
"<=" { return LESS_THEN_EQUAL; }
">" { return GREATER_THEN; }
"<" { return LESS_THEN; }
"and" { return AND; }
"or" { return OR; }

"+" { return PLUS; }
"-" { return MINUS; }
"*" { return TIMES; }
"/" { return DIVIDE; }
"%" { return MOD; }
"++" { return PLUS_PLUS; }
"--" { return MINUS_MINUS; }

"end" { return END; }
"while" { return WHILE; }
"if" { return IF; }
"else" { return ELSE; }
"unless" { return UNLESS; }
"for" { return FOR; }
"def" { return DEF; }
"[" { return OPEN_BRACKET; }
"]" { return CLOSE_BRACKET; }
"(" { return OPEN_PAREN; }
")" { return CLOSE_PAREN; }
"," { return COMMA; }

{number} { 
    yylval.integer = atoi(yytext);
    return NUMBER; 
}

{symbol} { 
    yylval.symbol = find_symbol(yytext);
    return SYMBOL; 
}

{whitespace} {}

%%

int main() {
    yyparse();
}

int yyerror(char *error) {
    printf("%s\n", error);
}
