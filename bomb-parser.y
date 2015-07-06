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
    char *string;

    struct function *function;
    struct list_of_parameter_symbols *list_parameters;
    struct list_of_function_definitions *functions;
    struct list_of_parameter_symbols *function_parameters;
    struct list_of_expressions *list_of_expressions;
}

%token IF
%token DEF
%token ELSE
%token UNLESS
%token WHILE
%token FOR
%token END
%token DATA_TYPE

%token GREATER_THEN_EQUAL
%token LESS_THEN_EQUAL
%token GREATER_THEN
%token LESS_THEN
%token EQUAL
%token EQUAL_EQUAL
%token NOT_EQUAL
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
%token STRING

%token OPEN_BRACKET
%token CLOSE_BRACKET
%token OPEN_PAREN
%token CLOSE_PAREN
%token OPEN_QUOTE
%token CLOSE_QUOTE
%token COMMA

%left EQUAL
%left SYMBOL
%left OPEN_BRACKET CLOSE_BRACKET
%right PLUS_PLUS MINUS_MINUS
%left AND OR
%left TIMES DIVIDE
%left PLUS MINUS MOD
%left GREATER_THEN_EQUAL LESS_THEN_EQUAL EQUAL_EQUAL GREATER_THEN LESS_THEN 

%type <integer> NUMBER DATA_TYPE
%type <string> STRING
%type <ast> program statements statement if_statement expression arithmetic_expression boolean_expression assignment SYMBOL while_loop function_call list_parameters array array_lookup
%type <function> function_definition
%type <functions> function_definitions
%type <function_parameters> list_definition_parameters
%type <list_of_expressions> list_of_expressions

%%

program
    : statements
        { 
            init();
            gen_code($1); 
            end_main_proc();
        }
      function_definitions
        {
            gen_code_for_functions($3); 
            end_program();
        }
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
    | array
    | array_lookup
    | STRING 
        { $$ = new_string_node($1); }
    ;

array_lookup
    : SYMBOL OPEN_BRACKET expression CLOSE_BRACKET
        { $$ = new_array_lookup_node((struct array_lookup_node *) $1, $3); }
    ;

array
    : OPEN_BRACKET list_of_expressions CLOSE_BRACKET
        { $$ = new_array_node($2); }
    ;

list_of_expressions
    : expression
        { $$ = new_list_of_expressions($1, NULL); }
    | expression COMMA list_of_expressions
        { $$ = new_list_of_expressions($1, $3); }
    ;

assignment
    : SYMBOL EQUAL expression 
        { $$ = new_assignment_node((struct symbol *) $1, $3); }
    | array_lookup EQUAL expression
        { $$ = new_array_lookup_assignment_node($1, $3); }        
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
    | expression NOT_EQUAL expression
        { $$ = new_boolean_node(5, $1, $3); }
    ;

arithmetic_expression
    : expression PLUS expression
        { $$ = new_arithmetic_node('+', $1, $3); }
    | expression MINUS expression
        { $$ = new_arithmetic_node('-', $1, $3); }
    | expression TIMES expression
        { $$ = new_arithmetic_node('*', $1, $3); }
    | expression DIVIDE expression
        { $$ = new_arithmetic_node('/', $1, $3); }
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
    : function_definition
        { $$ = new_function_definitions_list($1, NULL); }
    | function_definition function_definitions
        { $$ = new_function_definitions_list($1, $2); }
    ;

function_definition
    : DEF SYMBOL OPEN_PAREN list_definition_parameters CLOSE_PAREN statements END
        { $$ = new_function($2, $4, $6); }
    ;

list_definition_parameters
    : 
        {  }
    | DATA_TYPE SYMBOL 
        { $$ = new_list_of_parameter_symbols($1, $2, NULL);  }
    | list_definition_parameters COMMA DATA_TYPE SYMBOL 
        { $$ = new_list_of_parameter_symbols($3, $4, $1);  }
    ;
