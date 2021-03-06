#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bomb-funcs.h"

int current_offset = 0;

void gen_code_for_if_statement(struct if_statement_node *if_statement, int if_statement_number) {
    gen_code(if_statement->boolean_expression);
    CMP("AX", "0");
    JE("ELSE_CLAUSE", if_statement_number);
    gen_code(if_statement->then_clause);
    JMP("END_IF", if_statement_number);
    LABEL("ELSE_CLAUSE", if_statement_number);
    gen_code(if_statement->else_clause);
    LABEL("END_IF", if_statement_number);
}

void gen_code_for_arithmetic_expression(struct arithmetic_node *arithmetic) {
    gen_code(arithmetic->left);
    PUSH("AX");
    gen_code(arithmetic->right);
    POP("BX");
    switch(arithmetic->operation) {
        case '+':
            ADD("AX", "BX");
            break;
        case '-': 
            SUB("BX", "AX");
            MOV("AX", "BX");
            break;
        case '*':
            MUL("BX");
            break;
        case '/':
            XCHG("BX", "AX");
            XOR("DX", "DX");
            DIV("BX");
            break;
    }
}

void gen_code_for_boolean_expression(struct boolean_node *boolean) {
    switch(boolean->operation) {
        case 1:
            /* == */
            gen_code(boolean->left);    
            PUSH("AX");
            gen_code(boolean->right);    
            POP("BX");
            CMP("AX", "BX");
            LAHF();
            SHR("AH", "6");
            AND("AH", "1");
            MOV("AL", "AH");
            CBW();
            break;
        case 2:
            /* OR */
            gen_code(boolean->left);    
            PUSH("AX");
            gen_code(boolean->right);    
            POP("BX");
            OR("AX", "BX");
            break;
        case 3:
            /* AND */
            gen_code(boolean->left);    
            PUSH("AX");
            gen_code(boolean->right);    
            POP("BX");
            AND("AX", "BX");
            break;
        case 4:
            /* < */
            gen_code(boolean->left);
            PUSH("AX");
            gen_code(boolean->right);
            POP("BX");
            CMP("BX", "AX");
            LAHF();
            SHR("AH", "7");
            AND("AH", "1");
            MOV("AL", "AH");
            CBW();
            break;
        case 5:
            /* != */
            gen_code(boolean->left);
            PUSH("AX");
            gen_code(boolean->right);
            POP("BX");
            CMP("AX", "BX");
            LAHF();
            SHR("AH", "6");
            AND("AH", "1");
            MOV("AL", "AH");
            CBW();
            DEC("AX");
            break;
    }
}

void gen_code_for_symbol(struct symbol *symbol) {
    static char op1 [50];
    if (symbol->is_local_to_function) {
        MOV("BX", "OFFSET Parameters");
        ADD("BX", "Parameter_Offset");
        SUB_NUM("BX", symbol->offset);
        snprintf(op1, 50, "[BX]");
    } else {
        snprintf(op1, 50, "WORD PTR [GlobalVariables + %d]", symbol->offset);
    }
    MOV("AX", op1);
}

void gen_code_for_assignment(struct assignment_node *assignment) {
    gen_code(assignment->expression);
    static char op1 [50];
    if (assignment->expression->type == 't') {
        assignment->symbol->data_type = 't';
    } else {
        assignment->symbol->data_type = 'i';
    }
    snprintf(op1, 50, "WORD PTR [GlobalVariables + %d]", assignment->symbol->offset);
    MOV(op1, "AX");
}

void gen_code_for_array_lookup_assignment(struct array_lookup_assignment_node * assignment) {
    gen_code(assignment->array_lookup->expression);
    if (assignment->array_lookup->symbol->is_local_to_function) {
        MOV("BX", "OFFSET Parameters");
        ADD("BX", "Parameter_Offset");
        SUB_NUM("BX", assignment->array_lookup->symbol->offset);
    } else {
        MOV("BX", "OFFSET GlobalVariables");
        ADD_NUM("BX", assignment->array_lookup->symbol->offset);
    }
    MOV("BX", "WORD PTR [BX]");
    ADD("BX", "AX");
    if (assignment->array_lookup->symbol->data_type != 't') {
        ADD("BX", "AX");
    }
    PUSH("BX");
    gen_code(assignment->expression);
    POP("BX");
    if (assignment->array_lookup->symbol->data_type == 't') {
        MOV("BYTE PTR [BX]", "AL");
    } else {
        MOV("WORD PTR [BX]", "AX");
    }
}

