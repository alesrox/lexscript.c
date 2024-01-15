#include "language_core.h"

AST* if_statement(TokenList* tokens) {
    int line = tokens->tokens[0]->line;
    remove_token(tokens);
    AST* condition = logic_operators(tokens);
    if (tokens->size == 0 || strcmp(tokens->tokens[0]->value, THEN) != 0) {
        printf("\nSyntaxError on line %i: THEN STATEMENT was expected", line+1);
        exit(EXIT_FAILURE);
    }
    remove_token(tokens);
    bool evaluated = evaluate_ast(condition);
    TokenList* inside_if_tokens = initialize_token_list();

    if (tokens->size == 0) {
        printf("\nSyntaxError on line %i: IF-EXPRESION was expected", line+1);
        exit(EXIT_FAILURE);
    }

    bool multiline = false;
    if (strcmp(tokens->tokens[0]->type, NEW_LINE) == 0) {multiline = true;}

    Token* token = malloc(sizeof(Token));
    size_t end = tokens->size;
    int statements = 0;
    for (size_t i = 0; i < end; i++) {
        token = tokens->tokens[0];

        if (strcmp(token->type, NEW_LINE) == 0) {
            if (!multiline) {break;}
            remove_token(tokens);
            continue;
        }

        if (strcmp(token->value, IF) == 0 || strcmp(token->value, WHILE) == 0 || strcmp(token->value, FOR) == 0) {
            statements++;
        }

        if (strcmp(token->value, END) == 0) {
            if (statements > 0) {
                statements--;
            } else {
                break;
            }
        }

        if (strcmp(token->value, ELSE) == 0 || strcmp(token->value, ELIF) == 0) {break;}

        append_token(inside_if_tokens, create_token(token->type, token->value, token->line));
        remove_token(tokens);
    }
    

    if (evaluated) {
        // Before parsing the tokens again, we will clear the remaining tokens from the if-else
        if (tokens->size > 0) {
            end = tokens->size;
            statements = 0;
            for (size_t i = 0; i < end; i++) {
                token = tokens->tokens[0];

                if (strcmp(token->type, NEW_LINE) == 0) {
                    if (!multiline) {break;}
                    remove_token(tokens);
                    continue;
                }

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

                remove_token(tokens);
            }
        }
        
        AST* res = parser(inside_if_tokens);
        return res;
    } else {
        if (strcmp(tokens->tokens[0]->value, ELSE) == 0) {
            multiline = false;
            line = tokens->tokens[0]->line;
            TokenList* inside_else_tokens = initialize_token_list();

            if (tokens->size == 0) {
                printf("\nSyntaxError on line %i: ELSE-EXPRESION was expected", line+1);
                exit(EXIT_FAILURE);
            }

            Token* token = malloc(sizeof(Token));
            remove_token(tokens);

            if (strcmp(tokens->tokens[0]->type, NEW_LINE) == 0) {multiline = true;}
            size_t end = tokens->size;
            statements = 0;
            for (size_t i = 0; i < end; i++) {
                token = tokens->tokens[0];

                if (strcmp(token->type, NEW_LINE) == 0) {
                    if (!multiline) {break;}
                    remove_token(tokens);
                    continue;
                }

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

                append_token(inside_else_tokens, create_token(token->type, token->value, token->line));
                remove_token(tokens);
            }
            
            return parser(inside_else_tokens);
        } else if (strcmp(tokens->tokens[0]->value, ELIF) == 0) {
            return if_statement(tokens);
        }
    }

    AST* ast;
    return ast;
}

void while_statement(TokenList* tokens) {
    int line = tokens->tokens[0]->line;
    TokenList* condition_tokens;
    size_t end = tokens->size;
    for (size_t i = 0; i < end; ++i) {
        if (strcmp(tokens->tokens[0]->value, THEN) == 0) {break;}
        append_token(condition_tokens, tokens->tokens[0]);
        remove_token(tokens);
    }

    if (tokens->size == 0 || strcmp(tokens->tokens[0]->value, THEN) != 0) {
        printf("\nSyntaxError on line %i: THEN STATEMENT was expected", line+1);
        exit(EXIT_FAILURE);
    }
    remove_token(tokens);

    TokenList* while_body = initialize_token_list();
    Token* token = malloc(sizeof(Token));
    end = tokens->size;
    int statements = 0;
    for (size_t i = 0; i < end; i++) {
        token = tokens->tokens[0];

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

        append_token(while_body, create_token(token->type, token->value, token->line));
        remove_token(tokens);
    }

    TokenList* condition_tokens_copy = clone_token_list(condition_tokens);
    TokenList* while_body_copy = clone_token_list(while_body);
    AST* condition_ast;
    bool evaluated_condition;
    while (true) {
        condition_tokens = clone_token_list(condition_tokens_copy);
        condition_ast = logic_operators(condition_tokens);
        evaluated_condition = evaluate_ast(condition_ast);
        if (!evaluated_condition) {break;}
        while_body = clone_token_list(while_body_copy);
        parser(while_body);
    }
}

void for_statement(TokenList* tokens) {
    int line = tokens->tokens[0]->line;
    Token* for_variable = tokens->tokens[0];
    
    if (strcmp(for_variable->type, IDENTIFIER) != 0) {
        printf("\nSyntaxError on line %i: FOR VAR was expected", line+1);
        exit(EXIT_FAILURE);
    }

    remove_token(tokens);
    AST* for_var_value;

    if (strcmp(tokens->tokens[0]->type, EQ) == 0) {
        remove_token(tokens);
        for_var_value = logic_operators(tokens);
    } else {
        Result* zero = (Result*)malloc(sizeof(Result));
        zero->type_id = 1;
        zero->value.num = 0;
        for_var_value->item = zero;
    }

    bool exist = false;
    for (size_t i = 0; i < global_variables->size; i++) {
        if (strcmp(global_variables->variables[i]->name, for_variable->value) == 0) {
            exist = true;
            global_variables->variables[i]->expresion = for_var_value;
            break;
        }
    }

    if (!exist) {
        append_variable(global_variables, for_variable->value, NUMBER, for_var_value);
    }

    if (strcmp(tokens->tokens[0]->value, TO) != 0) {
        printf("\nSyntaxError on line %i: TO STATEMENT was expected", line+1);
        exit(EXIT_FAILURE);
    }

    remove_token(tokens);
    AST* to_expresion = logic_operators(tokens);

    if (strcmp(tokens->tokens[0]->value, THEN) != 0) {
        printf("\nSyntaxError on line %i: THEN STATEMENT was expected", line+1);
        exit(EXIT_FAILURE);
    }
    remove_token(tokens);

    bool multiline = false;
    if (strcmp(tokens->tokens[0]->type, NEW_LINE) == 0) {
        remove_token(tokens);
        multiline = true;
    }

    TokenList* for_body = initialize_token_list();
    Token* token;
    size_t end = tokens->size;
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

        append_token(for_body, create_token(token->type, token->value, token->line));
        remove_token(tokens);
    }

    size_t for_var_id;
    
    for (for_var_id = 0; for_var_id < global_variables->size; for_var_id++) {
        if (strcmp(global_variables->variables[for_var_id]->name, for_variable->value) == 0) {break;}
    }
    
    TokenList* for_body_copy = clone_token_list(for_body);
    for (int i = evaluate_ast(for_var_value); i < evaluate_ast(to_expresion); i++) {
        for_body = clone_token_list(for_body_copy);
        global_variables->variables[for_var_id]->expresion->item->value.num++;
        parser(for_body);
    }
}