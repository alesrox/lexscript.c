#include "language_core.h"
#include "lexer.c"
#include "parser.c"
#include "eval.c"
#include "management.c"
#include "flow_control.c"

void print_executed_time() {
    end_time = clock();
    time_taken = ((double)(end_time - start_time)) / CLOCKS_PER_SEC;
    printf("\n\nExecution time: %f seconds\n", time_taken);
}

void debug(char* code, TokenList* tokens) {
    printf("Code:\n%s\n----------\n", code);
    printf("Tokens:\n");
    printf("Line 1: ");
    print_tokens(tokens);
    printf("\n----------\nParser:\n");
};

int main(int argc, char *argv[]) {
   // Get the file
    if (argc < 2) {
        printf("Use: %s <name_file>\n", argv[0]);
        return 1;
    }

    FILE *file = fopen(argv[1], "r");
    if (file == NULL) {
        printf("File couldn't be opened.\n");
        return 1;
    }

    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    char *code = (char *)malloc((file_size + 1) * sizeof(char));
    if (code == NULL) {
        fclose(file);
        printf("Memory allocation error.\n");
        return 1;
    }

    fread(code, sizeof(char), file_size, file);
    fclose(file);


    // Lexer
    TokenList* tokens = lexer(code);
    if (argv[2] != NULL && strcmp(argv[2], "--debug") == 0) debug(code, tokens);

    // Parser
    start_time = clock();
    global_variables = initialize_variables();
    functions_list = initialize_functions();
    parser(tokens);

    print_executed_time();
    return 0;
}