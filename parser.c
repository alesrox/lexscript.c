#include "language_core.h"

// Print AST
void print_ast(AST* root) {
    if (root != NULL) {
        if (root->left != NULL || root->right != NULL) {
            printf("( ");
        }

        print_ast(root->left);

        if (root->item != NULL) {
            if (root->item->type_id >= 3) {
                printf("%s ", root->item->value.string);
            } else {
                printf("%.16g ", root->item->value.num);
            }
        }

        print_ast(root->right);

        if (root->left != NULL || root->right != NULL) {
            printf(") ");
        }
    }
}

// Create AST
AST* init_ast_node() {
    AST* node = (AST*)malloc(sizeof(AST));
    if (node != NULL) {
        node->item = NULL;
        node->left = NULL;
        node->right = NULL;
    }
    return node;
}

AST* create_ast_node(Result* value, AST* left, AST* right) {
    AST* node = (AST*)malloc(sizeof(AST));
    if (node != NULL) {
        node->item = value;
        node->left = left;
        node->right = right;
    }
    return node;
}

// Function to free the AST memory
void free_ast(AST* root) {
    if (root != NULL) {
        free_ast(root->left);
        free_ast(root->right);
        free(root->item);
        free(root);
    }
}

AST* logic_operators(TokenList* tokens) {
    AST* ast_node = logic_comparisions(tokens);
    Result* result = (Result*)malloc(sizeof(Result));

    while (tokens->size > 1) {
        if ((strcmp(tokens->tokens[0]->type, AND) != 0 && strcmp(tokens->tokens[0]->type, OR) != 0)) {break;}
        result->type_id = 5;
        strcpy(result->value.string, tokens->tokens[0]->type);
        remove_token(tokens);

        AST* right_node = logic_comparisions(tokens);

        ast_node = create_ast_node(result, ast_node, right_node);
    }

    return ast_node;
}

AST* logic_comparisions(TokenList* tokens) {
    AST* ast_node = add_sub_ast(tokens);
    Result* result = (Result*)malloc(sizeof(Result));

    while (tokens->size > 1) {
        if (
            strcmp(tokens->tokens[0]->type, COMPARISON) != 0 &&
            strcmp(tokens->tokens[0]->type, NOT_EQUAL) != 0 &&
            strcmp(tokens->tokens[0]->type, LOWER_THAN) != 0 &&
            strcmp(tokens->tokens[0]->type, GREATER_THAN) != 0 &&
            strcmp(tokens->tokens[0]->type, LOWER_OR_EQUAL) != 0 &&
            strcmp(tokens->tokens[0]->type, GREATER_OR_EQUAL) != 0
        ) {break;}
        result->type_id = 5;
        strcpy(result->value.string, tokens->tokens[0]->type);
        remove_token(tokens);

        AST* right_node = add_sub_ast(tokens);
        ast_node = create_ast_node(result, ast_node, right_node);
    }

    return ast_node;
}

AST* add_sub_ast(TokenList* tokens) {
    AST* ast_node = mul_div_ast(tokens);
    Result* result = (Result*)malloc(sizeof(Result));

    while (tokens->size > 1) {
        if ((strcmp(tokens->tokens[0]->type, ADD) != 0 && strcmp(tokens->tokens[0]->type, SUB) != 0)) {break;}
        result->type_id = 4;
        strcpy(result->value.string, tokens->tokens[0]->type);
        remove_token(tokens);

        AST* right_node = mul_div_ast(tokens);
        ast_node = create_ast_node(result, ast_node, right_node);
    }

    return ast_node;
}

AST* mul_div_ast(TokenList* tokens) {
    AST* ast_node = pow_ast(tokens);
    Result* result = (Result*)malloc(sizeof(Result));

    while (tokens->size > 0) {
        if ((strcmp(tokens->tokens[0]->type, MUL) != 0 && strcmp(tokens->tokens[0]->type, DIV) != 0)) {break;}
        result->type_id = 4;
        strcpy(result->value.string, tokens->tokens[0]->type);
        remove_token(tokens);
        
        AST* right_node = pow_ast(tokens);
        ast_node = create_ast_node(result, ast_node, right_node);
    }

    return ast_node;
}

AST* pow_ast(TokenList* tokens) {
    AST* ast_node = non_operators(tokens);
    Result* result = (Result*)malloc(sizeof(Result));

    while (tokens->size > 0) {
        if (strcmp(tokens->tokens[0]->type, POW) != 0) {break;}
        result->type_id = 4;
        strcpy(result->value.string, tokens->tokens[0]->type);
        remove_token(tokens);

        AST* right_node = non_operators(tokens);
        ast_node = create_ast_node(result, ast_node, right_node);
    }

    return ast_node;
}

