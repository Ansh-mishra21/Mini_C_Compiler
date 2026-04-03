#include <stdio.h>
#include <string.h>
#include "lexer.h"
#include "parser.h"
#include "symbol.h"
#include "icg.h"

extern int pos;
extern int symCount;
extern int line;
extern int tempCount;

int main() {

    char input[1000] = "";
    char lineInput[100];

    printf("Enter C code (type END to finish):\n");

    while(1) {
        fgets(lineInput, sizeof(lineInput), stdin);

        // remove newline
        lineInput[strcspn(lineInput, "\n")] = 0;

        if(strcmp(lineInput, "END") == 0)
            break;

        strcat(input, lineInput);
        strcat(input, " ");   // space add karo
    }

    // 🔥 RESET
    pos = 0;
    symCount = 0;
    line = 0;
    tempCount = 1;

    tokenize(input);
    printTokens();

    parse();

    printSymbolTable();
    printICG();

    return 0;
}