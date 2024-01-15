#include "language_core.h"

void print_tokens(TokenList* tokens) {
    for (size_t i = 0; i < tokens->size; ++i) {
        if (strcmp(tokens->tokens[i]->type, NEW_LINE) == 0) {
            printf("\n");
            printf("Line %d: ", tokens->tokens[i]->line+2);
        } else {
            printf("(Type: %s, Value: %s) ", tokens->tokens[i]->type, tokens->tokens[i]->value);
        }
    }

    printf("\n");
}

// CREATE TOKENS
TokenList* initialize_token_list() {
    TokenList* list = malloc(sizeof(TokenList));
    if (list != NULL) {
        list->tokens = NULL;
        list->size = 0;
    }
    return list;
}

// Remove the first token of a tokenlist
void remove_token(TokenList* list) {
    //free(list->tokens[0]);
    if (list->size > 0 || strcmp(list->tokens[0]->type, "ENDPROG") == 0) {
        list->tokens++;
        list->size--;
    }
}

// Append a token on a tokenlist
void append_token(TokenList* list, Token* token) {
    list->tokens = realloc(list->tokens, (list->size + 1) * sizeof(Token*));
    if (list->tokens == NULL) {
        fprintf(stderr, "Error: Could not allocate memory for token list.\n");
        exit(EXIT_FAILURE);
    }

    list->tokens[list->size++] = token;
}

void prepend_token(TokenList* list, Token* token) {
    // Create a new array of tokens with an increased size
    Token** new_tokens = (Token**)malloc((list->size + 1) * sizeof(Token*));

    // Add the new token to the beginning
    new_tokens[0] = token;

    // Copy the existing tokens to the new array
    for (size_t i = 0; i < list->size; ++i) {
        new_tokens[i + 1] = list->tokens[i];
    }

    // Update the token pointer and size in the TokenList structure
    list->tokens = new_tokens;
    list->size += 1;
}


void free_token_list(TokenList* list) {
    if (list != NULL) {
        if (list->tokens != NULL) {
            for (size_t i = 0; i < list->size; ++i) {
                free(list->tokens[i]);
            }
            free(list->tokens);
        }
        free(list);
    }
}

// Function to clone a token
Token* clone_token(Token* original) {
    if (original == NULL) {
        return NULL;
    }

    // Create a new token and copy the information
    Token* clone = malloc(sizeof(Token));
    if (clone == NULL) {
        fprintf(stderr, "Error: Could not allocate memory for the Token clone.\n");
        exit(EXIT_FAILURE);
    }

    // Copy the specific information of the Token
    strcpy(clone->type, original->type);
    strcpy(clone->value, original->value);
    clone->line = original->line;

    return clone;
}

// Function to clone a list of tokens
TokenList* clone_token_list(TokenList* original) {
    if (original == NULL) {
        return NULL;
    }

    // Create a new list and copy the information
    TokenList* clone = malloc(sizeof(TokenList));
    if (clone == NULL) {
        fprintf(stderr, "Error: Could not allocate memory for TokenList clone.\n");
        exit(EXIT_FAILURE);
    }

    // Initialize the cloned list
    clone->size = original->size;
    clone->tokens = malloc(clone->size * sizeof(Token*));
    if (clone->tokens == NULL) {
        fprintf(stderr, "Error: Could not allocate memory for cloned tokens.\n");
        exit(EXIT_FAILURE);
    }

    // Clone each token in the original list
    for (size_t i = 0; i < original->size; ++i) {
        clone->tokens[i] = clone_token(original->tokens[i]);
    }

    return clone;
}

Token* create_token(char* type, char* value, int line) {
    Token* token = malloc(sizeof(Token));
    strcpy(token->type, type);
    if (value != NULL) {
        strcpy(token->value, value);
    } else {
        token->value[0] = '\0';
    }
    token->line = line;
    return token;
}


Token* make_number(char* code, int *position, int num_line) {
    char num[20] = "";
    int dot = 0;
    char current_char = code[*position];

    while (current_char != '\0' && current_char != '#' && strchr(DIGITS, current_char) != NULL) {
        if (current_char == '.') {
            if (dot) break;
            dot = 1;
        }
        strncat(num, &current_char, 1);
        (*position)++;
        current_char = code[*position];
    }

    Token* token_number = (Token*)malloc(sizeof(Token));
    if (token_number == NULL) {
        fprintf(stderr, "Memory allocation error.\n");
        exit(EXIT_FAILURE);
    }

    if (dot) {
        strcpy(token_number->type, FLOAT);
    } else {
        strcpy(token_number->type, INT);
    }

    strcpy(token_number->value, num);
    token_number->line = num_line;

    (*position)--;
    return token_number;
}


