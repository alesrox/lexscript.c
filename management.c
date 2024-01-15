#include "language_core.h"

Variables* initialize_variables() {
    Variables* list = malloc(sizeof(Variables));
    if (list != NULL) {
        list->variables = NULL;
        list->size = 0;
    }
    return list;
}

Functions* initialize_functions() {
    Functions* list = malloc(sizeof(Variables));
    if (list != NULL) {
        list->functions = NULL;
        list->size = 0;
    }
    return list;
}

void append_variable(Variables* list, char* name, char* type, AST* expresion) {
    Variable* var = malloc(sizeof(Variable));

    if (var == NULL) {
        fprintf(stderr, "Error: Could not allocate memory for the variable.\n");
        exit(EXIT_FAILURE);
    }
    
    strncpy(var->name, name, sizeof(var->name) - 1);
    var->name[sizeof(var->name) - 1] = '\0';
    strncpy(var->type, type, sizeof(var->type) - 1);
    var->type[sizeof(var->type) - 1] = '\0';

    var->expresion = expresion;

    list->variables = realloc(list->variables, (list->size + 1) * sizeof(Variable*));

    if (list->variables == NULL) {
        fprintf(stderr, "Error: Could not allocate memory for token list.\n");
        exit(EXIT_FAILURE);
    }

    list->variables[list->size++] = var;
}

// Create a copy of Variables
Variables* copy_variables(Variables* source) {
    Variables* copy = malloc(sizeof(Variables));
    if (copy == NULL) {
        fprintf(stderr, "Error: Could not allocate memory for copy of Variables.\n");
        exit(EXIT_FAILURE);
    }

    copy->size = source->size;
    copy->variables = malloc(source->size * sizeof(Variable*));
    if (copy->variables == NULL) {
        fprintf(stderr, "Error: Could not allocate memory for variable list copy.\n");
        exit(EXIT_FAILURE);
    }

    // Copy each variable individually
    for (size_t i = 0; i < source->size; ++i) {
        Variable* original_variable = source->variables[i];

        // Copy the content of the variable
        Variable* copied_variable = malloc(sizeof(Variable));
        if (copied_variable == NULL) {
            fprintf(stderr, "Error: Could not allocate memory for the variable copy.\n");
            exit(EXIT_FAILURE);
        }

        strcpy(copied_variable->name, original_variable->name);
        strcpy(copied_variable->type, original_variable->type);

        // If the expression is an AST, creates a recursive copy of the AST
        if (original_variable->expresion != NULL) {
            copied_variable->expresion = create_ast_node(original_variable->expresion->item, original_variable->expresion->left, original_variable->expresion->right);
        } else {
            copied_variable->expresion = NULL;
        }

        // Add the copied variable to the list of copied variables
        copy->variables[i] = copied_variable;
    }

    return copy;
}

void assigment(TokenList* tokens, char* type) {
    TokenList* tokens_var = initialize_token_list();
    int line = tokens->tokens[0]->line;
    bool reasigment = true;

    if (strcmp(tokens->tokens[0]->type, KEYWORD) == 0) {
        remove_token(tokens);
        reasigment = false;
    }

    if (strcmp(tokens->tokens[0]->type, IDENTIFIER) == 0) {
        string name;
        strcpy(name, tokens->tokens[0]->value);
        remove_token(tokens);
        
        if (!reasigment) {
            for (size_t i = 0; i < global_variables->size; i++) {
                if (strcmp(global_variables->variables[i]->name, name) == 0) {
                    printf("\nSyntaxError on line %i: Identifier '%s' already exists", line+1, name);
                    exit(EXIT_FAILURE);
                }
            }
        }
        
        if (strcmp(tokens->tokens[0]->type, EQ) == 0) {
            remove_token(tokens);
            int count = 0;
            for (size_t i = 0; i < tokens->size; i++) {
                Token* token = tokens->tokens[i];
                count++;
                if (strcmp(token->type, NEW_LINE) == 0
                    || strcmp(token->type, END_LINE) == 0) break;
                append_token(tokens_var, token);
            }
            append_token(tokens_var, create_token(ENDPROG, ENDPROG, line));

            line = tokens_var->tokens[0]->line;
            AST* value = logic_operators(tokens_var);

            if (tokens_var->size != 0 && strcmp(tokens_var->tokens[0]->value, IF) == 0) {
                remove_token(tokens_var);
                AST* condition = logic_operators(tokens_var);
                if (tokens_var->size == 0 || strcmp(tokens_var->tokens[0]->value, ELSE) != 0) {
                    printf("\nSyntaxError on line %i: ELSE STATEMENT was expected", line+1);
                    exit(EXIT_FAILURE);
                }
                remove_token(tokens_var);

                if (!evaluate_ast(condition)) {
                    value = logic_operators(tokens_var);
                }
            }

            int expresion_type_id = value->item->type_id;
            char* expresion_type;
            if (expresion_type_id == 1 || expresion_type_id == 4) expresion_type = NUMBER;
            if (expresion_type_id == 2 || expresion_type_id == 5) expresion_type = BOOL;
            if (expresion_type_id == 3) expresion_type = STRING;

            if (strcmp(expresion_type, type) != 0) {
                printf("\nValueError on line %i: Can't assign a %s to a %s", line+1, expresion_type, type);
                exit(EXIT_FAILURE);
            }
            
            for (int i = 0; i < count; i++) {remove_token(tokens);}
            if (reasigment) {
                for (size_t i = 0; i < global_variables->size; i++) {
                    if (strcmp(global_variables->variables[i]->name, name) == 0) {
                        global_variables->variables[i]->expresion = value;
                    }
                }
            } else {
                append_variable(global_variables, name, type, value);
            }
        } else {
            printf("\nSyntaxError on line %i: An equal sign '=' was expected for assignment", line+1);
            exit(EXIT_FAILURE);
        }
    } else {
        printf("\nSyntaxError on line %i: Identifier expected for assignment", line+1);
        exit(EXIT_FAILURE);
    }
}

