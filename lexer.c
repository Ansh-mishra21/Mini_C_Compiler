#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include "lexer.h"

Token tokens[MAX_TOKENS];
int tokenCount = 0;

// 🔥 UPDATED KEYWORDS
char keywords[][10] = {"int", "if", "else", "while", "for"};
int keywordCount = 5;

// check if keyword
int isKeyword(char *str) {
    for(int i = 0; i < keywordCount; i++) {
        if(strcmp(str, keywords[i]) == 0)
            return 1;
    }
    return 0;
}

// add token safely
void addToken(char *type, char *value) {
    if(tokenCount >= MAX_TOKENS) {
        printf("Error: Too many tokens\n");
        return;
    }

    strcpy(tokens[tokenCount].type, type);
    strcpy(tokens[tokenCount].value, value);
    tokenCount++;
}

// main tokenizer
void tokenize(char *input) {

    tokenCount = 0;   // reset tokens
    int i = 0;

    while(input[i] != '\0') {

        // 🔹 skip spaces
        if(isspace(input[i])) {
            i++;
            continue;
        }

        // 🔹 identifiers / keywords
        if(isalpha(input[i])) {
            char buffer[20] = {0};
            int j = 0;

            while(isalnum(input[i])) {
                if(j < 19)
                    buffer[j++] = input[i];
                i++;
            }
            buffer[j] = '\0';

            if(isKeyword(buffer))
                addToken("KEYWORD", buffer);
            else
                addToken("ID", buffer);
        }

        // 🔹 numbers
        else if(isdigit(input[i])) {
            char buffer[20] = {0};
            int j = 0;

            while(isdigit(input[i])) {
                if(j < 19)
                    buffer[j++] = input[i];
                i++;
            }
            buffer[j] = '\0';

            addToken("NUM", buffer);
        }

        // 🔥 LOGICAL OPERATORS (&&, ||)
        else if(input[i] == '&' && input[i+1] == '&') {
            addToken("OP", "&&");
            i += 2;
        }
        else if(input[i] == '|' && input[i+1] == '|') {
            addToken("OP", "||");
            i += 2;
        }

        // 🔥 RELATIONAL OPERATORS (<, >, <=, >=, ==)
        else if(input[i] == '<' || input[i] == '>' || input[i] == '=') {
            char op[3] = {0};
            op[0] = input[i];

            if(input[i+1] == '=') {
                op[1] = '=';
                i++;
            }

            addToken("OP", op);
            i++;
        }

        // 🔥 NOT and !=
        else if(input[i] == '!') {
            char op[3] = {0};
            op[0] = '!';

            if(input[i+1] == '=') {
                op[1] = '=';
                i++;
            }

            addToken("OP", op);
            i++;
        }

        // 🔹 arithmetic + punctuation
        else if(strchr("+-*/();{}", input[i])) {
            char op[2] = {input[i], '\0'};
            addToken("OP", op);
            i++;
        }

        // ❌ unknown character
        else {
            printf("Unknown character: %c\n", input[i]);
            i++;
        }
    }
}

// print tokens
void printTokens() {
    printf("\nTOKENS:\n");
    for(int i = 0; i < tokenCount; i++) {
        printf("%s : %s\n", tokens[i].type, tokens[i].value);
    }
}