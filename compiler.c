#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct Token {
    char type[20];
    char value[100];
} Token;

typedef struct Node {
    char type[20];
    char name[100];
    char value[100];
    struct Node* next;
} Node;

Token* lexer(const char* input, int* tokenCount) {
    Token* tokens = malloc(100 * sizeof(Token));
    int cursor = 0;
    int index = 0;
    int length = strlen(input);

    while (cursor < length) {
        char char_ = input[cursor];

        // Skip whitespace
        if (isspace(char_)) {
            cursor++;
            continue;
        }

        if (isalpha(char_)) {
            char word[100] = "";
            while (isalnum(char_)) {
                strncat(word, &char_, 1);
                char_ = input[++cursor];
            }
            if (strcmp(word, "dr") == 0 || strcmp(word, "drpr") == 0) {
                strcpy(tokens[index].type, "keyword");
            } else {
                strcpy(tokens[index].type, "identifier");
            }
            strcpy(tokens[index].value, word);
            index++;
            continue;
        }

        if (isdigit(char_)) {
            char number[100] = "";
            while (isdigit(char_)) {
                strncat(number, &char_, 1);
                char_ = input[++cursor];
            }
            strcpy(tokens[index].type, "number");
            strcpy(tokens[index].value, number);
            index++;
            continue;
        }

        // Tokenize operators and equals sign
        if (strchr("+-*/=", char_)) {
            strcpy(tokens[index].type, "operator");
            tokens[index].value[0] = char_;
            tokens[index].value[1] = '\0';
            cursor++;
            index++;
            continue;
        }
    }

    *tokenCount = index;
    return tokens;
}

Node* parser(Token* tokens, int tokenCount) {
    Node* ast = malloc(sizeof(Node));
    strcpy(ast->type, "Program");
    ast->next = NULL;
    Node* current = ast;

    for (int i = 0; i < tokenCount; i++) {
        if (strcmp(tokens[i].type, "keyword") == 0 && strcmp(tokens[i].value, "dr") == 0) {
            Node* declaration = malloc(sizeof(Node));
            strcpy(declaration->type, "Declaration");
            strcpy(declaration->name, tokens[++i].value);
            strcpy(declaration->value, "");

            if (strcmp(tokens[i + 1].type, "operator") == 0 && strcmp(tokens[i + 1].value, "=") == 0) {
                i += 2;
                char expression[100] = "";
                while (i < tokenCount && strcmp(tokens[i].type, "keyword") != 0) {
                    strcat(expression, tokens[i].value);
                    i++;
                }
                i--;
                strcpy(declaration->value, expression);
            }

            current->next = declaration;
            current = declaration;
        } else if (strcmp(tokens[i].type, "keyword") == 0 && strcmp(tokens[i].value, "drpr") == 0) {
            Node* printStatement = malloc(sizeof(Node));
            strcpy(printStatement->type, "PrintStatement");
            strcpy(printStatement->name, tokens[++i].value);

            current->next = printStatement;
            current = printStatement;
        }
    }

    return ast;
}

void codeGen(Node* node, char* code) {
    Node* current = node->next;
    while (current != NULL) {
        if (strcmp(current->type, "Declaration") == 0) {
            sprintf(code + strlen(code), "const int %s = %s;\n", current->name, current->value);
        } else if (strcmp(current->type, "PrintStatement") == 0) {
            sprintf(code + strlen(code), "printf(\"%%d\\n\", %s);\n", current->name);
        }
        current = current->next;
    }
}

void compiler(const char* input, char* code) {
    int tokenCount;
    Token* tokens = lexer(input, &tokenCount);
    Node* ast = parser(tokens, tokenCount);

    sprintf(code, "#include <stdio.h>\n\nint main() {\n");
    codeGen(ast, code);
    sprintf(code + strlen(code), "return 0;\n}\n");

    free(tokens);
}

int main() {
    char code[1024] = "";
    char line[256];
    char generatedCode[2048] = "";

    printf("Enter your code (end with an empty line):\n");
    while (1) {
        fgets(line, sizeof(line), stdin);
        if (strcmp(line, "\n") == 0) {
            break;
        }
        strcat(code, line);
    }

    compiler(code, generatedCode);

    // Write the generated code to a temporary file
    FILE* tempFile = fopen("/tmp/temp_generated_code.c", "w");
    if (tempFile == NULL) {
        perror("Failed to create temporary file");
        exit(1);
    }
    fprintf(tempFile, "%s", generatedCode);
    fclose(tempFile);

    // Compile and run the generated code using popen
    char command[4096];
    sprintf(command, "gcc /tmp/temp_generated_code.c -o /tmp/temp_generated_code && /tmp/temp_generated_code");
    FILE* fp = popen(command, "r");
    if (fp == NULL) {
        perror("Failed to run command");
        exit(1);
    }

    // Print the output of the generated code
    char output[1035];
    while (fgets(output, sizeof(output), fp) != NULL) {
        printf("%s", output);
    }

    pclose(fp);

    // Remove the temporary file
    remove("/tmp/temp_generated_code.c");

    return 0;
}