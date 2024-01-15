// gcc main.c -o lexscript && ./lexscript test.lx
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <math.h>
#include <time.h>

#ifndef CONST
#define CONST

// CONSTANTS
#define DIGITS "0123456789."
#define LETTERS "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
#define LETTERS_NUMBER "_abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789"

// TOKENS
// DATATYPE TOKENS
#define INT		          "INT"
#define FLOAT             "FLOAT"
#define BOOLEAN           "BOOLEAN"
// SPECIAL TOKENS
#define STATEMENT         "STATEMENT"
#define IDENTIFIER        "IDENTIFIER"
#define KEYWORD           "KEYWORD"
#define EQ                "EQ"
#define EQ_ADD            "EQ_ADD"
#define EQ_SUB            "EQ_SUB"
#define EQ_MUL            "EQ_MUL"
#define EQ_DIV            "EQ_DIV"
#define EQ_MOD            "EQ_MOD"
#define EQ_POW            "EQ_POW"
#define LEFTPAREN         "LEFTPAREN"
#define RIGHTPAREN        "RIGHTPAREN"
#define COMMENT           "#"
#define COMMA             "COMMA"
#define END_LINE          "END_LINE"
#define NEW_LINE          "NEW_LINE"
#define ARG_TYPE          "ARG_TYPE"
// OPERATION TOKENS
#define ADD               "ADD"
#define SUB               "SUB"
#define MUL               "MUL"
#define DIV               "DIV"
#define MOD               "MOD"
#define POW               "POW"
// LOGIC TOKENS 
#define NOT               "NOT"
#define AND               "AND"
#define OR                "OR"
#define COMPARISON        "COMPARISION"
#define NOT_EQUAL         "NOT_EQUAL"
#define LOWER_THAN        "LOWER_THAN"
#define LOWER_OR_EQUAL    "LOWER_OR_EQUAL_THAN"
#define GREATER_THAN      "GREATER_THAN"
#define GREATER_OR_EQUAL  "GREATER_OR_EQUAL_THAN"
// STATEMENTS    
#define IF                "IF"
#define THEN              "THEN"
#define ELIF              "ELIF"
#define ELSE              "ELSE"
#define WHILE             "WHILE"
#define FOR               "FOR"
#define TO                "TO"
#define END               "END"
#define ENDPROG           "ENDPROG"
// KEYWORDS    
#define FUNCTION          "FUNCTION"
#define FUNCTION_RETURNED "FUNCTION-RETURNED"
#define DOES              "DOES"
#define ALT_DOES          "->"
#define RETURN            "RETURN"
#define GLOBAL            "GLOBAL"
#define NUMBER            "VAR"
#define STRING            "STRING"
#define BREAK             "BREAK"
#define CONTINUE          "CONTINUE"
#define BOOL              "BOOL"
#define TRUE              "True"
#define FALSE             "False"

// MATH VARIABLES
#define MATH_E   2.718281828459045
#define MATH_PI  3.141592653589793
#define MATH_TAU (2 * MATH_PI)

// Strings
#define MAX_LEN 100
typedef char string[MAX_LEN];

// Lexer Data Structs
typedef struct {
    char type[20];
    char value[100];
    int line;
} Token;

typedef struct {
    Token** tokens;
    size_t size;
} TokenList;

typedef struct {
    int type_id;
    union {
        float num;
        bool boolean;
        char string[100];
    } value;
} Result;

// Structure of a node in the AST
typedef struct AST {
    Result* item;
    struct AST *left;
    struct AST *right;
} AST;

// Lexscript variables
typedef struct {
    string name;
    string type;
    AST* expresion;
} Variable;

typedef struct {
    Variable** variables;
    size_t size;
} Variables;

Variables* global_variables;

// Functions
typedef struct {
    string name;
    bool returned;
    Variables* arguments;
    TokenList* func_body;
} Function;

typedef struct {
    Function** functions;
    size_t size;
} Functions;

Functions* functions_list;

bool returned = false;
void print_executed_time();

// lexer.c functions
void print_tokens(TokenList* tokens);
void remove_token(TokenList* list);
void append_token(TokenList* list, Token* token);
void free_token_list(TokenList* list);
bool is_in_list(char current_char, char* group);
Token* create_token(char* type, char* value, int line);
Token* make_number(char* code, int *position, int num_line);
Token* make_string(char* current_char, char* code, int* position, int num_line);
Token* make_identifier(char current_char, char* code, int* old_position, int num_line);
TokenList* initialize_token_list();
TokenList* lexer(char* code);

// parser.c functions
void free_ast(AST* root);
void print_ast(AST* root);
AST* parser(TokenList* tokens);
AST* init_ast_node();
AST* create_ast_node(Result* value, AST* left, AST* right);

// AST functions
AST* logic_operators(TokenList* tokens);
AST* logic_comparisions(TokenList* tokens);
AST* add_sub_ast(TokenList* tokens);
AST* mul_div_ast(TokenList* tokens);
AST* pow_ast(TokenList* tokens);
AST* non_operators(TokenList* tokens);

// statements.c functions
AST* if_statement(TokenList* tokens);
void while_statement(TokenList* tokens);
void for_statement(TokenList* tokens);

// management.c functions
void append_variable(Variables* list, char* name, char* type, AST* value);
void assigment(TokenList* tokens, char* type);
void define_funciton(TokenList* tokens);
bool bult_in_functions(TokenList* tokens, AST** ast_node);
AST* call_function(TokenList* tokens, size_t id);
Variables* initialize_variables();

// eval.c functions
double evaluate_ast(AST* ast_node);
void processString(string str);

// Executed time
clock_t start_time, end_time;
double time_taken;

// Num line control
int num_line_code = 0;
int get_num_line() {return num_line_code + 1;}
void set_line(int num_line) {num_line_code = num_line;}

#endif // !CONST