void gen_code_for_while(struct while_node *while_node, int while_loop_number) {
    LABEL("WHILE_LOOP", while_loop_number);
    gen_code(while_node->expression);
    CMP("AX", "0");
    JE("END_WHILE_LOOP", while_loop_number);
    gen_code(while_node->statement);
    JMP("WHILE_LOOP", while_loop_number);
    LABEL("END_WHILE_LOOP", while_loop_number);
}

void gen_code_for_list_of_statements(struct list_of_statements * list) {
    gen_code(list->statement);
    if (list->rest_of_statements) {
        gen_code_for_list_of_statements(list->rest_of_statements);
    }
}

void gen_code_for_function_call(struct function_call_node * function_call) {
    struct list_of_parameters * parameters = function_call->parameters;
    int total_offset = 0;
    while (parameters) {
        gen_code(parameters->expression);
        MOV("BX", "OFFSET Parameters");
        ADD("BX", "Parameter_Offset");
        ADD_NUM("BX", total_offset);
        MOV("WORD PTR [BX]", "AX");
        total_offset += 2;
        parameters = parameters->rest_of_parameters;
    }
    ADD_NUM("Parameter_Offset", total_offset);
    CALL(function_call->symbol->name);
    SUB_NUM("Parameter_Offset", total_offset); 
}

void gen_code_for_functions(struct list_of_function_definitions * functions) {
    while (functions) {
        BEGIN_PROC(functions->function->symbol->name);        
        gen_code((struct ast *) functions->function->statements);
        RET();
        END_PROC(functions->function->symbol->name);        
        functions = functions->rest_of_functions;
    }
}

void gen_code_for_array(struct array_node * array) {
    static char op1 [50];
    struct list_of_expressions * expressions = array->list_of_expressions;    
    MOV("BX", "OFFSET GlobalVariables");
    ADD_NUM("BX", current_offset); 
    PUSH("BX");
    while (expressions) {
        gen_code(expressions->expression);
        snprintf(op1, 50, "WORD PTR [GlobalVariables + %d]", current_offset);
        current_offset += 2;
        MOV(op1, "AX");
        expressions = expressions->rest_of_list;
    }
    POP("AX");
}

void gen_code_for_string(struct string_node * string_node) {
    static char op1 [50];
    char * string = string_node->string;
    MOV("BX", "OFFSET GlobalVariables");
    ADD_NUM("BX", current_offset); 
    PUSH("BX");
    string++;
    while (*string != '"') {
        snprintf(op1, 50, "BYTE PTR [GlobalVariables + %d]", current_offset);
        MOV_NUM(op1, *string);
        current_offset++;
        string++;
    }
    snprintf(op1, 50, "BYTE PTR [GlobalVariables + %d]", current_offset);
    MOV_NUM(op1, '$');
    current_offset++;
    POP("AX");
}

void gen_code_for_array_lookup(struct array_lookup_node * array_lookup) {
    gen_code(array_lookup->expression);
    if (array_lookup->symbol->is_local_to_function) {
        MOV("BX", "OFFSET Parameters");
        ADD("BX", "Parameter_Offset");
        SUB_NUM("BX", array_lookup->symbol->offset);
    } else {
        MOV("BX", "OFFSET GlobalVariables");
        ADD_NUM("BX", array_lookup->symbol->offset);
    }
    MOV("BX", "WORD PTR [BX]");  
    ADD("BX", "AX");
    if (array_lookup->symbol->data_type == 't') {
        MOV("AL", "BYTE PTR [BX]");  
        XOR("AH", "AH");
    } else {
        ADD("BX", "AX");
        MOV("AX", "WORD PTR [BX]");
    }
}

void gen_code(struct ast *ast) {
    if (!ast) return;
    static int number_of_if_statements;
    static int number_of_while_loops;

    switch(ast->type) {
        case 'n':
            MOV_NUM("AX", ((struct number_node *) ast)->value);
            break;
        case 'r':
            gen_code_for_arithmetic_expression((struct arithmetic_node *) ast);
            break;
        case 'i':
            gen_code_for_if_statement((struct if_statement_node *) ast, number_of_if_statements++);
            break;
        case 'b':
            gen_code_for_boolean_expression((struct boolean_node *) ast);
            break;
        case 's':
            gen_code_for_symbol((struct symbol *) ast);        
            break;
        case 'a':
            gen_code_for_assignment((struct assignment_node *) ast);        
            break;
        case 'L':
            gen_code_for_array_lookup_assignment((struct array_lookup_assignment_node *) ast);        
            break;
        case 'w':
            gen_code_for_while((struct while_node *) ast, number_of_while_loops++);        
            break;
        case 'l':
            gen_code_for_list_of_statements((struct list_of_statements *) ast);
            break;
        case 'f':
            gen_code_for_function_call((struct function_call_node *) ast);
            break;
        case 'y':
            gen_code_for_array((struct array_node *) ast);
            break;
        case 't':
            gen_code_for_string((struct string_node *) ast);
            break;
        case 'A':
            gen_code_for_array_lookup((struct array_lookup_node *) ast);
            break;
    }

}

