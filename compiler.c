#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct Token {
    char type[20];
    char value[100];
} Token;

// Update the Node structure to have larger buffers
typedef struct Node {
    char type[32];
    char name[256];
    char value[512];  // Increased buffer size significantly
    struct Node* next;
    struct Node* body;
    struct Node* params;
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
            if (strcmp(word, "dg") == 0 || strcmp(word, "dgpr") == 0 || strcmp(word, "if") == 0 || strcmp(word, "else") == 0 || strcmp(word, "while") == 0 || strcmp(word, "for") == 0 || strcmp(word, "do") == 0 || strcmp(word, "def") == 0 || strcmp(word, "return") == 0) {
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

        if (char_ == '"') {
            char string[100] = "";
            char_ = input[++cursor];
            while (char_ != '"') {
                strncat(string, &char_, 1);
                char_ = input[++cursor];
            }
            cursor++;
            strcpy(tokens[index].type, "string");
            strcpy(tokens[index].value, string);
            index++;
            continue;
        }

        // Tokenize operators and equals sign
        if (strchr("+-*/=<>", char_)) {
            strcpy(tokens[index].type, "operator");
            tokens[index].value[0] = char_;
            tokens[index].value[1] = '\0';
            cursor++;
            index++;
            continue;
        }

        // Tokenize parentheses and braces
        if (strchr("(){}", char_)) {
            strcpy(tokens[index].type, "punctuation");
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
        if (strcmp(tokens[i].type, "keyword") == 0 && strcmp(tokens[i].value, "dg") == 0) {
            Node* declaration = malloc(sizeof(Node));
            strcpy(declaration->type, "Declaration");
            strcpy(declaration->name, tokens[++i].value);
            strcpy(declaration->value, "");

            if (strcmp(tokens[i + 1].type, "operator") == 0 && strcmp(tokens[i + 1].value, "=") == 0) {
                i += 2;
                char expression[100] = "";
                while (i < tokenCount && strcmp(tokens[i].type, "keyword") != 0 && strcmp(tokens[i].type, "punctuation") != 0) {
                    strcat(expression, tokens[i].value);
                    i++;
                }
                i--;
                strcpy(declaration->value, expression);
            }

            current->next = declaration;
            current = declaration;
        } else if (strcmp(tokens[i].type, "keyword") == 0 && strcmp(tokens[i].value, "dgpr") == 0) {
            Node* printStatement = malloc(sizeof(Node));
            strcpy(printStatement->type, "PrintStatement");
            strcpy(printStatement->name, tokens[++i].value);

            current->next = printStatement;
            current = printStatement;
        } else if (strcmp(tokens[i].type, "keyword") == 0 && strcmp(tokens[i].value, "if") == 0) {
            Node* ifStatement = malloc(sizeof(Node));
            strcpy(ifStatement->type, "IfStatement");
            strcpy(ifStatement->value, "");

            i += 2; // Skip 'if' and '('
            char condition[100] = "";
            while (i < tokenCount && strcmp(tokens[i].type, "punctuation") != 0) {
                strcat(condition, tokens[i].value);
                i++;
            }
            strcpy(ifStatement->value, condition);

            i += 2; // Skip ')' and '{'
            Node* body = malloc(sizeof(Node));
            strcpy(body->type, "Block");
            body->next = NULL;
            Node* bodyCurrent = body;

            while (i < tokenCount && strcmp(tokens[i].type, "punctuation") != 0) {
                Node* statement = parser(&tokens[i], tokenCount - i);
                bodyCurrent->next = statement;
                bodyCurrent = statement;
                while (statement->next != NULL) {
                    statement = statement->next;
                }
                i += 1; // Increment by 1 to move to the next token
            }

            ifStatement->body = body;
            current->next = ifStatement;
            current = ifStatement;
        } else if (strcmp(tokens[i].type, "keyword") == 0 && strcmp(tokens[i].value, "while") == 0) {
            Node* whileStatement = malloc(sizeof(Node));
            strcpy(whileStatement->type, "WhileStatement");
            strcpy(whileStatement->value, "");

            i += 2; // Skip 'while' and '('
            char condition[100] = "";
            while (i < tokenCount && strcmp(tokens[i].type, "punctuation") != 0) {
                strcat(condition, tokens[i].value);
                i++;
            }
            strcpy(whileStatement->value, condition);

            i += 2; // Skip ')' and '{'
            Node* body = malloc(sizeof(Node));
            strcpy(body->type, "Block");
            body->next = NULL;
            Node* bodyCurrent = body;

            while (i < tokenCount && strcmp(tokens[i].type, "punctuation") != 0) {
                Node* statement = parser(&tokens[i], tokenCount - i);
                bodyCurrent->next = statement;
                bodyCurrent = statement;
                while (statement->next != NULL) {
                    statement = statement->next;
                }
                i += 1; // Increment by 1 to move to the next token
            }

            whileStatement->body = body;
            current->next = whileStatement;
            current = whileStatement;
        } else if (strcmp(tokens[i].type, "keyword") == 0 && strcmp(tokens[i].value, "for") == 0) {
            Node* forStatement = malloc(sizeof(Node));
            strcpy(forStatement->type, "ForStatement");
            strcpy(forStatement->value, "");

            i += 2; // Skip 'for' and '('
            char init[256] = "";        // Increased buffer sizes
            char condition[256] = "";
            char increment[256] = "";
            
            // Parse initialization
            while (i < tokenCount && strcmp(tokens[i].type, "punctuation") != 0 && strcmp(tokens[i].value, ";") != 0) {
                if (strlen(init) > 0) strcat(init, " "); // Add space between tokens
                strcat(init, tokens[i].value);
                i++;
            }
            i++; // Skip ';'
            
            // Parse condition
            while (i < tokenCount && strcmp(tokens[i].type, "punctuation") != 0 && strcmp(tokens[i].value, ";") != 0) {
                if (strlen(condition) > 0) strcat(condition, " "); // Add space between tokens
                strcat(condition, tokens[i].value);
                i++;
            }
            i++; // Skip ';'
            
            // Parse increment
            while (i < tokenCount && strcmp(tokens[i].type, "punctuation") != 0) {
                if (strlen(increment) > 0) strcat(increment, " "); // Add space between tokens
                strcat(increment, tokens[i].value);
                i++;
            }

            // Use snprintf with proper size checking
            size_t remaining = sizeof(forStatement->value);
            char forValue[512] = ""; // Temporary buffer for building the value
            
            int written = snprintf(forValue, remaining, "%s; %s; %s", init, condition, increment);
            if (written > 0 && written < remaining) {
                strcpy(forStatement->value, forValue);
            }

            i += 2; // Skip ')' and '{'
            Node* body = malloc(sizeof(Node));
            strcpy(body->type, "Block");
            body->next = NULL;
            Node* bodyCurrent = body;

            while (i < tokenCount && strcmp(tokens[i].type, "punctuation") != 0) {
                Node* statement = parser(&tokens[i], tokenCount - i);
                bodyCurrent->next = statement;
                bodyCurrent = statement;
                while (statement->next != NULL) {
                    statement = statement->next;
                }
                i += 1; // Increment by 1 to move to the next token
            }

            forStatement->body = body;
            forStatement->next = NULL;  // Initialize next pointer
            Node* currentNode = (Node*)current;  // Cast current to Node*
            currentNode->next = forStatement;
            current = forStatement;
        } else if (strcmp(tokens[i].type, "keyword") == 0 && strcmp(tokens[i].value, "do") == 0) {
            Node* doWhileStatement = malloc(sizeof(Node));
            strcpy(doWhileStatement->type, "DoWhileStatement");
            strcpy(doWhileStatement->value, "");

            i += 2; // Skip 'do' and '{'
            Node* body = malloc(sizeof(Node));
            strcpy(body->type, "Block");
            body->next = NULL;
            Node* bodyCurrent = body;

            while (i < tokenCount && strcmp(tokens[i].type, "punctuation") != 0) {
                Node* statement = parser(&tokens[i], tokenCount - i);
                bodyCurrent->next = statement;
                bodyCurrent = statement;
                while (statement->next != NULL) {
                    statement = statement->next;
                }
                i += 1; // Increment by 1 to move to the next token
            }

            i += 2; // Skip '}' and 'while'
            char condition[100] = "";
            while (i < tokenCount && strcmp(tokens[i].type, "punctuation") != 0) {
                strcat(condition, tokens[i].value);
                i++;
            }
            strcpy(doWhileStatement->value, condition);

            doWhileStatement->body = body;
            current->next = doWhileStatement;
            current = doWhileStatement;
        } else if (strcmp(tokens[i].type, "keyword") == 0 && strcmp(tokens[i].value, "def") == 0) {
            Node* function = malloc(sizeof(Node));
            strcpy(function->type, "FunctionDeclaration");
            strcpy(function->name, tokens[++i].value);
            strcpy(function->value, "");

            i += 2; // Skip function name and '('
            Node* params = malloc(sizeof(Node));
            strcpy(params->type, "Parameters");
            params->next = NULL;
            Node* paramsCurrent = params;

            while (i < tokenCount && strcmp(tokens[i].type, "punctuation") != 0) {
                Node* param = malloc(sizeof(Node));
                strcpy(param->type, "Parameter");
                strcpy(param->name, tokens[i].value);
                paramsCurrent->next = param;
                paramsCurrent = param;
                i++;
            }

            function->params = params;

            i += 2; // Skip ')' and '{'
            Node* body = malloc(sizeof(Node));
            strcpy(body->type, "Block");
            body->next = NULL;
            Node* bodyCurrent = body;

            while (i < tokenCount && strcmp(tokens[i].type, "punctuation") != 0) {
                Node* statement = parser(&tokens[i], tokenCount - i);
                bodyCurrent->next = statement;
                bodyCurrent = statement;
                while (statement->next != NULL) {
                    statement = statement->next;
                }
                i += 1; // Increment by 1 to move to the next token
            }

            function->body = body;
            current->next = function;
            current = function;
        }
    }

    return ast;
}

void codeGen(Node* node, char* code) {
    Node* current = node->next;
    while (current != NULL) {
        if (strcmp(current->type, "Declaration") == 0) {
            sprintf(code + strlen(code), "int %s = %s;\n", current->name, current->value); // Changed to int
        } else if (strcmp(current->type, "PrintStatement") == 0) {
            sprintf(code + strlen(code), "printf(\"%%d\\n\", %s);\n", current->name);
        } else if (strcmp(current->type, "IfStatement") == 0) {
            sprintf(code + strlen(code), "if (%s) {\n", current->value);
            codeGen(current->body, code);
            sprintf(code + strlen(code), "}\n");
        } else if (strcmp(current->type, "WhileStatement") == 0) {
            sprintf(code + strlen(code), "while (%s) {\n", current->value);
            codeGen(current->body, code);
            sprintf(code + strlen(code), "}\n");
        } else if (strcmp(current->type, "ForStatement") == 0) {
            sprintf(code + strlen(code), "for (%s) {\n", current->value);
            codeGen(current->body, code);
            sprintf(code + strlen(code), "}\n");
        } else if (strcmp(current->type, "DoWhileStatement") == 0) {
            sprintf(code + strlen(code), "do {\n");
            codeGen(current->body, code);
            sprintf(code + strlen(code), "} while (%s);\n", current->value);
        } else if (strcmp(current->type, "FunctionDeclaration") == 0) {
            sprintf(code + strlen(code), "void %s(", current->name);
            Node* param = current->params->next;
            while (param != NULL) {
                sprintf(code + strlen(code), "int %s", param->name);
                if (param->next != NULL) {
                    sprintf(code + strlen(code), ", ");
                }
                param = param->next;
            }
            sprintf(code + strlen(code), ") {\n");
            codeGen(current->body, code);
            sprintf(code + strlen(code), "}\n");
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