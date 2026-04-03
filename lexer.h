#ifndef LEXER_H
#define LEXER_H

#define MAX_TOKENS 100

typedef struct {
    char type[20];
    char value[20];
} Token;

extern Token tokens[MAX_TOKENS];
extern int tokenCount;

void tokenize(char *input);
void printTokens();

#endif