struct ast * new_number_node(int n) {
    struct number_node *new_number = (struct number_node *) malloc(sizeof(struct number_node));
    new_number->type = 'n';
    new_number->value = n;
    return (struct ast *) new_number;
}

struct ast * new_arithmetic_node(int operation, struct ast * left, struct ast * right) {
    struct arithmetic_node *new_arithmetic = (struct arithmetic_node *) malloc(sizeof(struct arithmetic_node));
    new_arithmetic->type = 'r';
    new_arithmetic->operation = operation;
    new_arithmetic->left = left;
    new_arithmetic->right = right;
    return (struct ast *) new_arithmetic;
}

struct ast * new_if_statement_node(struct ast * boolean_expression, struct ast * then_clause, struct ast * else_clause) {
    struct if_statement_node *new_if = (struct if_statement_node *) malloc(sizeof(struct if_statement_node));
    new_if->type = 'i';
    new_if->boolean_expression = boolean_expression;
    new_if->then_clause = then_clause;
    new_if->else_clause = else_clause;
    return (struct ast *) new_if;
}

struct ast * new_boolean_node(int operation, struct ast * left, struct ast * right) {
    struct boolean_node *new_boolean = (struct boolean_node *) malloc(sizeof(struct boolean_node)); 
    new_boolean->type = 'b';
    new_boolean->operation = operation;
    new_boolean->left = left;
    new_boolean->right = right;
    return (struct ast *) new_boolean;
}

struct ast * new_assignment_node(struct symbol * symbol, struct ast * expression) {
    struct assignment_node *new_assignment = (struct assignment_node *) malloc(sizeof(struct assignment_node));
    new_assignment->type = 'a';
    new_assignment->symbol = symbol;
    new_assignment->expression = expression;
    return (struct ast *) new_assignment;
}

struct ast * new_array_lookup_assignment_node(struct array_lookup_node * array_lookup, struct ast * expression) {
    struct array_lookup_assignment_node * new_assignment = (struct array_lookup_assignment_node *) malloc(sizeof(struct array_lookup_assignment_node));
    new_assignment->type = 'L';
    new_assignment->array_lookup = array_lookup;
    new_assignment->expression = expression;
    return (struct ast *) new_assignment;
}

struct ast * new_while_node(struct ast * expression, struct ast * statement) {
    struct while_node *new_while = (struct while_node *) malloc(sizeof(struct while_node));
    new_while->type = 'w';
    new_while->expression = expression;
    new_while->statement = statement;
    return (struct ast *) new_while;
}

struct ast * new_list_of_statements_node(struct ast * statement, struct ast * list) {
    struct list_of_statements * new_list = (struct list_of_statements *) malloc(sizeof(struct list_of_statements));
    new_list->type = 'l';
    new_list->statement = statement;
    new_list->rest_of_statements = (struct list_of_statements *) list;
    return (struct ast *) new_list;
}

struct ast * new_function_call_node(struct symbol * symbol, struct list_of_parameters * parameters) {
    struct function_call_node * new_function_call = (struct function_call_node *) malloc(sizeof(struct function_call_node));
    new_function_call->type = 'f';
    new_function_call->symbol = symbol;
    new_function_call->parameters = parameters;
    return (struct ast *) new_function_call;
}

struct ast * new_array_node(struct list_of_expressions * list_of_expressions) {
    struct array_node * new_array_node = (struct array_node *) malloc(sizeof(struct array_node));
    new_array_node->type = 'y';
    new_array_node->list_of_expressions = list_of_expressions;
    return (struct ast *) new_array_node;
}

struct list_of_expressions * new_list_of_expressions(struct ast * expression, struct list_of_expressions * rest_of_list) {
    struct list_of_expressions * new_list_of_expressions = (struct list_of_expressions *) malloc(sizeof(struct list_of_expressions));
    new_list_of_expressions->expression = expression;
    new_list_of_expressions->rest_of_list = rest_of_list;
    return new_list_of_expressions;
}

struct list_of_function_definitions * new_function_definitions_list(struct function * function, struct list_of_function_definitions * rest_of_functions) {
    struct list_of_function_definitions * new_list = (struct list_of_function_definitions *) malloc(sizeof(struct list_of_function_definitions));
    new_list->function = function;
    new_list->rest_of_functions = rest_of_functions;
    return new_list;
}

