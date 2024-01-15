#include "language_core.h"

void processString(string str) {
    size_t length = strlen(str);
    char result[2 * length]; // Twice the length to ensure enough space

    size_t i, j = 0;
    for (i = 0; i < length; ++i) {
        if (str[i] == '\\' && i + 1 < length) {
            switch (str[i + 1]) {
                case 'n':
                    result[j++] = '\n'; // Replace "\n" with a newline
                    ++i; // Move an extra character ahead
                    break;
                case 't':
                    result[j++] = '\t'; // Replace "\t" with a tab
                    ++i; // Move an extra character ahead
                    break;
                default:
                    result[j++] = str[i];
            }
        } else {
            result[j++] = str[i];
        }
    }

    result[j] = '\0'; // Ensure the resulting string is null-terminated

    // Copy the result back to the original string
    strcpy(str, result);
}

double evaluate_ast(AST *ast_node) {
    if (ast_node == NULL || ast_node->item == NULL) {
        return 0;
    }

    if (ast_node->item->type_id == 1 || ast_node->item->type_id == 4) {
        if (strcmp(ast_node->item->value.string, ADD) == 0) {
            return evaluate_ast(ast_node->left) + evaluate_ast(ast_node->right);
        } else if (strcmp(ast_node->item->value.string, SUB) == 0) {
            return evaluate_ast(ast_node->left) - evaluate_ast(ast_node->right);
        } else if (strcmp(ast_node->item->value.string, MUL) == 0) {
            return evaluate_ast(ast_node->left) * evaluate_ast(ast_node->right);
        } else if (strcmp(ast_node->item->value.string, DIV) == 0) {
            return evaluate_ast(ast_node->left) / evaluate_ast(ast_node->right);
        } else if (strcmp(ast_node->item->value.string, POW) == 0) {
            return pow(evaluate_ast(ast_node->left), evaluate_ast(ast_node->right));
        } else {
            return ast_node->item->value.num;
        }
    } else if (ast_node->item->type_id == 2 || ast_node->item->type_id == 5) {
        if (strcmp(ast_node->item->value.string, AND) == 0) {
            return evaluate_ast(ast_node->left) && evaluate_ast(ast_node->right);
        } else if (strcmp(ast_node->item->value.string, OR) == 0) {
            return evaluate_ast(ast_node->left) || evaluate_ast(ast_node->right);
        } else if (strcmp(ast_node->item->value.string, COMPARISON) == 0) {
            return evaluate_ast(ast_node->left) == evaluate_ast(ast_node->right);
        } else if (strcmp(ast_node->item->value.string, NOT_EQUAL) == 0) {
            return evaluate_ast(ast_node->left) != evaluate_ast(ast_node->right);
        } else if (strcmp(ast_node->item->value.string, LOWER_THAN) == 0) {
            return evaluate_ast(ast_node->left) < evaluate_ast(ast_node->right);
        } else if (strcmp(ast_node->item->value.string, GREATER_THAN) == 0) {
            return evaluate_ast(ast_node->left) > evaluate_ast(ast_node->right);
        } else if (strcmp(ast_node->item->value.string, LOWER_OR_EQUAL) == 0) {
            return evaluate_ast(ast_node->left) <= evaluate_ast(ast_node->right);
        } else if (strcmp(ast_node->item->value.string, GREATER_OR_EQUAL) == 0) {
            return evaluate_ast(ast_node->left) >= evaluate_ast(ast_node->right);
        } else {
            return ast_node->item->value.boolean;
        }
    }

    return 0;
}
