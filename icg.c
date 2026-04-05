#include <stdio.h>
#include <string.h>

#define MAX 100

char code[MAX][100];   // 🔥 FIXED SIZE
int line = 0;
int tempCount = 1;
int labelCount = 1;

char temps[MAX][10];
char labels[MAX][10];

// 🔥 generate temporary variable
char* generateTemp(char *arg1, char *op, char *arg2) {

    if(tempCount >= MAX) {
        printf("Error: Too many temporaries\n");
        return NULL;
    }

    if(line >= MAX) {
        printf("Error: Too many instructions\n");
        return NULL;
    }

    char arg1Copy[20], arg2Copy[20];

    strcpy(arg1Copy, arg1);
    strcpy(arg2Copy, arg2);

    sprintf(temps[tempCount], "t%d", tempCount);

    sprintf(code[line], "%s = %s %s %s",
            temps[tempCount], arg1Copy, op, arg2Copy);

    line++;

    char* temp = temps[tempCount];   // 🔥 FIXED RETURN
    tempCount++;

    return temp;
}

// 🔥 generate label
char* newLabel() {

    if(labelCount >= MAX) {
        printf("Error: Too many labels\n");
        return NULL;
    }

    sprintf(labels[labelCount], "L%d", labelCount);
    return labels[labelCount++];
}

// 🔥 emit custom instruction
void emit(char *instruction) {

    if(line >= MAX) {
        printf("Error: Too many instructions\n");
        return;
    }

    strcpy(code[line++], instruction);
}

// 🔥 reset (IMPORTANT)
void resetICG() {
    line = 0;
    tempCount = 1;
    labelCount = 1;
}

// print 3-address code
void printICG() {
    printf("\n3-ADDRESS CODE:\n");
    for(int i = 0; i < line; i++) {
        printf("%s\n", code[i]);
    }
}