struct list_of_parameters * new_list_of_parameters(struct ast * expression, struct list_of_parameters * list) {
    struct list_of_parameters * new_list = (struct list_of_parameters *) malloc(sizeof(struct list_of_parameters));
    new_list->expression = expression;
    new_list->rest_of_parameters = list;
    return new_list;
}

struct list_of_parameter_symbols * new_list_of_parameter_symbols(int data_type, struct symbol * symbol, struct list_of_parameter_symbols * list) {
    struct list_of_parameter_symbols * new_list = (struct list_of_parameter_symbols *) malloc(sizeof(struct list_of_parameter_symbols));
    new_list->symbol = symbol;
    new_list->symbol->data_type = data_type;
    new_list->rest_of_symbols = list;
    return new_list;
}

struct function * new_function(struct symbol * symbol, struct list_of_parameter_symbols * parameters, struct list_of_statements * statements) {
    static int number_of_functions = 0;
    int parameter_offset = 2;
    struct function * new_function = (struct function *) malloc(sizeof(struct function));
    new_function->number = number_of_functions++;
    new_function->symbol = symbol;
    new_function->parameters = parameters;
    while(parameters) {
        parameters->symbol->is_local_to_function = 1;
        parameters->symbol->offset = parameter_offset;
        parameter_offset += 2;
        parameters = parameters->rest_of_symbols;
    }
    new_function->statements = statements;
    return new_function;
}

struct ast * new_string_node(char * string) {
    struct string_node * new_string = (struct string_node *) malloc(sizeof(struct string_node));
    new_string->type = 't';
    new_string->string = string;
    return (struct ast *) new_string;
}

struct ast * new_array_lookup_node(struct symbol * symbol, struct ast * expression) {
    struct array_lookup_node * new_array_lookup = (struct array_lookup_node *) malloc(sizeof(struct array_lookup_node));    
    new_array_lookup->type = 'A';
    new_array_lookup->symbol = symbol;
    new_array_lookup->expression = expression;
    return (struct ast *) new_array_lookup;
}

int symbol_hash(char * name) {
    int hash = 0;
    char c;

    while(c = *name++) { 
        hash = (hash * 9 ^ c) % SYMBOL_TABLE_SIZE;
    }

    return hash % SYMBOL_TABLE_SIZE;
}

struct symbol * find_symbol(char * name) {
    struct symbol * symbol = &symbol_table[symbol_hash(name)];

    int symbol_count = SYMBOL_TABLE_SIZE;
    while(--symbol_count >= 0) {
        if (symbol->name && !strcmp(symbol->name, name)) { 
            return symbol; 
        }

        if (!symbol->name) {
            symbol->name = strdup(name);
            symbol->type = 's';
            symbol->is_local_to_function = 0;
            symbol->offset = current_offset;
            current_offset += 2;
            return symbol;
        }
    }

    /* no more room for symbols */
    return 0;
}

void init() {
    printf("\t.MODEL\tSMALL\n"); 
    printf("\t.386\n"); 
    printf("\t.STACK\t100h\n"); 
    printf("\t.DATA\n"); 
    printf("INCLUDE\tmacros/PCMAC.INC\n"); 
    printf("GlobalVariables DW 500 DUP (?)\n");
    printf("Parameters DW 500 DUP (?)\n");
    printf("Parameter_Offset DW ?\n");
    printf("\t.CODE\n"); 
    printf("\tEXTERN\tPutDec : Near\n"); 
    printf("main PROC\n"); 
    MOV("AX", "@DATA");
    MOV("DS", "AX");
}

void put_str_proc() {
    BEGIN_PROC("put_str");
    MOV("BX", "OFFSET Parameters");
    ADD("BX", "Parameter_Offset");
    SUB("BX", "2");
    printf("\t_PutStr\tWORD PTR [BX]\n");
    RET();
    END_PROC("put_str");
}

void put_dec_proc() {
    BEGIN_PROC("put_dec");
    MOV("BX", "OFFSET Parameters");
    ADD("BX", "Parameter_Offset");
    SUB("BX", "2");
    MOV("AX", "WORD PTR [BX]");
    printf("\tCALL\tPutDec\n");
    RET();
    END_PROC("put_dec");
}

void end_main_proc() {
    MOV("AL", "0");
    MOV("AH", "4CH");
    INT("21H");
    printf("main ENDP\n");
}

void end_program() {
    put_str_proc();
    put_dec_proc();
    printf("\tEND main\n");
}