AST* non_operators(TokenList* tokens) {
    AST* ast_node = NULL;
    Result* result = (Result*)malloc(sizeof(Result));
    int line = tokens->tokens[0]->line;

    if (tokens->size > 0) {
        if (strcmp(tokens->tokens[0]->type, INT) == 0 
        || strcmp(tokens->tokens[0]->type, FLOAT) == 0) {
            result->type_id = 1;
            result->value.num = atof(tokens->tokens[0]->value);

            tokens->tokens++;
            tokens->size--;

            ast_node = create_ast_node(result, NULL, NULL);
        } else if (strcmp(tokens->tokens[0]->type, BOOLEAN) == 0) {
            Token* value_token = tokens->tokens[0];
            result->type_id = 2;
            result->value.boolean = strcmp(value_token->value, TRUE) == 0 ? true : false;

            tokens->tokens++;
            tokens->size--;

            ast_node = create_ast_node(result, NULL, NULL);
        } else if (strcmp(tokens->tokens[0]->type, STRING) == 0) {
            result->type_id = 3;
            strcpy(result->value.string, tokens->tokens[0]->value);

            tokens->tokens++;
            tokens->size--;

            ast_node = create_ast_node(result, NULL, NULL);
        } else if (strcmp(tokens->tokens[0]->type, IDENTIFIER) == 0) {
            bool find = false;

            for (size_t i = 0; i < global_variables->size; i++) {
                if (strcmp(global_variables->variables[i]->name, tokens->tokens[0]->value) == 0) {
                    if (strcmp(global_variables->variables[i]->type, NUMBER) == 0) {
                        result->type_id = 1;
                        result->value.num = evaluate_ast(global_variables->variables[i]->expresion);
                    }
                    if (strcmp(global_variables->variables[i]->type, BOOL) == 0) {
                        result->type_id = 2;
                        result->value.boolean = evaluate_ast(global_variables->variables[i]->expresion);
                    }
                    if (strcmp(global_variables->variables[i]->type, STRING) == 0) {
                        result->type_id = 3;
                        strcpy(result->value.string, global_variables->variables[i]->expresion->item->value.string);
                    }
                    
                    ast_node = create_ast_node(result, NULL, NULL);
                    find = true;
                    break;
                }
            }

            if (!find) find = bult_in_functions(tokens, &ast_node);

            if (!find) {
                for (size_t i = 0; i < functions_list->size; i++) {
                    if (strcmp(functions_list->functions[i]->name, tokens->tokens[0]->value) == 0) {
                        ast_node = call_function(tokens, i);
                        return ast_node;
                    }
                }
            }

            if (!find) {
                printf("\nSyntaxError on line %i: Undeclared identifier or function -> %s", line+1, tokens->tokens[0]->value);
                exit(EXIT_FAILURE);
            }

            tokens->tokens++;
            tokens->size--;
            return ast_node;
        } else if (strcmp(tokens->tokens[0]->type, LEFTPAREN) == 0) {
            remove_token(tokens);
            AST* res = logic_operators(tokens);
            if (strcmp(tokens->tokens[0]->type, RIGHTPAREN) != 0) {
                printf("\nSyntaxError on line %i: Right parenthesis was expected", line+1);
                exit(EXIT_FAILURE);
            }
            remove_token(tokens);
            return res;
        } else if (strcmp(tokens->tokens[0]->type, NEW_LINE) == 0) {
            tokens->tokens++;
            tokens->size--;
        } else if (strcmp(tokens->tokens[0]->type, ENDPROG) == 0) {
            return ast_node;
        } else {
            printf("\nSyntaxError on line %i: Unexpected symbol -> %s", line+1, tokens->tokens[0]->value);
            exit(EXIT_FAILURE);
        }
    } 
    
    return ast_node;
}