void define_funciton(TokenList* tokens) {
    string func_name;
    remove_token(tokens);
    strcpy(func_name, tokens->tokens[0]->value);
    remove_token(tokens);

    for (size_t i = 0; i < functions_list->size; i++) {
        if (strcmp(functions_list->functions[i]->name, func_name) == 0) {
            printf("\nSyntaxError on line %i: Function '%s' already exists", tokens->tokens[0]->line+1, func_name);
            exit(EXIT_FAILURE);
        }
    }

    Variables* func_arguments = initialize_variables();
    if (strcmp(tokens->tokens[0]->type, LEFTPAREN) == 0) {
        remove_token(tokens);

        while (true) {
            if (strcmp(tokens->tokens[0]->type, IDENTIFIER) == 0 
            && strcmp(tokens->tokens[1]->value, ARG_TYPE) == 0
            && strcmp(tokens->tokens[2]->type, KEYWORD) == 0) {
                append_variable(func_arguments, tokens->tokens[0]->value, tokens->tokens[2]->value, NULL);
                remove_token(tokens);
                remove_token(tokens);
                remove_token(tokens);
            } else if (strcmp(tokens->tokens[0]->value, COMMA) == 0) {
                remove_token(tokens);
            } else if (strcmp(tokens->tokens[0]->type, RIGHTPAREN) == 0) {
                remove_token(tokens);
                break;
            } else {
                printf("\nSyntaxError on line %i: Unexpected simbol on function definition -> %s", tokens->tokens[0]->line+1, tokens->tokens[0]->value);
                exit(EXIT_FAILURE);
            }
        }
    }

    if (strcmp(tokens->tokens[0]->value, DOES) == 0 || strcmp(tokens->tokens[0]->value, ALT_DOES) == 0) {
        remove_token(tokens);
    } else {
        printf("\nSyntaxError on line %i: Expected a DOES or '->' STATEMENT", tokens->tokens[0]->line+1);
        exit(EXIT_FAILURE);
    }

    TokenList* func_body = initialize_token_list();
    Token* token;
    size_t end = tokens->size;
    bool multiline = strcmp(tokens->tokens[0]->type, NEW_LINE) == 0 ? true : false;
    int statements = 0;

    for (size_t i = 0; i < end; i++) {
        token = tokens->tokens[0];

        if (strcmp(token->type, NEW_LINE) == 0 && !multiline) {break;}

        if (strcmp(token->value, IF) == 0 || strcmp(token->value, WHILE) == 0 || strcmp(token->value, FOR) == 0) {
            statements++;
        }

        if (strcmp(token->value, END) == 0) {
            if (statements > 0) {
                statements--;
            } else {
                remove_token(tokens); 
                break;
            }
        }

        append_token(func_body, create_token(token->type, token->value, token->line));
        remove_token(tokens);
    }
    
    functions_list->functions = realloc(functions_list->functions, (functions_list->size + 1) * sizeof(Function*));
    Function* func = malloc(sizeof(Function));

    strcpy(func->name, func_name);
    func->returned = false;
    func->arguments = func_arguments;
    func->func_body = func_body;
    functions_list->functions[functions_list->size++] = func;
}

int get_type_id(string type) {
    if (strcmp(type, NUMBER) == 0) {
        return 1;
    } else if (strcmp(type, BOOL) == 0) {
        return 2;
    }

    return 3;
}

