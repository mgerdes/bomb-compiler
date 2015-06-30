#define RET() printf("\tRET\n"); 
#define LAHF() printf("\tLAHF\n"); 
#define CBW() printf("\tCBW\n"); 
#define PUSH(op) printf("\tPUSH\t%s\n", op);
#define POP(op) printf("\tPOP\t%s\n", op);
#define CALL(op) printf("\tCALL\t%s\n", op);
#define INT(op) printf("\tINT\t%s\n", op);
#define JE(label, label_num) printf("\tJE\t%s_%d\n", label, label_num);
#define JMP(label, label_num) printf("\tJMP\t%s_%d\n", label, label_num);
#define LABEL(label, label_num) printf("%s_%d:\n", label, label_num);
#define ADD(op1, op2) printf("\tADD\t%s, %s\n", op1, op2);
#define ADD_NUM(op1, op2) printf("\tADD\t%s, %d\n", op1, op2);
#define SUB(op1, op2) printf("\tSUB\t%s, %s\n", op1, op2);
#define SUB_NUM(op1, op2) printf("\tSUB\t%s, %d\n", op1, op2);
#define MOV(op1, op2) printf("\tMOV\t%s, %s\n", op1, op2);
#define MOV_NUM(op1, num) printf("\tMOV\t%s, %d\n", op1, num);
#define CMP(op1, op2) printf("\tCMP\t%s, %s\n", op1, op2);
#define SHR(op1, op2) printf("\tSHR\t%s, %s\n", op1, op2);
#define OR(op1, op2) printf("\tOR\t%s, %s\n", op1, op2);
#define AND(op1, op2) printf("\tAND\t%s, %s\n", op1, op2);
#define BEGIN_PROC(op) printf("%s PROC\n", op);
#define END_PROC(op) printf("%s ENDP\n", op);

struct ast {
    int type;
};

struct array_node {
    int type;
    struct list_of_expressions * list_of_expressions;
};

struct number_node {
    int type;
    int value;
};

struct arithmetic_node {
    int type;
    int operation;
    struct ast * left;
    struct ast * right;
};

struct if_statement_node {
    int type;
    struct ast * boolean_expression;
    struct ast * then_clause;
    struct ast * else_clause;
};

struct boolean_node {
    int type;
    int operation;
    struct ast * left;
    struct ast * right;
};

struct assignment_node {
    int type;
    struct symbol * symbol;
    struct ast * expression;
};

struct while_node {
    int type;
    struct ast * expression;
    struct ast * statement;
};

struct symbol {
    int type;
    int is_local_to_function;
    char * name;
    int offset;
};

struct list_of_statements {
    int type;
    struct ast * statement;
    struct list_of_statements * rest_of_statements;
};

struct list_of_parameters {
    struct ast * expression;
    struct list_of_parameters * rest_of_parameters;
};

struct function_call_node {
    int type;
    struct symbol * symbol;
    struct list_of_parameters * parameters;
};

struct function {
    int number;
    struct symbol * symbol;
    struct list_of_parameter_symbols * parameters;
    struct list_of_statements * statements;
};

struct list_of_function_definitions {
    struct function * function;
    struct list_of_function_definitions * rest_of_functions;
};

struct list_of_parameter_symbols {
    struct symbol * symbol;
    struct list_of_parameter_symbols * rest_of_symbols;
};

struct list_of_expressions {
    struct ast * expression;
    struct list_of_expressions * rest_of_list;
};

#define SYMBOL_TABLE_SIZE 1000
struct symbol symbol_table[SYMBOL_TABLE_SIZE];

void gen_code(struct ast *);

struct ast * new_number_node(int);
struct ast * new_arithmetic_node(int, struct ast *, struct ast *);
struct ast * new_if_statement_node(struct ast *, struct ast *, struct ast *);
struct ast * new_boolean_node(int, struct ast *, struct ast *);
struct ast * new_assignment_node(struct symbol *, struct ast *);
struct ast * new_while_node(struct ast *, struct ast *);
struct ast * new_list_of_statements_node(struct ast *, struct ast *);
struct ast * new_function_call_node(struct symbol *, struct list_of_parameters *);
struct ast * new_array_node(struct list_of_expressions *);
struct list_of_function_definitions * new_function_definitions_list(struct function *, struct list_of_function_definitions *);
struct list_of_parameters * new_list_of_parameters(struct ast *, struct list_of_parameters *);
struct list_of_parameter_symbols * new_list_of_parameter_symbols(struct symbol *, struct list_of_parameter_symbols *);
struct function * new_function(struct symbol *, struct list_of_parameter_symbols *, struct list_of_statements *);
struct list_of_expressions * new_list_of_expressions(struct ast *, struct list_of_expressions *);
struct symbol * find_symbol(char * name);