AST* parser(TokenList* tokens) {
    AST* ast = (AST*)malloc(sizeof(AST));
    TokenList* current_tokens = malloc(sizeof(TokenList));
    current_tokens->tokens = tokens->tokens;
    current_tokens->size = tokens->size;

    Token* token = NULL;
    string aux;

    while (current_tokens->size != 0) {
        if (returned) break;
        token = current_tokens->tokens[0];
        strcpy(aux, "");
        if (current_tokens->size > 1) {strcpy(aux, current_tokens->tokens[1]->type);}
    
        if (strcmp(token->value, NUMBER) == 0
            || strcmp(token->value, BOOL) == 0
            || strcmp(token->value, STRING) == 0) {
            string type; 
            strcpy(type, token->value);
            TokenList* aux_tokens = malloc(sizeof(TokenList));
            aux_tokens->tokens = current_tokens->tokens;
            aux_tokens->size = current_tokens->size;

            assigment(aux_tokens, type);
            current_tokens = aux_tokens;
        } else if (strcmp(token->value, FUNCTION) == 0) {
            remove_token(tokens);
            define_funciton(current_tokens);
        } else if (strcmp(token->type, IDENTIFIER) == 0 && strstr(aux, "EQ") != NULL) {
            if (strcmp(aux, EQ) == 0) {
                bool find = false;
                for (size_t i = 0; i < global_variables->size; i++) {
                    if (strcmp(global_variables->variables[i]->name, token->value) == 0) {
                        assigment(current_tokens, global_variables->variables[i]->type);
                        find = true;
                        break;
                    }
                }

                if (!find) {
                    printf("\nSyntaxError on line %i: Undeclared identifier -> %s", token->line+1, token->value);
                    exit(EXIT_FAILURE);
                }
            } else {
                Token* eq_op = current_tokens->tokens[1];
                size_t var_id;
                remove_token(current_tokens);
                remove_token(current_tokens);

                for (var_id = 0; var_id < global_variables->size; var_id++) {
                    if (strcmp(global_variables->variables[var_id]->name, token->value) == 0) {
                        if (strcmp(global_variables->variables[var_id]->type, NUMBER) == 0) {
                            string value;
                            if (strcmp(eq_op->type, EQ_ADD) == 0) {
                                prepend_token(current_tokens, create_token(ADD, "+", token->line));
                                sprintf(value, "%.16g", evaluate_ast(global_variables->variables[var_id]->expresion));
                                prepend_token(current_tokens, create_token(FLOAT, value, token->line));
                                global_variables->variables[var_id]->expresion = logic_operators(current_tokens);
                            } else if (strcmp(eq_op->type, EQ_SUB) == 0) {
                                prepend_token(current_tokens, create_token(SUB, "-", token->line));
                                sprintf(value, "%.16g", evaluate_ast(global_variables->variables[var_id]->expresion));
                                prepend_token(current_tokens, create_token(FLOAT, value, token->line));
                                global_variables->variables[var_id]->expresion = logic_operators(current_tokens);
                            } else if (strcmp(eq_op->type, EQ_MUL) == 0) {
                                prepend_token(current_tokens, create_token(MUL, "*", token->line));
                                sprintf(value, "%.16g", evaluate_ast(global_variables->variables[var_id]->expresion));
                                prepend_token(current_tokens, create_token(FLOAT, value, token->line));
                                global_variables->variables[var_id]->expresion = logic_operators(current_tokens);
                            } else if (strcmp(eq_op->type, EQ_DIV) == 0) {
                                prepend_token(current_tokens, create_token(DIV, "/", token->line));
                                sprintf(value, "%.16g", evaluate_ast(global_variables->variables[var_id]->expresion));
                                prepend_token(current_tokens, create_token(FLOAT, value, token->line));
                                global_variables->variables[var_id]->expresion = logic_operators(current_tokens);
                            } else if (strcmp(eq_op->type, EQ_MOD) == 0) {
                                prepend_token(current_tokens, create_token(MOD, "%", token->line));
                                sprintf(value, "%.16g", evaluate_ast(global_variables->variables[var_id]->expresion));
                                prepend_token(current_tokens, create_token(FLOAT, value, token->line));
                                global_variables->variables[var_id]->expresion = logic_operators(current_tokens);
                            } else if (strcmp(eq_op->type, EQ_POW) == 0) {
                                prepend_token(current_tokens, create_token(POW, "^", token->line));
                                sprintf(value, "%.16g", evaluate_ast(global_variables->variables[var_id]->expresion));
                                prepend_token(current_tokens, create_token(FLOAT, value, token->line));
                                global_variables->variables[var_id]->expresion = logic_operators(current_tokens);
                            }
                        } else {
                            printf("\nSyntaxError on line %i: var %s expected to be a number", token->line+1, token->value);
                            exit(EXIT_FAILURE);
                        }
                    }
                }
            }
        } else if (strcmp(token->type, NEW_LINE) == 0 || strcmp(token->value, END) == 0) {
            remove_token(current_tokens);
        } else if (strcmp(token->value, IF) == 0) {
            ast = if_statement(current_tokens);
        } else if (strcmp(token->value, WHILE) == 0) {
            remove_token(current_tokens);
            while_statement(current_tokens);
        } else if (strcmp(token->value, FOR) == 0) {
            remove_token(current_tokens);
            for_statement(current_tokens);
        } else if (strcmp(token->type, ENDPROG) == 0) {
            remove_token(current_tokens);
        } else if (strcmp(current_tokens->tokens[0]->value, RETURN) == 0) {
            remove_token(current_tokens);
            ast = logic_operators(current_tokens);
            return ast;
        } else {
            ast = logic_operators(current_tokens);
        }
    }

    free(current_tokens);
    return ast;
}