Token* make_string(char* current_char, char* code, int* position, int num_line) {
    char* string = (char*)malloc(sizeof(char) * 100);
    strcpy(string, "\"");

    (*position)++;
    *current_char = code[*position];

    while (*current_char != '\0' || *current_char != '#') {
        char str[2] = {*current_char, '\0'};
        strcat(string, str);

        if (*current_char == '"') break;

        (*position)++;
        *current_char = code[*position];
    }

    if (string[0] == '"' && string[strlen(string) - 1] == '"') {
        char* trimmed_string = (char*)malloc(sizeof(char) * (strlen(string)));
        strncpy(trimmed_string, string + 1, strlen(string) - 2);
        trimmed_string[strlen(string) - 2] = '\0';

        Token* token = (Token*)malloc(sizeof(Token));
        strcpy(token->type, STRING);
        strcpy(token->value, trimmed_string);
        token->line = num_line;

        free(string);
        return token;
    } else {
        printf("\nSyntax Error on line %d: Expected a '\"'\n", get_num_line());
        exit(-1);
    }
}

Token* make_identifier(char current_char, char* code, int* old_position, int num_line) {
    char id_str[100];
    strcpy(id_str, "");
    int position = *old_position;

    while (current_char != '\0' && strchr(LETTERS_NUMBER, current_char) != NULL) {
        strncat(id_str, &current_char, 1);
        position++;
        current_char = code[position];
    }

    *old_position = position - 1;

    char id_str_upper[100] = "";
    int longitud = strlen(id_str);
    
    for (int i = 0; i < longitud; i++) {
        id_str_upper[i] = toupper(id_str[i]);
    }

    // VARIABLES
    if (strcmp(id_str_upper, NUMBER) == 0) {
        return create_token(KEYWORD, NUMBER, num_line);
    } else if (strcmp(id_str_upper, STRING) == 0) {
        return create_token(KEYWORD, STRING, num_line);
    } else if (strcmp(id_str_upper, BOOL) == 0) {
        return create_token(KEYWORD, BOOL, num_line);
    } else if (strcmp(id_str, TRUE) == 0 || strcmp(id_str, FALSE) == 0) {
        return create_token(BOOLEAN, id_str, num_line);
    } else if (strcmp(id_str_upper, FUNCTION) == 0) {
        return create_token(KEYWORD, FUNCTION, num_line);
    } else if (strcmp(id_str_upper, DOES) == 0) {
        return create_token(KEYWORD, DOES, num_line);
    } else if (strcmp(id_str_upper, RETURN) == 0) {
        return create_token(KEYWORD, RETURN, num_line);
    } else if (strcmp(id_str_upper, END) == 0) {
        return create_token(KEYWORD, END, num_line);
    } else if (strcmp(id_str_upper, GLOBAL) == 0) {
        return create_token(KEYWORD, GLOBAL, num_line);
    } else if (strcmp(id_str_upper, BREAK) == 0) {
        return create_token(KEYWORD, BREAK, num_line);
    } else if (strcmp(id_str_upper, CONTINUE) == 0) {
        return create_token(KEYWORD, CONTINUE, num_line);
    }

    // LOGIC OPERATORS
    if (strcmp(id_str_upper, AND) == 0) {
        return create_token(AND, NULL, num_line);
    } else if (strcmp(id_str_upper, OR) == 0) {
        return create_token(OR, NULL, num_line);
    } else if (strcmp(id_str_upper, NOT) == 0) {
        return create_token(NOT, NULL, num_line);
    }

    // IF-ELSE STATEMENT
    if (strcmp(id_str_upper, IF) == 0) {
        return create_token(STATEMENT, IF, num_line);
    } else if (strcmp(id_str_upper, THEN) == 0) {
        return create_token(STATEMENT, THEN, num_line);
    } else if (strcmp(id_str_upper, ELSE) == 0) {
        return create_token(STATEMENT, ELSE, num_line);
    } else if (strcmp(id_str_upper, ELIF) == 0) {
        return create_token(STATEMENT, ELIF, num_line);
    }

    // WHILE
    if (strcmp(id_str_upper, WHILE) == 0) {
        return create_token(STATEMENT, WHILE, num_line);
    }

    // FOR
    if (strcmp(id_str_upper, FOR) == 0) {
        return create_token(STATEMENT, FOR, num_line);
    } else if (strcmp(id_str_upper, TO) == 0) {
        return create_token(STATEMENT, TO, num_line);
    }

    // DEFAULT
    return create_token(IDENTIFIER, id_str, num_line);
}

char next_char(const char* code, int *position) {
    (*position)++;
    return code[*position];
}


bool is_in_list(char current_char, char* group) {
    if (strchr(group, current_char) != NULL) {
        return true;
    } else {
        return false;
    }
}

