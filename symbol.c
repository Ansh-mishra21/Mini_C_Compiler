#include <stdio.h>
#include <string.h>

#define MAX_SYMBOLS 100

char symbolTable[MAX_SYMBOLS][20];
int symCount = 0;

// add symbol safely
void addSymbol(char *name) {

    // check duplicate
    for(int i = 0; i < symCount; i++) {
        if(strcmp(symbolTable[i], name) == 0)
            return;
    }

    // check overflow
    if(symCount >= MAX_SYMBOLS) {
        printf("Error: Symbol table overflow\n");
        return;
    }

    strcpy(symbolTable[symCount++], name);
}

// print symbol table
void printSymbolTable() {
    printf("\nSYMBOL TABLE:\n");

    if(symCount == 0) {
        printf("(empty)\n");
        return;
    }

    for(int i = 0; i < symCount; i++) {
        printf("%d. %s\n", i + 1, symbolTable[i]);
    }
}