AST* call_function(TokenList* tokens, size_t id) {
    Function* func = functions_list->functions[id];

    remove_token(tokens);
    if (strcmp(tokens->tokens[0]->type, LEFTPAREN) != 0) {
        printf("\nSyntaxError on line %i: Expected a parethesis '()'", tokens->tokens[0]->line+1);
        exit(EXIT_FAILURE);
    }
    remove_token(tokens);
    Variables* local_variables = initialize_variables();
    Variables* aux_globals = copy_variables(global_variables);
    AST* arg;
    size_t aux = 0;
    int type_id;

    while (true) {
        if (strcmp(tokens->tokens[0]->type, RIGHTPAREN) == 0) {remove_token(tokens); break;}
        if (strcmp(tokens->tokens[0]->value, COMMA) == 0) {remove_token(tokens); continue;}
        arg = logic_operators(tokens);

        type_id = get_type_id(func->arguments->variables[aux]->type);
        if (type_id != arg->item->type_id && type_id != arg->left->item->type_id) {
            printf("\nTyperror on line %i: Invalid argument", tokens->tokens[0]->line);
            exit(EXIT_FAILURE);
        }

        append_variable(local_variables, func->arguments->variables[aux]->name, func->arguments->variables[aux]->type, arg);
        aux++;
    }

    global_variables = local_variables;
    AST* res = parser(func->func_body);
    global_variables = aux_globals;
    return res;
}

void bult_in_functions(TokenList* tokens, bool *find) {
    int line = tokens->tokens[0]->line;
    if (strcmp(tokens->tokens[0]->value, "print") == 0) {
        *find = true;
        remove_token(tokens);
        if (strcmp(tokens->tokens[0]->type, LEFTPAREN) != 0) {
            printf("\nSyntaxError on line %i: Left parenthesis was expected", line+1);
            exit(EXIT_FAILURE);
        }

        AST* ast;
        AST* args[100];
        string end;
        strcpy(end, "");
        int count = 0;
        remove_token(tokens);
        while (tokens->size > 0) {
            if (strcmp(tokens->tokens[0]->value, COMMA) == 0) {remove_token(tokens);}
            if (strcmp(tokens->tokens[0]->type, RIGHTPAREN) == 0) {break;}
            if (strcmp(tokens->tokens[0]->value, "conclude") == 0 && strcmp(tokens->tokens[1]->type, EQ) == 0) {
                remove_token(tokens);
                remove_token(tokens);
                ast = logic_operators(tokens);
                strcpy(end, ast->item->value.string);
                break;
            }
            ast = logic_operators(tokens);
            args[count] = create_ast_node(ast->item, ast->left, ast->right);
            count++;
        }

        processString(end);
        for (int i = 0; i < count; i++) {
            ast = args[i];
            if (ast->item != NULL) {
                if (ast->item->type_id != 3) {
                    if (ast->item->type_id == 2) {
                        bool res = evaluate_ast(ast);
                        printf("%s", res ? "True" : "False");
                    } else {
                        printf("%g", evaluate_ast(ast));
                    }
                } else {
                    processString(ast->item->value.string);
                    printf("%s", ast->item->value.string);
                }
            }
            printf("%s", end);
        }
        if (strcmp(end, "") == 0) {printf("\n");}
    }

    if (strcmp(tokens->tokens[0]->value, "exit") == 0 || strcmp(tokens->tokens[0]->value, "close") == 0) {
        remove_token(tokens);
        if (strcmp(tokens->tokens[0]->type, LEFTPAREN) != 0) {
            printf("\nSyntaxError on line %i: Left parenthesis was expected", line+1);
            exit(EXIT_FAILURE);
        }
        remove_token(tokens);
        if (strcmp(tokens->tokens[0]->type, RIGHTPAREN) != 0) {
            printf("\nSyntaxError on line %i: Right parenthesis was expected", line+1);
            exit(EXIT_FAILURE);
        }
        print_executed_time();
        exit(0);
    }

    if (strcmp(tokens->tokens[0]->value, "clear") == 0) {
        *find = true;
        remove_token(tokens);
        if (strcmp(tokens->tokens[0]->type, LEFTPAREN) != 0) {
            printf("\nSyntaxError on line %i: Left parenthesis was expected", line+1);
            exit(EXIT_FAILURE);
        }
        remove_token(tokens);
        if (strcmp(tokens->tokens[0]->type, RIGHTPAREN) != 0) {
            printf("\nSyntaxError on line %i: Right parenthesis was expected", line+1);
            exit(EXIT_FAILURE);
        }
        remove_token(tokens);
        printf("\n");
        system("clear");
    }
}