TokenList* lexer(char* code) {
    char ENDFILE = '\0';
    code[strlen(code)] = ENDFILE;

    TokenList* tokens = initialize_token_list();
    int position = 0;
    int num_line = 0;
    char current_char = code[position];

    while (current_char != ENDFILE) {
        if (current_char == '#') {
            while (current_char != ENDFILE && current_char != '\n') {
                current_char = next_char(code, &position);
            }

            if (current_char == ENDFILE) break;
            if (current_char == '\n') {
                num_line++;
            }
        } else if (current_char == ' ' || current_char == '\t') {
            current_char = next_char(code, &position);
            continue;
        } else if (current_char == '\n') {
            append_token(tokens, create_token(NEW_LINE, "\n", num_line));
            num_line++;
        } else if (is_in_list(current_char, DIGITS)) {
            Token* num_token = make_number(code, &position, num_line);
            append_token(tokens, num_token);
        } else if (is_in_list(current_char, LETTERS)) {
            Token* identifier_token = make_identifier(current_char, code, &position, num_line);
            append_token(tokens, identifier_token);
        } else if (current_char == '"') {
            Token* string_token = make_string(&current_char, code, &position, num_line);
            append_token(tokens, string_token);
        } else if (current_char == '=') {
            if (code[position+1] == '=') {
                current_char = next_char(code, &position);
                append_token(tokens, create_token(COMPARISON, "==", num_line));
            } else {
                append_token(tokens, create_token(EQ, "=", num_line));
            }
        } else if (current_char == '+') {
            if (code[position+1] == '=') {
                current_char = next_char(code, &position);
                append_token(tokens, create_token(EQ_ADD, "+=", num_line));
                current_char = next_char(code, &position);
                continue;
            }
            append_token(tokens, create_token(ADD, "+", num_line));
        } else if (current_char == '-') {
            if (code[position+1] == '>') {
                current_char = next_char(code, &position);
                append_token(tokens, create_token(KEYWORD, ALT_DOES, num_line));
            } else if (code[position+1] == '=') {
                current_char = next_char(code, &position);
                append_token(tokens, create_token(EQ_SUB, "-=", num_line));
            } else if (tokens->size > 0) {
                if (strcmp(tokens->tokens[tokens->size - 1]->type, INT) == 0
                || strcmp(tokens->tokens[tokens->size - 1]->type, FLOAT) == 0
                || strcmp(tokens->tokens[tokens->size - 1]->type, IDENTIFIER) == 0) {
                    append_token(tokens, create_token(SUB, "-", num_line));
                }
            } else {
                Token* num_token = make_number(code, &position, num_line);
                string negative_num;
                strcpy(negative_num, "-");
                strcat(negative_num, num_token->value);
                append_token(tokens, create_token(num_token->type, negative_num, num_token->line));
            }
        } else if (current_char == '*') {
            if (code[position+1] == '=') {
                current_char = next_char(code, &position);
                append_token(tokens, create_token(EQ_MUL, "*=", num_line));
                current_char = next_char(code, &position);
                continue;
            }
            append_token(tokens, create_token(MUL, "*", num_line));
        } else if (current_char == '/') {
            if (code[position+1] == '=') {
                current_char = next_char(code, &position);
                append_token(tokens, create_token(EQ_DIV, "/=", num_line));
                current_char = next_char(code, &position);
                continue;
            }
            append_token(tokens, create_token(DIV, NULL, num_line));
        } else if (current_char == '^') {
            if (code[position+1] == '=') {
                current_char = next_char(code, &position);
                append_token(tokens, create_token(EQ_POW, "^=", num_line));
                current_char = next_char(code, &position);
                continue;
            }
            append_token(tokens, create_token(POW, "^", num_line));
        } else if (current_char == '%') {
            if (code[position+1] == '=') {
                current_char = next_char(code, &position);
                append_token(tokens, create_token(EQ_MOD, "%=", num_line));
                current_char = next_char(code, &position);
                continue;
            }
            append_token(tokens, create_token(MOD, "%", num_line));
        } else if (current_char == '!' && code[position+1] == '=') {
            current_char = next_char(code, &position);
            append_token(tokens, create_token(NOT_EQUAL, "!=", num_line));
        } else if (current_char == '<') {
            if (code[position+1] == '=') {
                current_char = next_char(code, &position);
                append_token(tokens, create_token(LOWER_OR_EQUAL, "<=", num_line));
                current_char = next_char(code, &position);
                continue;
            }
            append_token(tokens, create_token(LOWER_THAN, "<", num_line));
        } else if (current_char == '>') {
            if (code[position+1] == '=') {
                current_char = next_char(code, &position);
                append_token(tokens, create_token(GREATER_OR_EQUAL, ">=", num_line));
                current_char = next_char(code, &position);
                continue;
            }
            append_token(tokens, create_token(GREATER_THAN, ">", num_line));
        } else if (current_char == '(') {
            append_token(tokens, create_token(LEFTPAREN, "\"(\"", num_line));
        } else if (current_char == ')') {
            append_token(tokens, create_token(RIGHTPAREN, "\")\"", num_line));
        } else if (current_char == ',') {
            append_token(tokens, create_token(KEYWORD, COMMA, num_line));
        } else if (current_char == ':') {
            append_token(tokens, create_token(KEYWORD, ARG_TYPE, num_line));
        } else {
            set_line(num_line);
            printf("Syntax Error on line %d: Unexpected caracter %c", get_num_line(), current_char);
            exit(-1);
        }

        current_char = next_char(code, &position);
    }

    append_token(tokens, create_token(ENDPROG, ENDPROG, num_line));
    free(code);
    return tokens;
}
