/*
 *  Syntax analysis
 */

%{
    #include "bomb-funcs.h"
%}

%union {
    struct ast *ast;
    int integer;
    struct symbol *symbol;
    struct symlist *symbol_list;
}

%token IF
%token DEF
%token ELSE
%token UNLESS
%token WHILE
%token FOR
%token END

%token GREATER_THEN_EQUAL
%token LESS_THEN_EQUAL
%token GREATER_THEN
%token LESS_THEN
%token EQUAL
%token EQUAL_EQUAL
%token AND
%token OR

%token PLUS
%token MINUS
%token TIMES
%token DIVIDE
%token MOD
%token PLUS_PLUS
%token MINUS_MINUS

%token NUMBER
%token SYMBOL

%token OPEN_PAREN
%token CLOSE_PAREN
%token COMMA

%left EQUAL
%right PLUS_PLUS MINUS_MINUS
%left AND OR
%left TIMES DIVIDE
%left PLUS MINUS MOD
%left GREATER_THEN_EQUAL LESS_THEN_EQUAL EQUAL_EQUAL GREATER_THEN LESS_THEN 

%type <integer> NUMBER
%type <ast> program statements statement if_statement expression arithmetic_expression boolean_expression assignment SYMBOL while_loop function_call list_parameters

%%

program
    : statements
        { gen_code($1); }
      function_definitions
    ;

statements
    : statement
        { $$ = new_list_of_statements_node($1 ,NULL); }
    | statement statements
        { $$ = new_list_of_statements_node($1, $2); }
    ;

statement
    : if_statement
    | while_loop
    | expression
    ;

while_loop
    : WHILE expression statements END
        { $$ = new_while_node($2, $3); } 
    ;

expression
    : NUMBER
        { $$ = new_number_node($1); }
    | SYMBOL
    | function_call
    | boolean_expression
    | arithmetic_expression
    | assignment
    ;

assignment
    : SYMBOL EQUAL expression 
        { $$ = new_assignment_node((struct symbol *) $1, $3); }
    ;

boolean_expression
    : expression EQUAL_EQUAL expression
        { $$ = new_boolean_node(1, $1, $3); }
    | expression OR expression
        { $$ = new_boolean_node(2, $1, $3); }
    | expression AND expression
        { $$ = new_boolean_node(3, $1, $3); }
    | expression LESS_THEN expression
        { $$ = new_boolean_node(4, $1, $3); }
    ;

arithmetic_expression
    : expression PLUS expression
        { $$ = new_arithmetic_node('+', $1, $3); }
    | expression MINUS expression
        { $$ = new_arithmetic_node('-', $1, $3); }
    ;

if_statement
    : IF expression statements ELSE statement END
        { $$ = new_if_statement_node($2, $3, $5); }
    ;

function_call
    : SYMBOL OPEN_PAREN list_parameters CLOSE_PAREN
        { $$ = new_function_call_node((struct symbol *) $1, $3); }
    ;

list_parameters
    : { /* nothing */ }
    | expression
        { $$ = new_list_of_parameters($1, NULL); }
    | expression COMMA list_parameters
        { $$ = new_list_of_parameters($1, $3); }
    ;

function_definitions
    :
    | function_definitions function_definition
    ;

function_definition
    : DEF SYMBOL OPEN_PAREN list_definition_parameters CLOSE_PAREN statement END
    ;

list_definition_parameters
    : 
    | SYMBOL 
    | SYMBOL COMMA list_definition_parameters 
    ;
